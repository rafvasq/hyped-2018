/*
 * Author: Ragnor Comerford
 * Organisation: HYPED
 * Date: 19/06/18
 * Description:
 * Main manages sensor drivers, collects data from sensors and updates
 * shared Data::Sensors structure. Main is not responsible for initialisation
 * of supporting io drivers (i2c, spi, can). This should be done by the sensor
 * drivers themselves.
 * Currently supported sensors:
 * - BMS (low powered), ids: 0
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

#ifndef BEAGLEBONE_BLACK_SENSORS_GPIO_COUNTER_HPP_
#define BEAGLEBONE_BLACK_SENSORS_GPIO_COUNTER_HPP_

#include <cstdint>

#include "utils/concurrent/thread.hpp"
#include "data/data.hpp"


namespace hyped {

using utils::concurrent::Thread;
using utils::Logger;

namespace sensors {


class GpioCounter: public Thread {
 public:
  explicit GpioCounter(Logger& log, int pin);
  void run() override;
  data::StripeCounter getStripeCounter();

 private:
  int pin_;

  data::StripeCounter stripe_counter_;
};
}}  // namespace hyped::sensors

#endif  // BEAGLEBONE_BLACK_SENSORS_GPIO_COUNTER_HPP_