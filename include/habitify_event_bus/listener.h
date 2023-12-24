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

#ifndef HABITIFY_EVENT_BUS_INCLUDE_LISTENER_H_
#define HABITIFY_EVENT_BUS_INCLUDE_LISTENER_H_

#include <cassert>
#include <cstdint>
#include <functional>
#include <memory>
#include <queue>
#include <shared_mutex>
#include <thread>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include <habitify_event_bus/impl/Channel.h>
#include <habitify_event_bus/event.h>
#include <habitify_event_bus/impl/event_bus_impl.h>
#include <habitify_event_bus/impl/id_types.h>

namespace habitify_event_bus {
// Forward declarations
class EventBus;

using ListenerId = uint64_t;
using ListenerPtr = std::shared_ptr<Listener>;
template <typename T>
using ListenerCallbackSig = std::function<void(const T&)>;

/// Listener is used to read events from the Publisher. It is designed to be
/// thread safe. Usage:
/// TODO: Add usage example
class Listener {
 public:
  // EventBus needs to access the private constructor to create Listener objects
  friend class EventBus;

  // The constructor is private to ensure that Listener objects can only be
  // created by the EventBus
  virtual ~Listener() = default;

  // Listener is not copyable due to the use of std::shared_mutex
  Listener(const Listener&) = delete;
  const Listener& operator=(const Listener&) = delete;

  // Operator overloads
  // Calls GetLatest internally and copies the event into the provided event.
  template <typename T>
  Event<const T>& operator>>(Event<const T>& event) const;

  /// Returns a ptr to the latest event as const instance. If there are no
  /// events it returns nullptr.
  template <typename T>
  const EventConstPtr<T> ReadLatest() const;

  /// Checks if an unread event is available and returns it if possible. This
  /// might return the same as ReadLatest. Returns nullptr if no Event exists or
  /// if the latest event was already read.
  template <typename T>
  const EventConstPtr<T> Read() const;

  /// Returns a copy of the latest event. If there are no events it returns an
  /// empty event.
  template <typename T>
  Event<const T>& GetLatest() const;

  /// Checks if an unread event is available. If so it copies the retrieved
  /// event to the provided event and returns true. This might be the same as
  /// GetLatest. Returns false if the latest event was read or no event exists.
  template <typename T>
  bool GetEvent(Event<const T>& event) const;

  /// Blocks the calling call and waits for incoming events of type EventType.
  /// Returns a shared_ptr to the const object. Otherwise
  /// returns nullptr if an error occours.
  template <typename T>
  const EventConstPtr<T> Wait() const;

  /// Blocks the calling call and waits for incoming events of type EventType.
  /// Internally calls GetEvent and copies the event into the provided event.
  template <typename T>
  const EventConstPtr<T> Wait(Event<const T>& event) const;

  // Asynchronously waits for an incoming event and runs the provided callback
  // with the incoming event. Returns the status of the Channel.
  template <typename T>
  const ChannelStatus Listen(ListenerCallbackSig callback);

  // Getters
  inline const internal::ListenerId get_id() { return kId_; }

 private:
  Listener() = delete;
  Listener::Listener(const internal::ListenerId id,
                     internal::EventBusImplPtr event_bus);

 private:
  // shared_mutex is used over standard mutex to allow multiple threads to read
  // simultaneously. And only one thread to write.
  mutable std::shared_mutex mux_;

  // Metadata
  const internal::ListenerId kId_;

  // Helpers
  internal::EventBusImplPtr event_bus_;

  // Stores the id of the latest event for each EventType that has been read so
  // far to safe the last reading position.
  std::unordered_map<const std::type_index, const EventId> latest_events_;

  // Store the channels that were used previously to avoid unnecessary lookups
  // by the event bus.
  std::unordered_map<const std::type_index, internal::ChannelPtr>
      used_channels_;

  // Callbacks for listening requests are stored with their respective data type
  // and are executed in FIFO.
  std::unordered_map<const std::type_index, std::queue<ListenerCallbackSig>>
      callbacks_;

  // For each listening request via listen() a thread is spun up per event type
  // that waits for incoming events and executes the coresponding callbacks.
  std::vector<std::thread> thread_pool_;
};
}  // namespace habitify_event_bus

#endif  // HABITIFY_EVENT_BUS_INCLUDE_LISTENER_H_