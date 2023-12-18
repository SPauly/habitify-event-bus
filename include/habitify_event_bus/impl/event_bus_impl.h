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
#include <habitify_event_bus/impl/Channel.h>
#include <habitify_event_bus/publisher.h>

namespace habitify_event_bus {
namespace internal {
using ListenerPtr = std::shared_ptr<Listener>;
using PublisherPtr = std::shared_ptr<Publisher>;
using ChannelPtr = std::shared_ptr<Channel>;
using PublisherBasePtr = std::shared_ptr<PublisherBase>;

/// The EventBusImpl is the implementation of the EventBus. It is not exposed to
/// the user. It manages the Listener and Publisher objects by matching them
/// with their respective Channels.
class EventBusImpl : public std::enable_shared_from_this<EventBusImpl> {
 public:
  EventBusImpl();
  virtual ~EventBusImpl();

  // EventBusImpl is noncopyable
  EventBusImpl(const EventBusImpl&) = delete;
  EventBusImpl& operator=(const EventBusImpl&) = delete;

  /// Attempts to create a listener object. The Listener object additionally is
  /// stored in the eventbus for proper life time management with multiple
  /// threads.
  ListenerPtr CreateListener();

  /// Attempts to create a publisher object. The Publisher object additionally
  /// is stored in the eventbus for proper life time management with multiple
  /// threads.
  PublisherPtr CreatePublisher();

 private:
  /// Retrieves a free listener id. This is called when a new listener is
  /// created.
  const ListenerId GetFreeListenerId();
  /// Retrieves a free publisher id. This is called when a new publisher is
  /// created.
  const PublisherId GetFreePublisherId();

 private:
  // Mutexes for thread safety of the different actors
  // (mutable is needed for locking in const functions)
  mutable std::shared_mutex mux_publisher_, mux_listener_, mux_Channel_;
  // Mutex for thread safety of the counters
  mutable std::mutex mux_l_counter_, mux_p_counter;

  // Counters for Ids
  size_t publisher_counter_ = 0, listener_counter_ = 0;

  // Channels are stored together with their ID for fast lookups.
  std::unordered_map<const EventType, ChannelPtr> Channels_;

  // Publishers are stored together with their ID for fast lookups.
  std::unordered_map<const PublisherId, PublisherBasePtr> publishers_;

  // Listeners are stored together with their ID for fast lookups.
  std::unordered_map<const ListenerId, ListenerPtr> listeners_;
};

}  // namespace internal
}  // namespace habitify_event_bus

#endif  // HABITIFY_EVENT_BUS_IMPL_EVENT_BUS_IMPL_H_