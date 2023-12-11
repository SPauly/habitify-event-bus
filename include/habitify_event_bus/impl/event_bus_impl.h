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
#ifndef HABITIFY_EVENT_BUS_IMPL_EVENT_BUS_IMPL_H_
#define HABITIFY_EVENT_BUS_IMPL_EVENT_BUS_IMPL_H_

#include <cstddef>
#include <memory>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

#include <habitify_event_bus/listener.h>
#include <habitify_event_bus/impl/port.h>
#include <habitify_event_bus/publisher.h>

namespace habitify_event_bus {
using ListenerPtr = std::shared_ptr<Listener>;
template <typename EvTyp>
using PublisherPtr = std::shared_ptr<Publisher<EvTyp>>;
namespace internal {
using PortPtr = std::shared_ptr<Port>;
using PublisherBasePtr = std::shared_ptr<PublisherBase>;

/// The EventBusImpl is the implementation of the EventBus. It is not exposed to
/// the user. It manages the Listener and Publisher objects by matching them
/// with their respective ports.
class EventBusImpl final : public std::enable_shared_from_this<EventBusImpl> {
 public:
  EventBusImpl();
  virtual ~EventBusImpl();

  // EventBusImpl is noncopyable
  EventBusImpl(const EventBusImpl&) = delete;
  EventBusImpl& operator=(const EventBusImpl&) = delete;

  /// Attempts to create a listener object that is subscribed to the specified
  /// port. Returns nullptr if the port is blocked. The Listener object
  /// additionally is stored in the eventbus for proper life time management
  /// with multiple threads.
  ListenerPtr CreateListener(const PortId id);

  /// Attempts to create a publisher object that is subscribed to the specified
  /// port. Returns nullptr if the port is blocked or already has a publisher.
  /// The Publisher object additionally is stored in the eventbus for proper
  /// life time management with multiple threads.
  /// For id = -1 the next free port id is used.
  template <typename EvTyp>
  PublisherPtr<EvTyp> CreatePublisher(const PortId id = -1);

 private:
  /// Retrieves a free port id. This is called when a new port is created.
  const PortId GetFreePortId();
  /// Retrieves a free listener id. This is called when a new listener is
  /// created.
  const ListenerId GetFreeListenerId();
  /// Retrieves a free publisher id. This is called when a new publisher is
  /// created.
  const PublisherId GetFreePublisherId();

 private:
  // Mutexes for thread safety of the different actors
  // (mutable is needed for locking in const functions)
  mutable std::shared_mutex mux_publisher_, mux_listener_, mux_port_;

  // Mutex for thread safety of the counters
  mutable std::mutex mux_pid_counter_, mux_l_counter_, mux_p_counter;

  // Counters for Ids
  size_t portid_counter_ = 0, publisher_counter_ = 0, listener_counter_ = 0;

  // Ports are stored together with their ID for fast lookups.
  std::unordered_map<const PortId, PortPtr> ports_;

  // Publishers are stored together with their ID for fast lookups.
  std::unordered_map<const PublisherId, PublisherBasePtr> publishers_;

  // Listeners are stored together with their ID for fast lookups.
  std::unordered_map<const ListenerId, ListenerPtr> listeners_;
};

// Template definition of CreatePublisher
template <typename EvTyp>
PublisherPtr<EvTyp> EventBusImpl::CreatePublisher(const PortId id) {
  std::unique_lock<std::shared_mutex> lock(mux_publisher_);

  PortPtr port;

  // Create a new Port if it does not exist yet
  if (ports_.find(id) == ports_.end()) {
    // set port id to the next free port id if id = -1
    if (id == -1)
      port = std::make_shared<Port>(GetFreePortId());
    else
      port = std::make_shared<Port>(id);
  } else {
    // return nullptr if the port already has a publisher publishing to it
    if (ports_.at(id)->get_has_publisher()) return nullptr;

    // set the port properly otherwise
    port = ports_.at(id);
  }

  // Create publisher
  auto publisher = Publisher::Create(GetFreePublisherId(), port);

  // Add publisher to port
  port->set_publisher_id(publisher->get_id());
  port->set_has_publisher();

  // Add publisher to publishers
  publishers_.emplace(publisher->get_id(), publisher);

  return publisher;
}

}  // namespace internal
}  // namespace habitify_event_bus

#endif  // HABITIFY_EVENT_BUS_IMPL_EVENT_BUS_IMPL_H_