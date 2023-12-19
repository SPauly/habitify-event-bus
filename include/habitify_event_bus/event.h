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

namespace habitify_event_bus {

/// TODO: Add documentation + Move getters and setters from base class here for
/// a better interface
template <typename DataT>
class Event : public internal::EventBase {
 public:
  Event(EventType etype) : internal::EventBase(etype) {}
  Event(EventType etype, const DataT &data)
      : Event(etype), data_(std::make_shared<const DataT>(std::move(data))) {}
  virtual ~Event() {}

  /// MutableGetData() returns a mutable copy of the provided data. The
  /// requested type T must be of the same type as DataT of Event.
  template <typename T>
  std::shared_ptr<T> MutableGetData() {
    // Make sure T matches DataT
    static_assert(
        std::is_same_v<DataT, typename std::remove_const<
                                  typename std::remove_pointer<T>::type>::type>,
        "Type mismatch in MutableGetData! Requested type T must be of the same "
        "type as DataT of Event");

    // Create mutable copy of the data
    std::shared_ptr<T> mutable_copy = std::make_shared<T>(std::copy(*data_));

    return mutable_copy;
  }

  /// GetData provides immutable access to the underlying shared_ptr<const T>.
  /// Use MutableGetData() for mutable access to the data.
  template <typename T>
  const std::shared_ptr<const T> GetData() const {
    // Make sure T matches DataT
    static_assert(
        std::is_same_v<DataT, typename std::remove_const<
                                  typename std::remove_pointer<T>::type>::type>,
        "Type mismatch in GetData! Requested type T must be of the same "
        "type as DataT of Event");

    // Create mutable copy of the data
    return data_;
  }

 private:
  std::shared_ptr<const DataT> data_;
};

}  // namespace habitify_event_bus

#endif  // HABITIFY_EVENT_BUS_SRC_HABITIFY_EVENT_H_
