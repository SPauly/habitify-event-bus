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
#include <habitify_event_bus/impl/event_bus_impl.h>

namespace habitify_event_bus {
namespace internal {
EventBusImpl::EventBusImpl() {
  // instantiate default Channels here etc...
  // do some basic setup
}
EventBusImpl::~EventBusImpl() {
  std::unique_lock<std::shared_mutex> lock(mux_publisher_);

  // close all open Channels and remove them from the map
  for (std::pair<const EventType, ChannelPtr>& p : Channels_) {
    p.second->Close();
  }
  Channels_.clear();

  // Remove the all Publisher and Listener instances to call their dtors
  publishers_.clear();
  listeners_.clear();
}

std::shared_ptr<Listener> EventBusImpl::CreateListener() {
  std::unique_lock<std::shared_mutex> lock(mux_listener_);

  // create the listener object
  std::shared_ptr<Listener> listener =
      std::make_shared<Listener>(GetFreeListenerId(), shared_from_this());

  // store the listener object in the map
  listeners_.emplace(listener->get_id(), listener);

  return listener;
}

PublisherPtr EventBusImpl::CreatePublisher() {
  std::unique_lock<std::shared_mutex> lock(mux_publisher_);

  ChannelPtr Channel;

  // Create publisher
  PublisherPtr publisher = Publisher::Create(GetFreePublisherId());

  // Add publisher to publishers
  publishers_.emplace(publisher->get_id(), publisher);

  return publisher;
}

const ListenerId EventBusImpl::GetFreeListenerId() {
  std::lock<std::mutex> lock(mux_l_counter_);

  return ++listener_count_;
}

const PublisherId EventBusImpl::GetFreePublisherId() {
  std::lock<std::mutex> lock(mux_p_counter);

  return ++publisher_counter_;
}

}  // namespace internal
}  // namespace habitify_event_bus
