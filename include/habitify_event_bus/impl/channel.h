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
#ifndef HABITIFY_EVENT_BUS_IMPL_Channel_H_
#define HABITIFY_EVENT_BUS_IMPL_Channel_H_

#include <condition_variable>
#include <cstddef>
#include <memory>
#include <shared_mutex>
#include <unordered_map>

#include <habitify_event_bus/actor_ids.h>
#include <habitify_event_bus/impl/event_base.h>
#include <habitify_event_bus/Channel_state.h>

namespace habitify_event_bus {
/// TODO: Move ChannelStatus into Channel class and call it Status for convinience
enum class ChannelStatus { kOpen, kClosed, kBlocked, kWaitingForClosure };

namespace internal {
class Channel {
 public:
  Channel() = delete;
  Channel(const ChannelId id);
  virtual ~Channel();

  /// TODO: Move most of this functionality into Channel_state.h
  // Getters
  /// Returns the ChannelId of the Channel
  inline const ChannelId get_id() const { return id_; }
  /// Returns the ChannelStatus
  inline const ChannelStatus get_status() const { return status_; }
  /// Returns the size of the data stored in the Channel
  inline const size_t get_data_size() const { return data_size_; }
  /// Returns true if the Channel has a publisher
  inline const bool get_has_publisher() const { return has_publisher_; }

  // Setters
  /// Sets has_publisher_ to true
  inline void set_has_publisher() { has_publisher_ = true; }
  /// Sets publisher_id_ to the specified id
  inline void set_publisher_id(const PublisherId id) { publisher_id_ = id; }

  // Channel management
  /// Opens the Channel for writing and reading
  const ChannelStatus Open();
  /// Closes the Channel for writing and reading
  const ChannelStatus Close();
  /// Blocks the Channel for writing
  const ChannelStatus Block();
  /// Unblocks the Channel for writing. The PublisherId is used to ensure that only
  /// the Publisher that blocked the Channel can unblock it.
  const ChannelStatus Unblock(const PublisherId id);

  // Operants on the Channel
  /// stores the event internally as shared_ptr but needs to obtain ownership
  /// first
  bool Push(std::unique_ptr<const EventBase> event);
  /// Enables streaming events to the Channel. Calls internal::Channel::Push(event)
  /// internally.
  bool operator<<(std::unique_ptr<const EventBase> event);

  // Access the latest Event
  /// TODO: Add functionality to access the latest event without removing or
  /// copying it

  /// PullLatest() returns a copy of the latest Event without
  /// removing it from the Channel. The copy ensures that thread safety is
  /// maintained in case other threads pop the latest Event.
  const EventBase PullLatest() const;
  /// PopLatest() returns the latest Event and removes it from the Channel.
  /// It is advised to store the returned Event for later use.
  std::shared_ptr<const EventBase> PopLatest();
  /// Calls PopLatest() internally to return the latest event from the Channel and
  /// remove it from the Channel.
  std::shared_ptr<const EventBase> operator>>(
      std::shared_ptr<const EventBase> event_storage);

 private:
  mutable std::shared_mutex mux_;
  std::shared_ptr<std::condition_variable_any> cv_;

  const ChannelId id_;
  ChannelStatus status_ = ChannelStatus::kClosed;

  size_t data_size_ = 0;
  bool has_publisher_ = false;
  PublisherId publisher_id_ = 0;
  unsigned int listener_count_ = 0;

  // Events are stored as their baseclass to ensure type flexibility.
  // They are stored together with their respectiv ids to enable a fast lookup
  // by listeners.
  std::unordered_map<EventId, EventBase> events_;
};

}  // namespace internal
}  // namespace habitify

#endif  // HABITIFY_EVENT_BUS_IMPL_Channel_H_