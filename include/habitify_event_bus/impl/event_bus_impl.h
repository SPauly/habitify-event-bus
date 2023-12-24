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
// Contact via <https://github.com/SPauly/habitify-event-bus>#ifndef HABITIFY_EVENT_BUS_IMPL_EVENT_BUS_IMPL_H_
#define HABITIFY_EVENT_BUS_IMPL_EVENT_BUS_IMPL_H_

#include <cstddef>
#include <memory>
#include <shared_mutex>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include <habitify_event_bus/impl/channel.h>

namespace habitify_event_bus {
namespace internal {
using EventBusImplPtr = std::shared_ptr<EventBusImpl>;

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
  /// Returns the amount of channels currently registered. This might not
  /// directly match the amount of different event types in the future for
  /// optimization but should for now.
  std::size_t get_channel_count() const;
  /// Returns the amount of data currently stored in the channels. Counted in
  /// bytes.
  std::size_t get_data_size() const;

  // Operants
  /// Publish serves as an interface to an internally used EventBroker that
  /// manages the distribution of events. Different EventBrokers may be created
  /// if their individual queues are too long.
  template <typename T>
  const ChannelPtr Publish(::EventConstPtr<T> event);

  /// Does the same type deduction as performed by the EventBroker to determine
  /// the correct channel. Returns nullptr if the channel does not exist.
  template <typename T>
  const ChannelPtr GetChannel() const;

 private:
  // Mutexes for thread safety of the different actors
  // (mutable is needed for locking in const functions)
  mutable std::shared_mutex mux_Channel_;

  // Channels are stored together with the type_index which corresponds the the
  // type of messages shared on this channel.
  std::unordered_map<const std::type_index, ChannelPtr> Channels_;
};

}  // namespace internal
}  // namespace habitify_event_bus

#endif  // HABITIFY_EVENT_BUS_IMPL_EVENT_BUS_IMPL_H_