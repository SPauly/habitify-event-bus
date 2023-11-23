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
#ifndef HABITIFY_EVENT_BUS_INCLUDE_PUBLISHER_H_
#define HABITIFY_EVENT_BUS_INCLUDE_PUBLISHER_H_

#include <cassert>
#include <condition_variable>
#include <memory>
#include <shared_mutex>
#include <vector>

#include "include/habitify_event.h"

namespace habitify {

// forward declaration implemented in include/event_bus_impl.h
class EventBusImpl;

namespace internal {
/// PublisherBase is used as a way of storing Publisher in the EventBus. The
/// actual functionality is implemented by the derived class. It needs to be
/// inherated from. Use Publisher as the interface to EventBus!
class PublisherBase : public std::enable_shared_from_this<PublisherBase> {
 public:
  PublisherBase::PublisherBase(const PortId port_id, std::shared_ptr<Port> port)
      : cv_(std::make_shared<std::condition_variable_any>()),
        kPortId_(port_id),
        kPublisherId_(internal::GetPublisherId()) {}
  virtual ~PublisherBase() = default;

  // PublisherBase is not copyable due to the use of std::shared_mutex
  PublisherBase(const PublisherBase&) = delete;
  const PublisherBase& operator=(const PublisherBase&) = delete;

  // Getters and Setters:
  inline const bool get_is_registered() const { return is_registered_; }
  inline const PublisherId get_id() const { return kPublisherId_; }

 protected:
  mutable std::shared_mutex
      mux_;  // shared_ptr is used over standard mutex to allow multiple threads
             // to read simultaneously. And only one thread to write.

  std::shared_ptr<std::condition_variable_any> cv_;

 private:
  bool is_registered_ = false;

  const PortId kPortId_;
  const PublisherId kPublisherId_;
};
}  // namespace internal

/// Publisher manages objects of type Event<EvTyp> and publishes the data to the
/// Listener.
/// It is designed to be thread safe so that multiple Listeners can access the
/// data concurrently.
/// TODO: Add Usage:
template <typename EvTyp>
class Publisher : public internal::PublisherBase {
 public:
  friend class EventBus;

  ~Publisher() = default;

  // Publisher is not copyable due to the use of std::shared_mutex
  Publisher(const Publisher&) = delete;
  const Publisher& operator=(const Publisher&) = delete;

  /// Publisher<EvTyp>::Publish(std::unique_ptr< const internal::EventBase>)
  /// takes ownership of the event and provides thread safe access to the
  /// Listener.
  template <typename T>
  bool Publish(std::unique_ptr<const Event<T>> event) {}

 private:
  Publisher() = delete;
  Publisher(const PortId port_id, std::shared_ptr<Port> port)
      : PublisherBase(port_id, port) {}
  /// Publisher()::Create() was made private to ensure that it is only created
  /// via the EventBus::CreatePublisher() function. This way we can enforce
  /// that Publisher is purely used as shared_ptr instance.
  static std::shared_ptr<Publisher<EvTyp>> Create(const PortId port_id,
                                                  std::shared_ptr<Port> port) {
    /// TODO: Remove use of new keyword here
    return std::shared_ptr<Publisher<EvTyp>>(
        new Publisher<EvTyp>(port_id, port));
  }
};

}  // namespace habitify

#endif  // HABITIFY_EVENT_BUS_INCLUDE_PUBLISHER_H_