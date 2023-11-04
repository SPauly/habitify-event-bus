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

// forward declaration implemented in include/event_bus.h
class EventBus;

namespace internal {
/// PublisherBase is used as a way of storing Publisher in the EventBus. The
/// actual functionality is implemented by the derived class. It needs to be
/// inherated from. Use Publisher as the interface to EventBus!
class PublisherBase : public std::enable_shared_from_this<PublisherBase> {
 public:
  PublisherBase::PublisherBase()
      : cv_(std::make_shared<std::condition_variable_any>()) {}
  virtual ~PublisherBase() = default;

  // PublisherBase is not copyable due to the use of std::shared_mutex
  PublisherBase(const PublisherBase&) = delete;
  const PublisherBase& operator=(const PublisherBase&) = delete;

  // Getters and Setters:
  inline const ChannelIdType& get_channel_id() { return channel_id_; }
  inline const bool get_is_registered() { return is_registered_; }
  /// Returns a conditonal_variable_any that is notified by Publish().
  inline std::shared_ptr<std::condition_variable_any> get_cv() { return cv_; }
  /// Returns the latest Event as it's base class. This is mostly used for
  /// testing. Prefer accessing the Data via a Listener object.
  const std::shared_ptr<const internal::EventBase> GetLatestEvent() {
    return ReadLatestImpl();
  }

  /// PublisherBase::HasReceivedEvent(size_t index) checks if there are unread
  /// events for the caller based on the specified index. TODO: take into
  /// account that events can be removed from the que.
  virtual bool HasReceivedEvent(size_t index) {
    assert(false && "HasReceivedEvent() not implemented");
    return true;
  }

 protected:
  /// This function is called by Listener::ReadLatest and is implemented by
  /// the derived class.
  virtual const std::shared_ptr<const internal::EventBase> ReadLatestImpl() {
    return nullptr;
  }

 protected:
  mutable std::shared_mutex
      mux_;  // shared_ptr is used over standard mutex to allow multiple threads
             // to read simultaneously. And only one thread to write.

  std::shared_ptr<std::condition_variable_any> cv_;

 private:
  bool is_registered_ = false;
  /// channel_id_ refers to a predefined ChannelId and is used for
  /// identification by the Listener.
  ChannelIdType channel_id_ = 0;
};
}  // namespace internal
}  // namespace habitify

#endif  // HABITIFY_EVENT_BUS_INCLUDE_PUBLISHER_H_