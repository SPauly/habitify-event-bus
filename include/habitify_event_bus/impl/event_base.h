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
#include <memory>
#include <typeindex>

namespace habitify_event_bus {

using EventConstBasePtr = std::shared_ptr<const EventBase>;

namespace internal {
using EventId = uint64_t;

class EventBase {
 public:
  EventBase() = default;
  EventBase(const EventId eId) : event_id_(eId) {}
  // EventId might be set later by the EventBroker, so it is not required
  EventBase(const std::type_info eType, const EventId eId = 0)
      : EventBase(eId), event_type(eType) {}
  virtual ~EventBase() {}

  // Getters
  inline const EventId get_id() const { return event_id_; }
  inline const std::type_index get_event_type() const { return event_type; }
  inline const ::PublisherId get_publisher_id() const { return pub_id_; }

  // Setters
  /// Sets the EventId to the given value only if the id hasn't been set yet to
  /// avoid changing ids.
  bool set_id(const EventId eId) {
    return (event_id != 0) ? false : event_id_ = eId;
  }
  /// PublisherId may only be set once. Returns if the attempt failed.
  bool set_publisher_id(const ::PublisherId pId) {
    return (pub_id_ != 0) ? false : pub_id_ = pId;
  }

 private:
  EventId event_id_ = 0;
  std::type_index event_type;

  // Metadata
  ::PublisherId pub_id_ = 0;
};
}  // namespace internal
}  // namespace habitify_event_bus

#endif  // HABITIFY_EVENT_BUS_IMPL_HABITIFY_EVENT_BASE_H_