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

#include <memory>
#include <shared_mutex>
#include <unordered_map>

#include <habitify_event_bus/impl/id_types.h>
#include <habitify_event_bus/impl/event_base.h>
#include <habitify_event_bus/impl/event_bus_impl.h>
#include <habitify_event_bus/listener.h>
#include <habitify_event_bus/impl/Channel.h>
#include <habitify_event_bus/publisher.h>

namespace habitify_event_bus {
using ListenerPtr = std::shared_ptr<Listener>;
using PublisherPtr = std::shared_ptr<Publisher>;
using ListenerId = size_t;
using PublisherId = size_t;
using EventId = size_t;

class EventBus : public std::enable_shared_from_this<EventBus> {
 public:
  using EventBusImplPtr = std::shared_ptr<internal::EventBusImpl>;

  // EventBus() is private since this should only be created via Create().
  ~EventBus() = default;

  // EventBus is noncopyable
  EventBus(const EventBus&) = delete;
  EventBus& operator=(const EventBus&) = delete;

  /// Construct and access the EventBus via this function.
  /// NOTE that EventBus cannot be constructed via a constructor.
  /// It can be more efficient to store the returned shared_ptr for future use
  /// than to call this function.
  static std::shared_ptr<EventBus> Create() {
    /// TODO: Remove use of new keyword here
    return std::shared_ptr<EventBus>(new EventBus);
  }

  /// Attempts to create a listener object. This is the only way to
  /// obtain a Listener object.
  ListenerPtr EventBus::CreateListener() { return impl_->CreateListener(); }

  /// Attempts to create a publisher object.This is the only way to obtain a
  /// Publisher object.
  PublisherPtr CreatePublisher() { return impl_->CreatePublisher(); }

 private:
  // This is a singleton class so the constructor needs to be private.
  EventBus() : impl_(std::make_shared<internal::EventBusImpl>()) {}

 private:
  // The EventBusImpl is the actual implementation of the EventBus. It is used
  // internally for managing the Channels, publishers and listeners.
  EventBusImplPtr impl_;
};

}  // namespace habitify_event_bus

#endif  // HABITIFY_EVENT_BUS_HABITIFY_EVENT_BUS_H_
