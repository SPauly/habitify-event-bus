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
// Contact via <https://github.com/SPauly/habitify-event-bus>#ifndef
// HABITIFY_EVENT_BUS_IMPL_EVENT_BUS_IMPL_H_
#ifndef HABITIFY_EVENT_BUS_IMPL_EVENT_BUS_IMPL_H_
#define HABITIFY_EVENT_BUS_IMPL_EVENT_BUS_IMPL_H_

#include <cstddef>
#include <memory>
#include <shared_mutex>
#include <typeindex>
#include <unordered_map>

#include <habitify_event_bus/impl/channel.h>

namespace habitify_event_bus {
namespace internal {
using EventBusImplPtr = std::shared_ptr<EventBusImpl>;

/// BusLoad is a struct that contains information about the current load of the
/// event bus.
struct BusLoad {
  /// The amount of channels currently registered.
  size_t channel_count = 0;
  /// The amount of events currently stored in the channels.
  size_t event_count = 0;
  /// The amount of data currently stored in the channels. Counted in bytes.
  size_t data_size = 0;
};

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

  // Getters
  /// Returns the current data load of the event bus. Tge load is calculated
  /// when called.
  const BusLoad& get_load();

  /// Returns the number of registered Channels
  const size_t get_channel_count() const;

  // Operants
  /// Publish uses typedetuction to determine the correct Channel and creates it
  /// if needed. The event is then published to the channel and the
  /// corresponding Channel is returned. Returns nullptr if the event fails to
  /// be published.
  template <typename T>
  const ChannelPtr Publish(::EventConstPtr<T> event);

  /// GetChannel returns the Channel that is deduced from the given type T and
  /// creates it if it does not yet exist. It does not return nullptr. Even if a
  /// certain Channel is blocked or nonexistent.
  template <typename T>
  const ChannelPtr GetChannel();

  /// RemoveChannel removes the Channel that is deduced from the given type T
  /// from the event bus. The events stored in the Channel are lost to listeners
  /// that have not obtained access to them yet. Handle with care!
  template <typename T>
  void RemoveChannel();

  /// Removes all events from the Channels for memory efficiency. By default the
  /// latest event is kept but this can be set to any value >= 0.
  void FreeEvents(const unsigned int n_keep = 1);

  /// Reduces the overall amount of stored data dynamicaly meaning that the
  /// provided maximum amount of bytes will be split across all channels to be
  /// matched.
  void DynamicFreeSpace(const unsigned int n_max_bytes = 1);

 private:
  // Mutexes for thread safety of the different actors
  // (mutable is needed for locking in const functions)
  mutable std::shared_mutex mux_channels_;

  // Channels are stored together with the type_index which corresponds the the
  // type of messages shared on this channel.
  std::unordered_map<const std::type_index, ChannelPtr> channels_;

  // Metadata
  BusLoad load_;
};

// Type deduction
template <typename T>
inline constexpr std::type_index DeduceEventType = typeid(T);

// Template definitions

template <typename T>
const ChannelPtr EventBusImpl::Publish(::EventConstPtr<T> event) {
  // Prepare the event for publishing
  event->set_event_type(DeduceEventType<T>);

  const ChannelPtr channel = GetChannel<T>();

  if (channel->Push(event) == true)
    return channel;
  else
    return nullptr;
}

template <typename T>
const ChannelPtr EventBusImpl::GetChannel() {
  std::type_index channel_t = DeduceEventType<T>;

  std::shared_lock<std::shared_mutex> slock(mux_channels_);

  // Check if the Channel already exists
  auto it = channels_.find(channel_t);

  if (it != channels_.end()) {
    return channels_.at(channel_t);
  } else {
    // Aquire the exclusive lock over channels_ since we have to add the
    // Channel. Open the shared_lock first
    slock.unlock();
    std::unique_lock<std::shared_mutex> ulock(mux_channels_);

    ChannelPtr new_channel = std::make_shared<Channel>(channel_t, sizeof(T));
    channels_.emplace(
        std::pair<std::type_index, ChannelPtr>(channel_t, new_channel));
    return new_channel;
  }
}

template <typename T>
void EventBusImpl::RemoveChannel() {
  std::type_index channel_t = DeduceEventType<T>;

  std::unique_lock<std::shared_mutex> lock(mux_channels_);

  // Only Remove the channel if it already exists
  auto it = channels_.find(channel_t);

  if (it != channels_.end()) {
    channels_.at(channel_t)->Close();
    channels_.erase(it);
  }

}  // namespace internal
}  // namespace habitify_event_bus

#endif  // HABITIFY_EVENT_BUS_IMPL_EVENT_BUS_IMPL_H_