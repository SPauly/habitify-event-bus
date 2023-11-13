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
#include <memory>
#include <shared_mutex>

#include "include/actor_ids.h"
#include "include/habitify_event.h"
#include "include/port.h"

namespace habitify {

// forward declarations
class EventBusImpl;

/// Listener is used to read events from the Publisher. It is designed to be
/// thread safe. Usage:
/// TODO: Add usage example

class Listener : public std::enable_shared_from_this<Listener> {
 public:
  // EventBus needs access to the CreateSubscriber() function to properly
  // instantiate the Listener object
  friend class EventBusImpl;

  virtual ~Listener() = default;

  // Listener is not copyable due to the use of std::shared_mutex
  Listener(const Listener&) = delete;
  const Listener& operator=(const Listener&) = delete;

  /// Attempts to subscribe to the specified channel. If no Publisher is set it
  /// creates a new Channel. The Channel then calls RefreshPublisher() once the
  /// Publisher was added.
  /// TODO: void ChangeSubscription(const PortId& id);

  /// Returns the latest event published by the Publisher. If there are no
  /// events it returns nullptr.
  template <typename EvTyp>
  const std::shared_ptr<const Event<EvTyp>> ReadLatest() {}

  inline bool HasReceivedEvent();

  // Getters
  inline const bool get_is_subscribed() { return is_subscribed_; }
  inline const size_t get_read_index() { return read_index_; }

 private:
  Listener() = delete;
  Listener::Listener(std::shared_ptr<EventBusImpl> event_bus,
                     std::shared_ptr<internal::Port> port)
      : event_bus_(event_bus), port_(port), kId_(internal::GetListenerId());

 private:
  mutable std::shared_mutex mux_;

  const ListenerId kId_;
  bool is_subscribed_ = false;
  size_t read_index_ = 0;

  PortId port_id_ = 0;
  // This might be a nullptr if the port is blocked or not yet subscribed to
  std::shared_ptr<internal::Port> port_;

  std::shared_ptr<EventBusImpl> event_bus_;
};
}  // namespace habitify

#endif  // HABITIFY_EVENT_BUS_INCLUDE_LISTENER_H_