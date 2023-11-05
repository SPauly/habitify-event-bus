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

  /// Publisher::HasReceivedEvent(size_t index) checks if there are unread
  /// events for the Listener
  virtual bool HasReceivedEvent(size_t index) override {
    std::shared_lock<std::shared_mutex> lock(mux_);
    return index < writer_index_;
  }

  /// Publisher<EvTyp>::Publish(std::unique_ptr< const internal::EventBase>)
  /// takes ownership of the event and provides thread safe access to the
  /// Listener.
  template <typename T>
  bool Publish(std::unique_ptr<const Event<T>> event) {
    if (!get_is_registered()) return false;
    std::unique_lock<std::shared_mutex> lock(mux_);

    auto shared_event =
        std::shared_ptr<const internal::EventBase>(std::move(event));
    event_storage_.emplace(writer_index_, shared_event);

    cv_->notify_all();
    ++writer_index_;
    return true;
  }

  inline const size_t get_writer_index() { return writer_index_; }

 protected:
  /// See PublisherBase::ReadLatestImpl()
  virtual const std::shared_ptr<const internal::EventBase> ReadLatestImpl()
      override {
    std::shared_lock<std::shared_mutex> lock(mux_);

    if (event_storage_.empty()) return nullptr;

    auto event = event_storage_.find(writer_index_ - 1);
    if (event == event_storage_.end()) return nullptr;

    return event->second;
  }

 private:
  Publisher() : PublisherBase() {}
  /// Publisher()::Create() was made private to ensure that it is only created
  /// via the EventBus::CreatePublisher() function. This way we can enforce
  /// that Publisher is purely used as shared_ptr instance.
  static std::shared_ptr<Publisher<EvTyp>> Create() {
    return std::shared_ptr<Publisher<EvTyp>>(new Publisher<EvTyp>());
  }

 private:
  std::unordered_map<int, std::shared_ptr<const internal::EventBase>>
      event_storage_;
  size_t writer_index_ = 0;
};

}  // namespace habitify

#endif  // HABITIFY_EVENT_BUS_INCLUDE_PUBLISHER_H_