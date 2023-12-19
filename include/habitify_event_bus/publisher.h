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
#ifndef HABITIFY_EVENT_BUS_INCLUDE_PUBLISHER_H_
#define HABITIFY_EVENT_BUS_INCLUDE_PUBLISHER_H_

#include <cassert>
#include <condition_variable>
#include <memory>
#include <shared_mutex>
#include <vector>

#include <habitify_event_bus/impl/Channel.h>
#include <habitify_event_bus/event.h>
#include <habitify_event_bus/impl/event_bus_impl.h>
#include <habitify_event_bus/impl/id_types.h>

namespace habitify_event_bus {

/// Publisher serves as an interface to publish data to different channels in
/// the event bus. Internally it preprocess the incoming data for faster
/// distribution by the event broker.
/// TODO: Add Usage:
class Publisher {
 public:
  // The construtor is private to ensure that Publisher is only created via the
  // designated Create() function.
  virtual ~Publisher() = default;

  // Publisher is not copyable due to the use of std::shared_mutex
  Publisher(const Publisher&) = delete;
  const Publisher& operator=(const Publisher&) = delete;

  // Getters and Setters:
  const PublisherId get_id() const;

  /// template<typename>Publisher::Publish(const T& data)
  /// Copies the provided data and creates an Event of it for further storage
  /// and distribution. T is used to determine the type of the event.
  template <typename T>
  bool Publish(const T& data) const;

 private:
  Publisher() = delete;
  Publisher(const PublisherId id, internal::EventBusImplPtr event_bus_impl);

  /// Publisher()::Create() was made private to ensure that it is only created
  /// via the EventBus::CreatePublisher() function. This way we can enforce
  /// that Publisher is purely used as shared_ptr instance.
  std::shared_ptr<Publisher> Create(const PublisherId id,
                                    internal::EventBusImplPtr event_bus_impl) {
    return std::make_shared<Publisher>(id, event_bus_impl);
  }

 private:
  // shared_mutex is used over standard mutex to allow multiple threads
  // to read simultaneously. And only one thread to write.
  mutable std::shared_mutex mux_;
  std::shared_ptr<std::condition_variable_any> cv_;

  // Metadata
  bool is_registered_ = false;
  const PublisherId kPublisherId_;

  // Helpers
  const internal::EventBusImplPtr event_bus_;
};

}  // namespace habitify_event_bus

#endif  // HABITIFY_EVENT_BUS_INCLUDE_PUBLISHER_H_