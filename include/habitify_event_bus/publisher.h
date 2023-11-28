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
#ifndef HABITIFY_EVENT_BUS_INCLUDE_PUBLISHER_H_
#define HABITIFY_EVENT_BUS_INCLUDE_PUBLISHER_H_
#include <habitify_event_bus/impl/publisher_base.h>

#include <cassert>
#include <condition_variable>
#include <memory>
#include <shared_mutex>
#include <vector>

#include <habitify_event_bus/actor_ids.h>
#include <habitify_event_bus/event.h>
#include <habitify_event_bus/impl/port.h>

namespace habitify {
class EventBusImpl;

/// Publisher manages objects of type Event<EvTyp> and publishes the data to the
/// Listener.
/// It is designed to be thread safe so that multiple Listeners can access the
/// data concurrently.
/// TODO: Add Usage:
template <typename EvTyp>
class Publisher : public internal::PublisherBase {
 public:
  virtual ~Publisher() = default;

  // Publisher is not copyable due to the use of std::shared_mutex
  Publisher(const Publisher&) = delete;
  const Publisher& operator=(const Publisher&) = delete;

  // Getters and Setters:
  virtual const bool get_is_registered() const override;
  virtual const PublisherId get_id() const override;
  virtual const PortId get_port_id() const override;

  /// Publisher<EvTyp>::Publish(std::unique_ptr< const internal::EventBase>)
  /// takes ownership of the event and provides thread safe access to the
  /// Listener.
  template <typename T>
  bool Publish(std::unique_ptr<const Event<T>> event) {}

 private:
  friend class EventBusImpl;

  Publisher() = delete;
  Publisher(const PortId port_id, std::shared_ptr<Port> port);

  /// Publisher()::Create() was made private to ensure that it is only created
  /// via the EventBus::CreatePublisher() function. This way we can enforce
  /// that Publisher is purely used as shared_ptr instance.
  static std::shared_ptr<Publisher<EvTyp>> Create(const PortId port_id,
                                                  std::shared_ptr<Port> port) {
    /// TODO: Remove use of new keyword here
    return std::shared_ptr<Publisher<EvTyp>>(
        new Publisher<EvTyp>(port_id, port));
  }

 protected:
  // shared_ptr is used over standard mutex to allow multiple threads
  // to read simultaneously. And only one thread to write.
  mutable std::shared_mutex mux_;
  std::shared_ptr<std::condition_variable_any> cv_;

 private:
  bool is_registered_ = false;
  const PublisherId kPublisherId_;
};

}  // namespace habitify

#endif  // HABITIFY_EVENT_BUS_INCLUDE_PUBLISHER_H_