/*
 * Author: Martin Kristien
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

#ifndef BEAGLEBONE_BLACK_SENSORS_MAIN_HPP_
#define BEAGLEBONE_BLACK_SENSORS_MAIN_HPP_

#include <cstdint>

#include "utils/concurrent/thread.hpp"
#include "data/data.hpp"

namespace hyped {

using utils::concurrent::Thread;

namespace sensors {

class Main: public Thread {
 public:
  explicit Main(uint8_t id);
  void run() override;

 private:
  data::Data& data_;
};

}}  // namespace hyped::sensors

#endif  /* BEAGLEBONE_BLACK_SENSORS_MAIN_HPP_ */