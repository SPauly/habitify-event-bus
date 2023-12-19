// habitify-event-bus - Event bus system from
// Copyright (C) 2023  Simon Pauly
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

#ifndef HABITIFY_EVENT_BUS_IMPL_HABITIFY_EVENT_BASE_H_
#define HABITIFY_EVENT_BUS_IMPL_HABITIFY_EVENT_BASE_H_

#include <cstdint>

namespace habitify_event_bus {

using EventType = uint64_t;

namespace internal {
class EventBase {
 public:
  EventBase() = default;
  EventBase(EventType etype) : event_type_(etype) {}
  virtual ~EventBase() {}

  // Getters and setters
  inline const EventType &get_event_type() const { return event_type_; }

 private:
  const EventType event_type_;
};
}  // namespace internal
}  // namespace habitify_event_bus

#endif  // HABITIFY_EVENT_BUS_IMPL_HABITIFY_EVENT_BASE_H_