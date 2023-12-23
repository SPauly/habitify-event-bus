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

// This file exposes the interface to the EventBus
#ifndef HABITIFY_EVENT_BUS_HABITIFY_EVENT_BUS_H_
#define HABITIFY_EVENT_BUS_HABITIFY_EVENT_BUS_H_

#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>

#include <habitify_event_bus/impl/id_types.h>
#include <habitify_event_bus/impl/event_base.h>
#include <habitify_event_bus/impl/event_bus_impl.h>
#include <habitify_event_bus/listener.h>
#include <habitify_event_bus/impl/Channel.h>
#include <habitify_event_bus/publisher.h>

namespace habitify_event_bus {
using EventBusPtr = std::shared_ptr<EventBus>;

class EventBus {
 public:
  EventBus() : impl_(std::make_shared<internal::EventBusImpl>()) {}
  virtual ~EventBus() = default;

  /// Attempts to create a listener object.
  Listener EventBus::CreateListener();

  /// Attempts to create a shared_ptr<Listener> object.
  ListenerPtr EventBus::CreateSharedListener();

  /// Attempts to create a publisher object,
  Publisher CreatePublisher();

  /// Attempts to create a shared_ptr<Publisher> object.
  PublisherPtr CreateSharedPublisher();

 protected:
  /// Retrieves a free listener id. This is called when a new listener is
  /// created.
  const ListenerId GetFreeListenerId();
  /// Retrieves a free publisher id. This is called when a new publisher is
  /// created.
  const PublisherId GetFreePublisherId();

 private:
  // Mutex for thread safety of the counters
  mutable std::mutex mux_l_counter_, mux_p_counter;

  // Counters for Ids
  size_t publisher_counter_ = 0, listener_counter_ = 0;

  // The EventBusImpl is the actual implementation of the EventBus. It is used
  // internally for managing the Channels. It is not exposed to the user. Use
  // Publisher and Listener objects for reading and writing access to the
  // EventBus.
  std::shared_ptr<internal::EventBusImpl> impl_;
};

}  // namespace habitify_event_bus

#endif  // HABITIFY_EVENT_BUS_HABITIFY_EVENT_BUS_H_
