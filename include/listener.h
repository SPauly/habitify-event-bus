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

#include "include/habitify_event.h"

namespace habitify {

// forward declarations
class EventBus;

/// Listener is used to read events from the Publisher. It is designed to be
/// thread safe. Usage:
/// TODO: Add usage example

class Listener : public std::enable_shared_from_this<Listener> {
 public:
  // EventBus needs access to the CreateSubscriber() function to properly
  // instantiate the Listener object
  friend class EventBus;

  virtual ~Listener() = default;

  // Listener is not copyable due to the use of std::shared_mutex
  Listener(const Listener&) = delete;
  const Listener& operator=(const Listener&) = delete;

  /// Attempts to subscribe to the specified channel. If no Publisher is set it
  /// creates a new Channel. The Channel then calls RefreshPublisher() once the
  /// Publisher was added.
  /// TODO: void ChangeSubscription(const ChannelIdType& channel);

  /// Returns true if the Listener is subscribed to a Publisher. And false if no
  /// publisher is set.
  inline bool ValidatePublisher() { return (bool)publisher_; }

  /// RefreshPublisher() is called by the Channel if a Publisher is added to it.
  inline void RefreshPublisher() {
    std::unique_lock<std::shared_mutex> lock(mux_);
    publisher_ = channel_->get_publisher();
  }

  /// Returns the latest event published by the Publisher. If there are no
  /// events it returns nullptr.
  template <typename EvTyp>
  const std::shared_ptr<const Event<EvTyp>> ReadLatest() {
    std::shared_lock<std::shared_mutex> lock(mux_);

    if (!ValidatePublisher()) return nullptr;

    auto event = publisher_->ReadLatestImpl();
    if (event == nullptr) return nullptr;

    auto latest_converted = std::static_pointer_cast<const Event<EvTyp>>(event);
    if (!latest_converted)
      assert(false && "ReadLatest tried retrieving data of wrong format");

    read_index_++;
    return latest_converted;
  }

  inline bool HasReceivedEvent() {
    return ValidatePublisher() ? publisher_->HasReceivedEvent(read_index_)
                               : false;
  }

  // Getters
  inline const bool get_is_subscribed() { return is_subscribed_; }
  inline const ChannelIdType get_channel_id() { return channel_id_; }
  inline const size_t get_read_index() { return read_index_; }
  inline const std::shared_ptr<EventBus> get_event_bus() { return event_bus_; }

 protected:
  /// Listener() was made private to ensure that it is only created via the
  /// Create function. This way we can enforce that Listener is purely used as
  /// shared_ptr instance.
  /// NOTE: Listener is instantiated via EventBus::CreateSubscriber()
  static std::shared_ptr<Listener> Create(std::shared_ptr<EventBus> event_bus) {
    return std::shared_ptr<Listener>(new Listener(event_bus));
  }

  /// Listener::CreateSubscriber() is used
  /// by the EventBus to assign the Listener to a specific channel
  void Listener::CreateSubscriber(std::shared_ptr<internal::Channel> channel) {
    std::unique_lock<std::shared_mutex> lock(mux_);
    channel_ = channel;
    channel_id_ = channel->get_channel_id();
    publisher_ = channel->get_publisher();
    is_subscribed_ = true;
  }

 private:
  Listener() = delete;
  Listener::Listener(std::shared_ptr<EventBus> event_bus)
      : event_bus_(event_bus) {}

 private:
  mutable std::shared_mutex mux_;
  bool is_subscribed_ = false;
  size_t read_index_ = 0;

  /// channel_id_ refers to a predefined ChannelId and is used to identify the
  /// Publisher.
  ChannelIdType channel_id_ = 0;
  std::shared_ptr<internal::Channel> channel_;

  /// This might be nullptr if the Listener is not subscribed to a Publisher.
  std::shared_ptr<internal::PublisherBase> publisher_;
  std::shared_ptr<EventBus> event_bus_;
};
}  // namespace habitify

#endif  // HABITIFY_EVENT_BUS_INCLUDE_LISTENER_H_