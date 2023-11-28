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
#ifndef HABITIFY_EVENT_BUS_IMPL_PORT_H_
#define HABITIFY_EVENT_BUS_IMPL_PORT_H_

#include <condition_variable>
#include <cstddef>
#include <memory>
#include <shared_mutex>
#include <unordered_map>

#include <habitify_event_bus/actor_ids.h>
#include <habitify_event_bus/impl/event_base.h>
#include <habitify_event_bus/port_state.h>

namespace habitify {
/// TODO: Move PortStatus into Port class and call it Status for convinience
enum class PortStatus { kOpen, kClosed, kBlocked, kWaitingForClosure };

namespace internal {
class Port {
 public:
  Port() = delete;
  Port(const PortId id);
  virtual ~Port();

  /// TODO: Move most of this functionality into port_state.h
  // Getters
  /// Returns the PortId of the Port
  inline const PortId get_id() const { return id_; }
  /// Returns the PortStatus
  inline const PortStatus get_status() const { return status_; }
  /// Returns the size of the data stored in the Port
  inline const size_t get_data_size() const { return data_size_; }
  /// Returns true if the Port has a publisher
  inline const bool get_has_publisher() const { return has_publisher_; }

  // Setters
  /// Sets has_publisher_ to true
  inline void set_has_publisher() { has_publisher_ = true; }
  /// Sets publisher_id_ to the specified id
  inline void set_publisher_id(const PublisherId id) { publisher_id_ = id; }

  // Port management
  /// Opens the Port for writing and reading
  const PortStatus Open();
  /// Closes the Port for writing and reading
  const PortStatus Close();
  /// Blocks the Port for writing
  const PortStatus Block();
  /// Unblocks the Port for writing. The PublisherId is used to ensure that only
  /// the Publisher that blocked the Port can unblock it.
  const PortStatus Unblock(const PublisherId id);

  // Operants on the Port
  /// stores the event internally as shared_ptr but needs to obtain ownership
  /// first
  bool Push(std::unique_ptr<const EventBase> event);
  /// Enables streaming events to the Port. Calls internal::Port::Push(event)
  /// internally.
  bool operator<<(std::unique_ptr<const EventBase> event);

  // Access the latest Event
  /// TODO: Add functionality to access the latest event without removing or
  /// copying it

  /// PullLatest() returns a copy of the latest Event without
  /// removing it from the port. The copy ensures that thread safety is
  /// maintained in case other threads pop the latest Event.
  const EventBase PullLatest() const;
  /// PopLatest() returns the latest Event and removes it from the port.
  /// It is advised to store the returned Event for later use.
  std::shared_ptr<const EventBase> PopLatest();
  /// Calls PopLatest() internally to return the latest event from the port and
  /// remove it from the port.
  std::shared_ptr<const EventBase> operator>>(
      std::shared_ptr<const EventBase> event_storage);

 private:
  mutable std::shared_mutex mux_;
  std::shared_ptr<std::condition_variable_any> cv_;

  const PortId id_;
  PortStatus status_ = PortStatus::kClosed;

  size_t data_size_ = 0;
  bool has_publisher_ = false;
  PublisherId publisher_id_ = 0;
  unsigned int listener_count_ = 0;

  // Events are stored as their baseclass to ensure type flexibility.
  // They are stored together with their respectiv ids to enable a fast lookup
  // by listeners.
  std::unordered_map<EventId, EventBase> events_;
};

}  // namespace internal
}  // namespace habitify

#endif  // HABITIFY_EVENT_BUS_IMPL_PORT_H_