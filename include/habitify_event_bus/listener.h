// habitify-event-bus - Event bus system from
// <https://github.com/SPauly/Habitify> Copyright (C) 2023  Simon Pauly
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
#include <functional>
#include <memory>
#include <shared_mutex>
#include <unordered_map>

#include <habitify_event_bus/actor_ids.h>
#include <habitify_event_bus/event.h>
#include <habitify_event_bus/impl/Channel.h>

namespace habitify_event_bus {
template <typename T>
using EventPtr<T> = std::shared_ptr<const Event<T>>;

class EventBusImpl;

/// Listener is used to read events from the Publisher. It is designed to be
/// thread safe. Usage:
/// TODO: Add usage example
class Listener : public std::enable_shared_from_this<Listener> {
 public:
  virtual ~Listener() = default;

  // Listener is not copyable due to the use of std::shared_mutex
  Listener(const Listener&) = delete;
  const Listener& operator=(const Listener&) = delete;

  /// TODO: void ChangeSubscription(const ChannelId& id);

  /// Returns a copy of the latest event. If there are no events it returns
  /// nullptr.
  template <typename T>
  const EventPtr<T> ReadLatest(const EventType event_t) const;

  /// Returns the latest event after removing it from the Channel. If there are
  /// no events it returns nullptr.
  template <typename T>
  EventPtr<T> ReadLatestAndRemove(const EventType event_t);

  /// Blocks the calling call and waits for incoming events of type EvType.
  /// Returns nullptr if an error occours.
  template <typename T>
  const EventPtr<T> Wait(const EventType event_t) const;

  // Asynchronously waits for an incoming event and runs the provided callback
  // with the incoming event. Returns the status of the Channel.
  template <typename T>
  const ChannelStatus Listen(std::function<void(EventPtr<T>)> callback);

  bool HasUnreadEvent(const EventType event_t) const;

  // Getters
  inline const ListenerId get_id() { return kId_; }

 private:
  friend class EventBusImpl;

  Listener() = delete;
  Listener::Listener(const ListenerId id,
                     std::shared_ptr<EventBusImpl> event_bus);

 private:
  mutable std::shared_mutex mux_;

  const ListenerId kId_;

  // Used to store the latest event for each event type that has been read.
  std::unordered_map<EventType, EventId> latest_event_ids_;

  std::shared_ptr<EventBusImpl> event_bus_;
};
}  // namespace habitify_event_bus

#endif  // HABITIFY_EVENT_BUS_INCLUDE_LISTENER_H_