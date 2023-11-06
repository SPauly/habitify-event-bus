// habitify-event-bus - Event bus system from
// <https://github.com/SPauly/Habitify> Copyright (C) 2023  Simon Pauly
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// Contact via <https://github.com/SPauly/habitify-event-bus>
#ifndef HABITIFY_EVENT_BUS_INCLUDE_EVENT_BUS_IMPL_H_
#define HABITIFY_EVENT_BUS_INCLUDE_EVENT_BUS_IMPL_H_

#include "include/port.h"

namespace habitify {
namespace internal {
class EventBusImpl {
 public:
  EventBusImpl();
  ~EventBusImpl();

 private:
  // Ports are stored together with their ID for fast lookups.
  std::unordered_map<internal::PortId, std::shared_ptr<internal::Port>> ports_;
};
}  // namespace internal
}  // namespace habitify

#endif  // HABITIFY_EVENT_BUS_INCLUDE_EVENT_BUS_IMPL_H_