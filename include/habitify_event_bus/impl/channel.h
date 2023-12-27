// habitify-event-bus - Lightweight event bus system developed for Habitify
//
// Copyright 2023 Simon Pauly
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
// Contact via <https://github.com/SPauly/habitify-event-bus>
#ifndef HABITIFY_EVENT_BUS_IMPL_CHANNEL_H_
#define HABITIFY_EVENT_BUS_IMPL_CHANNEL_H_

#include <condition_variable>
#include <cstddef>
#include <memory>
#include <shared_mutex>
#include <unordered_map>
#include <typeindex>

#include <habitify_event_bus/impl/id_types.h>
#include <habitify_event_bus/impl/event_base.h>

namespace habitify_event_bus {
using ChannelPtr = std::shared_ptr<Channel>;

/// TODO: Move ChannelStatus into Channel class and call it Status for
/// convinience
enum class ChannelStatus { kOpen, kClosed, kBlocked, kWaitingForClosure };

namespace internal {
class Channel {
 public:
  Channel() = delete;
  Channel(const std::type_index t_index, const size_t data_size);
  virtual ~Channel();

  // Operations
  /// Store the given event in the Channel and notify all listeners.
  bool Push(const EventConstBasePtr event);

  /// PullLatest() returns a shared_ptr to the latest Event without
  /// removing it from the Channel. This ensures that thread safety is
  /// maintained in case the event is popped during the process.
  const EventConstBasePtr PullLatest() const;

  /// Removes all events from the Channel for memory efficiency. By default the
  /// latest event is kept but this can be set to any value >= 0.
  void FreeEvents(const unsigned int n_keep = 1);

  /// listener_count is estimated using this function together with
  /// DecreaseListenerCount(). Listener objects should call this function upon
  /// fetching data from this function initially.
  void IncreaseListenerCount();

  /// This function should be called in the destructor of each Listener that has
  /// fetched data from this channel.
  void DecreaseListenerCount();

  // Channel management
  /// Opens the Channel for writing and reading
  const ChannelStatus Open();
  /// Closes the Channel for writing and reading
  const ChannelStatus Close();
  /// Blocks the Channel for writing
  const ChannelStatus Block();
  /// Unblocks the Channel for writing. The PublisherId is used to ensure that
  /// only the Publisher that blocked the Channel can unblock it.
  const ChannelStatus Unblock(const PublisherId id);

  // Getters
  /// Returns the EventType which is stored in this channel
  /// EventBase should always be interpreted as this type.
  inline const std::type_index get_event_type() const { return event_t_; }
  /// Returns the ChannelStatus
  inline const ChannelStatus get_status() const { return status_; }
  /// Returns the size of the data of one element stored in the Channel
  inline const size_t get_data_size() const { return data_size_; }
  /// Returns the amount of events stored in the Channel
  inline const size_t get_event_count() const;

 private:
  mutable std::shared_mutex mux_;
  std::shared_ptr<std::condition_variable_any> cv_;

  const std::type_index event_t_;
  ChannelStatus status_ = ChannelStatus::kClosed;

  size_t data_size_ = 0;
  size_t event_count_ = 0;
  unsigned int listener_count_ = 0;

  // Events are stored as their baseclass to ensure type flexibility.
  // They are stored together with their respectiv ids to enable a fast lookup
  // by listeners.
  std::unordered_map<EventId, EventConstBasePtr> events_;
};

}  // namespace internal
}  // namespace habitify_event_bus

#endif  // HABITIFY_EVENT_BUS_IMPL_CHANNEL_H_