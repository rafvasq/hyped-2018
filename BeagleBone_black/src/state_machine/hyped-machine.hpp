
/*
 * Authors: Yash Mittal and Ragnor Comerford
 * Organisation: HYPED
 * Date: 11. February 2018
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

#pragma once

#include "state_machine/event.hpp"
#include "state_machine/machine-states.hpp"

namespace hyped {
namespace state_machine {

class State;
class HypedMachine {
  friend class State;

 public:
  HypedMachine();
  void handleEvent(Event event);
  void transition(State *state);

 private:
  State *current_state;
};

}}   // namespace hyped::state_machine