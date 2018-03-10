/*
 * Organisation: HYPED
 * Date: 18/02/2018
 * Description: Class for data exchange between sub-team threads and structures for holding data
 *              produced by each of the sub-teams.
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

#ifndef BEAGLEBONE_BLACK_DATA_DATA_HPP_
#define BEAGLEBONE_BLACK_DATA_DATA_HPP_

#include <array>
#include <cstdint>

#include "data/data_point.hpp"
#include "utils/concurrent/lock.hpp"

namespace hyped {

// imports
using utils::concurrent::Lock;

namespace data {

// -----------------------------------------------------------------------------
// Navigation
// -----------------------------------------------------------------------------
struct Navigation {
  uint32_t distance;
  uint32_t velocity;
  int32_t acceleration;
  uint32_t stripe_count;
};

// -----------------------------------------------------------------------------
// Raw Sensor data
// -----------------------------------------------------------------------------

struct Imu {
  uint16_t acc_x;
  uint16_t acc_y;
  uint16_t acc_z;

  uint16_t gyr_x;
  uint16_t gyr_y;
  uint16_t gyr_z;
};

struct Proximity {
  uint8_t val;
};

/*struct StripeCount {
  DataPoint<uint32_t> count;
};//*/
typedef DataPoint<uint32_t> StripeCount;

struct Sensors {
  static constexpr int kNumImus = 8;
  static constexpr int kNumProximities = 24;

  std::array<Imu, kNumImus> imu;
  std::array<Proximity, kNumProximities> proxy;
  StripeCount stripe_cnt;
};

// -----------------------------------------------------------------------------
// Motor data
// -----------------------------------------------------------------------------
struct Motors {
  int32_t angular_velocity_FL;
  int32_t angular_velocity_FR;
  int32_t angular_velocity_BL;
  int32_t angular_velocity_BR;
};

// -----------------------------------------------------------------------------
// Common Data structure/class
// -----------------------------------------------------------------------------
/**
 * @brief      A singleton class managing the data exchange between sub-team threads.
 */
class Data {
 public:
  /**
   * @brief      Always returns a reference to the only instance of `Data`.
   */
  static Data& getInstance();

  /**
   * @brief      Retrieves data produced by navigation sub-team.
   */
  Navigation getNavigationData();
  /**
   * @brief      Should be called by navigation sub-team whenever they have new data.
   */
  void setNavigationData(const Navigation& nav_data);

  /**
   * @brief      Retrieves data from all sensors
   */
  Sensors getSensorsData();

  /**
   * @brief      Should be called to update sensor data
   */
  void setSensorsData(const Sensors& sensors_data);

  /**
   * @brief      Retrieves data produced by each of the four motors.
   */
  Motors getMotorData();

  /**
   * @brief      Should be called to update motor data.
   */
  void setMotorData(const Motors& motor_data);

 private:
  Navigation  navigation_;
  Sensors     sensors_;
  Motors      motors_;

  // locks for data substructures
  Lock lock_navigation_;
  Lock lock_sensors_;
  Lock lock_motors_;
};

}}  // namespace hyped::data

#endif  // BEAGLEBONE_BLACK_DATA_DATA_HPP_