/*
 * Author: Martin Kristien and Jack Horsburgh
 * Organisation: HYPED
 * Date: 13/03/18
 * Description:
 *
 *    Copyright 2018 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "sensors/main.hpp"

#include "data/data.hpp"

namespace hyped {

using data::Data;
using data::Sensors;
using data::Batteries;

namespace sensors {

Main::Main(uint8_t id, Logger& log)
    : Thread(id, log),
      data_(data::Data::getInstance()),
      imu_manager_(id, log),
      proxi_manager_front_(id, log, true),
      proxi_manager_back_(id, log, false),
      battery_manager_lp(id, log)
{
  // Config new IMU manager
  imu_manager_.config(&sensors_.imu);

  // Config Proxi manager
  proxi_manager_front_.config(&sensors_.proxi_front);
  proxi_manager_back_.config(&sensors_.proxi_back);

  // Config BMS Manager
  battery_manager_lp.config(&batteries_.low_power_batteries);

  // Used for initialisation of old sensor and old battery data
  old_sensors_ = sensors_;
  old_batteries_ = batteries_;
}

void Main::run()
{
  while (1) {
    // Write sensor data to data structure only when all the imu and proxi values are different
    if (updateImu() || updateProxi()) {
      data_.setSensorsData(sensors_);
      old_sensors_ = sensors_;
      yield();
    }

    // Update battery data only when there is some change
    if (updateBattery()) {
      data_.setBatteryData(batteries_);
      old_batteries_ = batteries_;
      yield();
    }
  }
}

bool Main::updateImu()
{
  for (int i = 0; i < data::Sensors::kNumImus; i++) {
    if (old_sensors_.imu[i].acc.timestamp == sensors_.imu[i].acc.timestamp) {
      return false;
    }
  }
  return true;
}

bool Main::updateProxi()
{
    if (old_sensors_.proxi_front.timestamp == sensors_.proxi_front.timestamp) {
      return false;
    } else if (old_sensors_.proxi_back.timestamp == sensors_.proxi_back.timestamp) {
      return false;
    } else {
      return true;
    }
}

bool Main::updateBattery()
{
  for (int i = 0; i < data::Batteries::kNumLPBatteries; i++) {
    if (old_batteries_.low_power_batteries[i].voltage != batteries_.low_power_batteries[i].voltage
     || old_batteries_.low_power_batteries[i].temperature != batteries_.low_power_batteries[i].temperature) { //NOLINT
      return true;
    }
  }
  return false;
}

}}  // namespace hyped::sensors
