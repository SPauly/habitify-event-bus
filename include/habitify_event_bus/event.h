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
#ifndef HABITIFY_EVENT_BUS_SRC_HABITIFY_EVENT_H_
#define HABITIFY_EVENT_BUS_SRC_HABITIFY_EVENT_H_
#include <habitify_event_bus/impl/event_base.h>

#include <cstdint>
#include <memory>
#include <typeindex>
#include <type_traits>
#include <shared_mutex>

namespace habitify_event_bus {
using EventId = uint64_t;
template <typename T>
using EventConstPtr = std::shared_ptr<const Event<T>>;

namespace internal {
/// IsSameEventType is a helper template that checks if the provided type T is
/// the same as the type of the data of the event.
template <typename Data_t, typename T>
inline constexpr bool IsSameEventType = std::is_same_v<Data_t, T>;
}  // namespace internal

/// Event is a templated class that represents an event that can be published
/// TODO: implement Event<T> answer function that creates a new Event type with
/// the metadata set to represent a response.
template <typename DataT>
class Event : public internal::EventBase {
 public:
  Event() = delete;
  Event(const DataT& data) : EventBase(typeid(DataT)), data_(data) {}
  virtual ~Event() {}

  /// MutableGetData() returns a mutable copy of the provided data. The
  /// requested type T must be of the same type as DataT of Event.
  template <typename T>
  std::unique_ptr<T> MutableGetData() {
    // Make sure T matches DataT
    static_assert(
        internal::IsSameEventType<DataT, T>,
        "Type mismatch in MutableGetData! Requested type T must be of the same "
        "type as DataT of Event");

    // Create mutable copy of the data
    std::unique_ptr<T> mutable_copy = std::make_unique<T>(std::copy(data_));

    return std::move(mutable_copy);
  }

  /// GetData provides immutable access to the stored data.
  /// Use MutableGetData() for mutable access to the data.
  template <typename T>
  const T& GetData() const {
    // Make sure T matches DataT
    static_assert(
        internal::IsSameEventType<DataT, T>,
        "Type mismatch in GetData! Requested type T must be of the same "
        "type as DataT of Event");

    return data_;
  }

 private:
  const DataT data_;
};

/// EventTypes used for Testing
#if defined(HABITIFY_EVENT_BUS_TESTING)
namespace habitify_testing {
namespace TestEvents {
struct OK {};
struct ERROR {};
struct TEST {
  int a;
  std::string b;
};
}  // namespace TestEvents
}  // namespace habitify_testing
#endif  // defined(HABITIFY_EVENT_BUS_TESTING)

}  // namespace habitify_event_bus

#endif  // HABITIFY_EVENT_BUS_SRC_HABITIFY_EVENT_H_
