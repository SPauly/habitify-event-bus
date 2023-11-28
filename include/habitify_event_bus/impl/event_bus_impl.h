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

#include <memory>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

#include <habitify_event_bus/listener.h>
#include <habitify_event_bus/impl/port.h>
#include <habitify_event_bus/publisher.h>

namespace habitify {
namespace internal {
class EventBusImpl : public std::enable_shared_from_this<EventBusImpl> {
 public:
  EventBusImpl();
  ~EventBusImpl();

  // EventBusImpl is noncopyable
  EventBusImpl(const EventBusImpl&) = delete;
  EventBusImpl& operator=(const EventBusImpl&) = delete;

  /// Attempts to create a listener object that is subscribed to the specified
  /// port. Returns nullptr if the port is blocked. The Listener object
  /// additionally is stored in the eventbus for proper life time management
  /// with multiple threads.
  std::shared_ptr<Listener> CreateListener(const PortId id);

  /// Attempts to create a publisher object that is subscribed to the specified
  /// port. Returns nullptr if the port is blocked or already has a publisher.
  /// The Publisher object additionally is stored in the eventbus for proper
  /// life time management with multiple threads.
  template <typename EvTyp>
  std::shared_ptr<Publisher<EvTyp>> CreatePublisher(const PortId id);

 private:
  // Ports are stored together with their ID for fast lookups.
  std::unordered_map<const PortId, std::shared_ptr<Port>> ports_;

  // Publishers are stored together with their ID for fast lookups.
  std::unordered_map<const PublisherId, std::shared_ptr<PublisherBase>>
      publishers_;

  // Listeners are stored together with their ID for fast lookups.
  std::unordered_map<const ListenerId, std::shared_ptr<Listener>> listeners_;
};

// Template definition of CreatePublisher
template <typename EvTyp>
std::shared_ptr<Publisher<EvTyp>> EventBusImpl::CreatePublisher(
    const PortId id) {
  std::shared_ptr<Port> port;

  // Check if port already exists
  if (ports_.find(id) != ports_.end()) {
    // return nullptr if the port already has a publisher publishing to it
    if (ports_.at(id)->get_has_publisher()) return nullptr;

    // set the port properly otherwise
    port = ports_.at(id);
  } else {
    port = std::make_shared<internal::Port>(id);
  }

  // Create publisher
  auto publisher = std::make_shared<Publisher<EvTyp>>(id, port);

  // Add publisher to port
  port->set_publisher_id(publisher->get_id());
  port->set_has_publisher();

  // Add publisher to publishers
  publishers_.emplace(publisher->get_id(), publisher);

  return publisher;
}

}  // namespace internal
}  // namespace habitify

#endif  // HABITIFY_EVENT_BUS_IMPL_EVENT_BUS_IMPL_H_