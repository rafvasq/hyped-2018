/*
 * Author: Jack Horsburgh
 * Organisation: HYPED
 * Date: 17/07/18
 * Description: Main class for fake IMUs.
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

#include "sensors/fake_batteries.hpp"

#include "utils/timer.hpp"
#include "data/data.hpp"

namespace hyped {

using data::StripeCounter;

namespace sensors {

FakeBatteries::FakeBatteries(Logger& log, bool is_high_voltage, bool is_nominal)
    : data_(Data::getInstance()),
      is_started_(false),
      is_high_voltage_(is_high_voltage),
      voltage_(1100),
      current_(200),
      temperature_(30)
{}

void FakeBatteries::getData(Battery* battery)
{
  if (is_high_voltage_) {
    battery->voltage     = voltage_;
    battery->temperature = temperature_;
    battery->current     = current_;
  } else {
    battery->voltage     = 170;
    battery->temperature = temperature_;
    battery->current     = 200;
  }
}

bool FakeBatteries::isOnline()
{
  return true;
}

}}  // namespace hyped::sensors