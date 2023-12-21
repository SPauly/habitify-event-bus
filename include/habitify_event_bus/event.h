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

#ifndef HABITIFY_EVENT_BUS_SRC_HABITIFY_EVENT_H_
#define HABITIFY_EVENT_BUS_SRC_HABITIFY_EVENT_H_
#include <habitify_event_bus/impl/event_base.h>

#include <memory>
#include <type_traits>
#include <shared_mutex>

namespace habitify_event_bus {
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
  Event(EventType etype, const DataT& data) : Event(etype), data_(data) {}
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
