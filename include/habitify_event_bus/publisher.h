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
// Contact via <https://github.com/SPauly/habitify-event-bus>#ifndef
// HABITIFY_EVENT_BUS_INCLUDE_PUBLISHER_H_
#define HABITIFY_EVENT_BUS_INCLUDE_PUBLISHER_H_

#include <cassert>
#include <cstdint>
#include <condition_variable>
#include <memory>
#include <shared_mutex>
#include <vector>

#include <habitify_event_bus/impl/Channel.h>
#include <habitify_event_bus/event.h>
#include <habitify_event_bus/impl/event_bus_impl.h>

namespace habitify_event_bus {
// Forward declarations
class EventBus;

using PublisherId = uint64_t;
using PublisherPtr = std::unique_ptr<Publisher>;

/// Publisher serves as an interface to publish data to different channels in
/// the event bus. Internally it preprocess the incoming data for faster
/// distribution by the event broker.
/// TODO: Add Usage:
class Publisher {
 public:
  // EventBus needs to access the private constructor to create Publisher
  // objects.
  friend class EventBus;

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
  /// and distribution by the EventBroker. T is used to determine the type of
  /// the event.
  template <typename T>
  bool Publish(const T& data) const;

  /// Publishers can close certain channels to prevent further publishing.
  /// This is useful to indicate when a certain event has finished.
  template <typename T>
  bool CloseChannel() const;

 private:
  Publisher() = delete;
  Publisher(const PublisherId id, internal::EventBusImplPtr event_bus_impl);

 private:
  // shared_mutex is used over standard mutex to allow multiple threads
  // to read simultaneously. And only one thread to write.
  mutable std::shared_mutex mux_;
  std::shared_ptr<std::condition_variable_any> cv_;

  // Metadata
  bool is_registered_ = false;
  const PublisherId kPublisherId_;

  // Helpers
  const internal::EventBusImplPtr ebus_impl_;
};

}  // namespace habitify_event_bus

#endif  // HABITIFY_EVENT_BUS_INCLUDE_PUBLISHER_H_