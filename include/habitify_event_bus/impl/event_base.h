// habitify-event-bus - Lightweight event bus system developed for Habitify
//
// Copyright 2023 Simon Pauly
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
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
  EventBase(const std::type_info& eType) : event_type(eType) {}
  virtual ~EventBase() {}

  // Getters
  inline const EventId get_id() const { return event_id_; }
  inline const std::type_index get_event_type() const { return event_type_; }
  inline const PublisherId get_publisher_id() const { return pub_id_; }

  // Setters
  /// Sets the EventId to the given value only if the id hasn't been set yet to
  /// avoid changing ids.
  bool set_id(const EventId eId) {
    return (event_id != 0) ? false : event_id_ = eId;
  }
  /// Sets the EventType. Should be used with care and only to set the type
  /// once!
  void set_event_type(const std::type_info& eType) { event_type_ = eType; }
  /// PublisherId may only be set once. Returns if the attempt failed.
  bool set_publisher_id(const PublisherId pId) {
    return (pub_id_ != 0) ? false : pub_id_ = pId;
  }

 private:
  EventId event_id_ = 0;
  std::type_index event_type_;

  // Metadata
  PublisherId pub_id_ = 0;
};
}  // namespace internal
}  // namespace habitify_event_bus

#endif  // HABITIFY_EVENT_BUS_IMPL_HABITIFY_EVENT_BASE_H_