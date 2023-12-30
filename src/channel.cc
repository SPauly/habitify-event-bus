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
#include <habitify_event_bus/impl/channel.h>

namespace habitify_event_bus {
namespace internal {
Channel::Channel(const std::type_index t_index, const size_t type_size)
    : event_type_(t_index),
      type_size_(type_size),
      cv_(std::make_shared<std::condition_variable_any>()) {}

Channel::~Channel() {}

bool Channel::Push(EventConstBasePtr event) {
  // Check if the event is of the correct type
  if (event->get_event_type() != event_type_ || status_ != Status::kOpen) {
    return false;
  }

  // Lock the mutex
  std::unique_lock<std::shared_mutex> lock(mux_events_);

  // Set the position of the event in the queue
  event->set_queue_pos(event_queue_.size() + event_offset_);
  event_queue_.push_back(event->get_id());

  // Store the event
  events_.emplace(std::make_pair<EventId, EventConstBasePtr>(event->get_id(),
                                                             std::move(event)));

  // Update state data
  event_count_++;
  data_size_ += type_size_;

  // Notify all listeners
  cv_->notify_all();

  return true;
}

const EventConstBasePtr Channel::PullLatest() const {
  // Lock the mutex
  std::shared_lock<std::shared_mutex> lock(mux_events_);

  // Check if the queue is empty
  if (event_queue_.empty() || status_ == Status::kClosed) {
    return nullptr;
  }

  return events_.at(event_queue_.back());
}

const EventConstBasePtr Channel::PullNext(const unsigned int pos) const {
  // Lock the mutex
  std::shared_lock<std::shared_mutex> lock(mux_events_);

  // Check if the queue is empty or the position is invalid (with the -1 we also
  // exclude the latest event which can be obtained using PullLatest()
  if (event_queue_.empty() ||
      pos >= (event_queue_.size() + event_offset_) - 1 ||
      status_ == Status::kClosed) {
    return nullptr;
  }

  return events_.at(event_queue_.at(pos - event_offset_));
}

void Channel::FreeEvents(const unsigned int n_keep) {
  // Lock the mutex
  std::unique_lock<std::shared_mutex> lock(mux_events_);

  // Check if the queue is empty or the events can be kept anyways
  if (event_queue_.empty() || n_keep >= event_queue_.size()) return;

  // Check the fast case that n_keep = 0
  if (0 == n_keep) {
    event_offset_ = event_queue_.size() - 1;
    events_.clear();
    event_queue_.clear();
    event_count_ = 0;
    data_size_ = 0;
  }

  // Remove all events except the last n_keep ones
  for (unsigned int i = 0; i < event_queue_.size() - n_keep; i++) {
    events_.erase(event_queue_.at(i));
  }

  // Set the state data
  event_count_ -= event_queue_.size() - n_keep;
  data_size_ -= (event_queue_.size() - n_keep) * type_size_;

  // Set the offset while keeping the last one
  event_offset_ += event_queue_.size() - n_keep - 1;

  // Remove the events from the queue
  event_queue_.erase(event_queue_.begin(), event_queue_.end() - n_keep);
}

void Channel::IncreaseListenerCount() {
  // Lock the mutex
  std::unique_lock<std::shared_mutex> lock(mux_listener_count_);

  // Increase the listener count
  listener_count_++;
}

void Channel::DecreaseListenerCount() {
  // Lock the mutex
  std::unique_lock<std::shared_mutex> lock(mux_listener_count_);

  // Decrease the listener count
  listener_count_--;
}

const Channel::Status Channel::Open() {
  // Lock the mutex
  std::unique_lock<std::shared_mutex> lock(mux_status_);

  if (status_ == Status::kBlocked) return status_;

  return status_ = Status::kOpen;
}

const Channel::Status Channel::Close() {
  // Lock the mutex
  std::unique_lock<std::shared_mutex> lock(mux_status_);

  // Clear the events
  FreeEvents(0);

  // Unsubscribe all listeners
  listener_count_ = 0;

  return status_ = Status::kClosed;
}

const Channel::Status Channel::Block() {
  // Lock the mutex
  std::unique_lock<std::shared_mutex> lock(mux_status_);

  return status_ = Status::kBlocked;
}

const Channel::Status Channel::Unblock() {
  // Lock the mutex
  std::unique_lock<std::shared_mutex> lock(mux_status_);

  return status_ = Status::kOpen;
}

}  // namespace internal
}  // namespace habitify_event_bus
