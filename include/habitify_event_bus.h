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

// This file exposes the interface to the EventBus
#ifndef HABITIFY_EVENT_BUS_INCLUDE_HABITIFY_EVENT_BUS_H_
#define HABITIFY_EVENT_BUS_INCLUDE_HABITIFY_EVENT_BUS_H_

#include <memory>
#include <shared_mutex>

#include "include/event_bus_impl.h"
#include "include/habitify_event.h"
#include "include/listener.h"
#include "include/port.h"
#include "include/publisher.h"

namespace habitify {
class EventBus : public std::enable_shared_from_this<EventBus> {
 public:
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
    return std::shared_ptr<EventBus>(new EventBus());
  }

  /// Returns a shared_ptr to the Listener object that is subscribed to the
  /// specified channel. This is the only way to obtain a Listener object.
  std::shared_ptr<Listener> EventBus::CreateListener(
      const ChannelIdType& channel_id) {
    auto channel = GetChannel(channel_id);

    std::unique_lock<std::shared_mutex> lock(mux_);

    if (channel) {
      auto listener = Listener::Create(shared_from_this());
      listener->CreateSubscriber(channel);
      channel->RegisterListener(listener);
      return listener;
    }

    return nullptr;
  }

  /// Returns a shared_ptr to the Publisher object that publishes to the
  /// specified channel
  template <typename EvTyp>
  std::shared_ptr<Publisher<EvTyp>> CreatePublisher(
      const ChannelIdType& channel) {
    auto channel_ptr = GetChannel(channel);

    std::unique_lock<std::shared_mutex> lock(mux_);
    // If the channel already has a publisher we avoid creating a new one. And
    // instead share the access to it.
    if (channel_ptr->get_publisher() != nullptr)
      return std::static_pointer_cast<Publisher<EvTyp>>(
          channel_ptr->get_publisher());

    auto publisher = Publisher<EvTyp>::Create();
    publisher->CreatePublisher(channel_ptr);
    channel_ptr->CreatePublisher(publisher);

    return publisher;
  }

  // Getters
  inline const int GetChannelCount() { return channels_.size(); }

 protected:
  /// Returns the Channel with the specified ID. If no Channel with that ID
  /// exists it instantiates a new one.
  std::shared_ptr<internal::Channel> EventBus::GetChannel(
      const ChannelIdType& channel) {
    std::unique_lock<std::shared_mutex> lock(mux_);

    auto it = channels_.find(channel);
    if (it != channels_.end()) return it->second;

    // If the channel does not exist yet we create it.
    auto channel_ptr = std::make_shared<internal::Channel>(channel);
    channels_.emplace(std::make_pair(channel, channel_ptr));

    return channel_ptr;
  }

 private:
  // This is a singleton class so the constructor needs to be private.
  EventBus() = default;

 private:
  mutable std::shared_mutex mux_;

  // Channels are stored together with their ID for fast lookups.
  std::unordered_map<ChannelIdType, std::shared_ptr<internal::Channel>>
      channels_;
};

}  // namespace habitify

#endif  // HABITIFY_EVENT_BUS_INCLUDE_HABITIFY_EVENT_BUS_H_
