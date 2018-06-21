/*
 * Author: Sean Mullan and Jack Horsburgh
 * Organisation: HYPED
 * Date: 17/02/18
 * Description:
 *
 *    Copyright 2018 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 *    except in compliance with the License. You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software distributed under
 *    the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 *    either express or implied. See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "motor_control/main.hpp"

#include <cstdint>

#include "motor_control/communicator.hpp"
#include "data/data.hpp"
#include "utils/system.hpp"

namespace hyped {

using data::NavigationType;
using utils::System;

namespace motor_control {

Main::Main(uint8_t id, Logger& log)
    : Thread(id, log),
      data_(data::Data::getInstance()),
      post_calibration_barrier_(System::getSystem().navigation_motors_sync_),
      communicator_(log),
      target_velocity_(0),
      target_torque_(0),
      run_(true),
      nav_calib_(false),
      motors_init_(false),
      motors_ready_(false),
      motor_failure_(false),
      all_motors_stopped_(false)
{
  state_      = data_.getStateMachineData();
  motor_data_ = data_.getMotorData();
  motor_data_.module_status = data::ModuleStatus::kStart;
  motor_data_.velocity_1 = 0;
  motor_data_.velocity_2 = 0;
  motor_data_.velocity_3 = 0;
  motor_data_.velocity_4 = 0;
  motor_data_.torque_1 = 0;
  motor_data_.torque_2 = 0;
  motor_data_.torque_3 = 0;
  motor_data_.torque_4 = 0;
  data_.setMotorData(motor_data_);
  motor_velocity_ = {0, 0, 0, 0};
  motor_torque_   = {0, 0, 0, 0};
}

void Main::run()
{
  log_.INFO("MOTOR", "Starting motor controller");
  while (run_) {
    state_ = data_.getStateMachineData();
    if (state_.current_state == data::State::kIdle) {
      this->initMotors();
      yield();
    } else if (state_.current_state == data::State::kCalibrating) {
      this->prepareMotors();
      yield();
    } else if (state_.current_state == data::State::kAccelerating) {
      this->accelerateMotors();
    } else if (state_.current_state == data::State::kDecelerating) {
      this->decelerateMotors();
    } else if (state_.current_state == data::State::kRunComplete) {
      // Wait for state machine to transition to kExiting
    } else if (state_.current_state == data::State::kExiting) {
      this->servicePropulsion();
    } else if (state_.current_state == data::State::kEmergencyBraking) {
      this->stopMotors();
    } else if (state_.current_state == data::State::kFailureStopped) {
      communicator_.enterPreOperational();
    } else {
      run_ = false;
    }
  }
}

void Main::initMotors()
{
  if (!motors_init_ && !motor_failure_) {
    // Register controllers on CAN Bus
    communicator_.registerControllers();

    // Configure controller parameters
    communicator_.configureControllers();

    // If a failure occured during configuration, set motor status to critical failure
    if (communicator_.getFailure()) {
      updateMotorFailure();
    // Otherwise update motor status to initialised
    } else {
      motor_data_.module_status = data::ModuleStatus::kInit;
      data_.setMotorData(motor_data_);
      motors_init_ = true;
      log_.INFO("MOTOR", "Motor State: Idle");
    }
  }
}

void Main::prepareMotors()
{
  if (!motors_ready_ && !motor_failure_) {
    // Set motors into operational mode
    communicator_.prepareMotors();

    // Check for any errors and warning
    communicator_.healthCheck();

    // If there is an error or warning, set motor status to critical failure
    if (communicator_.getFailure()) {
      updateMotorFailure();
    // Otherwise set motor status to ready
    } else {
      motor_data_.module_status = data::ModuleStatus::kReady;
      data_.setMotorData(motor_data_);
      motors_ready_ = true;
      log_.INFO("MOTOR", "Motor State: Ready");
    }
  }
}

void Main::accelerateMotors()
{
  // Hit the barrier to sync with navigation calibration
  if (!nav_calib_) {
    post_calibration_barrier_.wait();
    nav_calib_ = true;
  }

  log_.INFO("MOTOR", "Motor State: Accelerating\n");
  while (state_.current_state == data::State::kAccelerating) {
    // Check for state machine critical failure flag
    state_ = data_.getStateMachineData();
    if (state_.critical_failure) {
      this->stopMotors();
      break;
    }

    // Check for motors critial failure flag
    communicator_.healthCheck();

    // If a failure occurs in any motor, set motor status to critical failure
    //  and stop all motors
    if (communicator_.getFailure()) {
      updateMotorFailure();
      this->stopMotors();
      break;
    }

    // Otherwise step up motor velocity
    log_.DBG2("MOTOR", "Motor State: Accelerating\n");
    data::Navigation nav_ = data_.getNavigationData();
    target_velocity_      = accelerationVelocity(nav_.velocity);
    target_torque_        = accelerationTorque(nav_.velocity);
    communicator_.sendTargetVelocity(target_velocity_);
    communicator_.sendTargetTorque(target_torque_);
    updateMotorData();
  }
}

void Main::decelerateMotors()
{
  log_.INFO("MOTOR", "Motor State: Deccelerating\n");
  while (state_.current_state == data::State::kDecelerating) {
    // Check for state machine critical failure flag
    state_ = data_.getStateMachineData();
    if (state_.critical_failure) {
      this->stopMotors();
      break;
    }

    // Check for motors critical failure flag
    communicator_.healthCheck();

    // If a failure occurs in any motor, set motor status to critical failure
    //  and stop all motors
    if (communicator_.getFailure()) {
      updateMotorFailure();
      this->stopMotors();
      break;
    }

    // Otherwise step down motor velocity
    log_.DBG2("MOTOR", "Motor State: Deccelerating\n");
    data::Navigation nav_ = data_.getNavigationData();
    target_velocity_      = decelerationVelocity(nav_.velocity);
    target_torque_        = decelerationTorque(nav_.velocity);
    communicator_.sendTargetVelocity(target_velocity_);
    communicator_.sendTargetTorque(target_torque_);
    updateMotorData();
  }
}

void Main::stopMotors()
{
  communicator_.quickStopAll();
  // Updates the motor data while motors are stopping
  while (!all_motors_stopped_) {
    log_.DBG2("MOTOR", "Motor State: Stopping\n");
    updateMotorData();

    if (motor_velocity_.velocity_1 == 0 && motor_velocity_.velocity_2 == 0 &&
        motor_velocity_.velocity_3 == 0 && motor_velocity_.velocity_4 == 0)
    {
      all_motors_stopped_ = true;
      log_.INFO("MOTOR", "Motor State: Stopped\n");
    }
  }
  updateMotorData();
  communicator_.enterPreOperational();
}

int32_t Main::accelerationVelocity(NavigationType velocity)
{
  return target_velocity_ += 100;  // dummy calculation to increase rpm
}

int32_t Main::decelerationVelocity(NavigationType velocity)
{
  return target_velocity_ -= 100;  // dummy calculation to decrease rpm
}

int16_t Main::accelerationTorque(NavigationType velocity)
{
  return 0;
}

int16_t Main::decelerationTorque(NavigationType velocity)
{
  return 0;
}

void Main::servicePropulsion()
{
  // TODO(Anyone) Implement service propulsion
}

void Main::updateMotorData()
{
  motor_velocity_   = communicator_.requestActualVelocity();
  motor_torque_     = communicator_.requestActualTorque();
  // Write motor data to data structure
  motor_data_.velocity_1 = motor_velocity_.velocity_1;
  motor_data_.velocity_2 = motor_velocity_.velocity_2;
  motor_data_.velocity_3 = motor_velocity_.velocity_3;
  motor_data_.velocity_4 = motor_velocity_.velocity_4;
  motor_data_.torque_1 = motor_torque_.torque_1;
  motor_data_.torque_2 = motor_torque_.torque_2;
  motor_data_.torque_3 = motor_torque_.torque_3;
  motor_data_.torque_4 = motor_torque_.torque_4;
  data_.setMotorData(motor_data_);
}

void Main::updateMotorFailure()
{
  log_.ERR("MOTOR", "Motor State: MOTOR FAILURE\n");
  motor_data_.module_status = data::ModuleStatus::kCriticalFailure;
  data_.setMotorData(motor_data_);
  motor_failure_ = true;
}

}}  // namespace hyped::motor_control
