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
#ifndef HABITIFY_EVENT_BUS_IMPL_PUBLISHER_BASE_H_
#define HABITIFY_EVENT_BUS_IMPL_PUBLISHER_BASE_H_

#include <cassert>
#include <condition_variable>
#include <memory>
#include <shared_mutex>
#include <vector>

#include <habitify_event_bus/actor_ids.h>
#include <habitify_event_bus/impl/port.h>

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
  // shared_ptr is used over standard mutex to allow multiple threads
  // to read simultaneously. And only one thread to write.
  mutable std::shared_mutex mux_;

  std::shared_ptr<std::condition_variable_any> cv_;

 private:
  bool is_registered_ = false;

  const PortId kPortId_;
  const PublisherId kPublisherId_;
};
}  // namespace internal
}  // namespace habitify

#endif  // HABITIFY_EVENT_BUS_IMPL_PUBLISHER_BASE_H_