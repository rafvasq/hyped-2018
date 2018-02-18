/*
 * Author: Sean Mullan and Jack Horsburgh
 * Organisation: HYPED
 * Date: 17/02/18
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

#include "motor_control/motor.hpp"
#include <iostream>

using namespace hyped {
using namespace motor_control {

Motor::Motor() 
{
  std::cout << "Motors initialised" << std::endl;
}

/**
  *  @brief  { Function will send RPM over CAN network to motor controllers }
  */
void Motor::setSpeed(int rpm) 
{
  std::cout << "RPM: " << rpm << std::endl;
}

}} // namespace hyped::motor_control
