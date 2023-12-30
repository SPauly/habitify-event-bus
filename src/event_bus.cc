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
// This file exposes the interface to the EventBus
#include <habitify_event_bus/event_bus.h>

namespace habitify_event_bus {
ListenerPtr EventBus::CreateListener() {
  return std::move(std::make_unique<Listener>(GetFreeListenerId(), impl_));
}

PublisherPtr EventBus::CreatePublisher() {
  return std::move(std::make_unique<Publisher>(GetFreePublisherId(), impl_));
}

const ListenerId EventBus::GetFreeListenerId() {
  std::lock_guard<std::mutex> lock(mux_l_counter_);
  return listener_counter_++;
}

const PublisherId EventBus::GetFreePublisherId() {
  std::lock_guard<std::mutex> lock(mux_p_counter);
  return publisher_counter_++;
}

}  // namespace habitify_event_bus
