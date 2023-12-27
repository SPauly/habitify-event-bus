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
  /// Returns the current data load of the event bus. Is calculated when called.
  const BusLoad& get_load();

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
  mutable std::shared_mutex mux_channel_;

  // Channels are stored together with the type_index which corresponds the the
  // type of messages shared on this channel.
  std::unordered_map<const std::type_index, ChannelPtr> channels_;

  // Metadata
  BusLoad load_;
};

}  // namespace internal
}  // namespace habitify_event_bus

#endif  // HABITIFY_EVENT_BUS_IMPL_EVENT_BUS_IMPL_H_