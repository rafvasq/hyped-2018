/*
 * Authors: M. Kristien
 * Organisation: HYPED
 * Date: 18. June 2018
 * Description:
 * Drive connected sensors, take and log all readings.
 * Sensors supported: Proxi, IMU, BMS LP and HP, SAM CAN/PRoxi
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

#include "data/data.hpp"

#include "sensors/bms.hpp"
#include "sensors/mpu9250.hpp"
#include "sensors/vl6180.hpp"
#include "sensors/can_proxi.hpp"

#include "utils/concurrent/thread.hpp"
#include "utils/system.hpp"
#include "utils/logger.hpp"

constexpr uint8_t BMS_LP = 2;
constexpr uint8_t BMS_HP = 1;

using hyped::data::Sensors;
using hyped::data::Batteries;
using hyped::data::Imu;
using hyped::data::Proximity;

using hyped::sensors::ProxiInterface;
using hyped::sensors::ImuInterface;
using hyped::sensors::BMSInterface;
using hyped::sensors::BMS;

using hyped::utils::concurrent::Thread;
using hyped::utils::Logger;

int main(int argc, char* argv[])
{
  hyped::utils::System::parseArgs(argc, argv);
  Logger& log = hyped::utils::System::getLogger();
  log.INFO("MAIN", "system started, logger created");

  // data
  Proximity proxi;
  Proximity proxi2;
  Imu       imu;
  Batteries batteries;

  log.INFO("MAIN", "creating sensors");
  log.INFO("MAIN", "creating VL6180");
  ProxiInterface* proxi_sensor = new hyped::sensors::VL6180(0x29);

  log.INFO("MAIN", "creating VL6180");
  ImuInterface* imu_sensor = new hyped::sensors::MPU9250(log, 66, 0x08, 0x00);
  

  log.INFO("MAIN", "creating BMS");
  BMS* bms;
  bms = new BMS(0);
  bms->start();
  BMSInterface* bms1 = bms;
  bms = new BMS(1);
  bms->start();
  BMSInterface* bms2 = bms;

  log.INFO("MAIN", "creating can proxi");
  ProxiInterface* proxi_can = new hyped::sensors::CanProxi(0);

  log.INFO("MAIN", "all sensors created, entering test loop");
  while (1) {
    proxi_sensor->getData(&proxi);
    proxi_can->getData(&proxi2);
    imu_sensor->getData(&imu);
    bms1->getData(&batteries.low_power_batteries[0]);
    bms2->getData(&batteries.low_power_batteries[1]);
    log.INFO("TEST", "proxi here distance: %u", proxi.val);
    log.INFO("TEST", "proxi can  distance: %u", proxi2.val);
    log.INFO("TEST", "imu acc %f %f %f",
      imu.acc.value[0], imu.acc.value[1], imu.acc.value[2]);
    log.INFO("TEST", "imu gyr %f %f %f",
      imu.gyr.value[0], imu.gyr.value[1], imu.gyr.value[2]);
    log.INFO("TEST", "bms0 voltage, current, temp: %d %d %d",
      batteries.low_power_batteries[0].voltage,
      batteries.low_power_batteries[1].current,
      batteries.low_power_batteries[0].temperature);
    log.INFO("TEST", "bms1 voltage, current, temp: %d %d %d",
      batteries.low_power_batteries[1].voltage,
      batteries.low_power_batteries[1].current,
      batteries.low_power_batteries[1].temperature);

    log.INFO("TEST", "");
    Thread::sleep(100);
  }
}

