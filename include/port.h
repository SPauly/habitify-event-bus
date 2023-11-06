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
#ifndef HABITIFY_EVENT_BUS_INCLUDE_PORT_H_
#define HABITIFY_EVENT_BUS_INCLUDE_PORT_H_

#include <condition_variable>
#include <cstddef>
#include <memory>
#include <shared_mutex>

#include "include/habitify_event.h"

namespace habitify {
namespace internal {

using PortId = int;

enum class PortStatus { kOpen, kClosed, kBlocked, kWaitingForClosure };
class Port {
 public:
  Port();
  virtual ~Port();

  // Port management
  /// Opens the Port for writing and reading
  const PortStatus Open();
  /// Closes the Port for writing and reading
  const PortStatus Close();
  /// Blocks the Port for writing
  const PortStatus Block();

  /// Returns the PortId of the Port
  inline const PortId GetId() const { return id_; }

  /// stores the event internally as shared_ptr but needs to obtain ownership
  /// first
  bool Push(std::unique_ptr<const EventBase> event);

  // Access the latest Event
  /// PullLatest() returns a copy of the latest Event without removing it from
  /// the port. The copy ensures that thread safety is maintained in case other
  /// threads pop the latest Event.
  const EventBase PullLatest() const;
  /// PopLatest() returns the latest Event and removes it from the port.
  /// It is advised to store the returned Event for later use.
  std::shared_ptr<const EventBase> PopLatest();

 private:
  mutable std::shared_mutex mux_;
  std::shared_ptr<std::condition_variable_any> cv_;

  const PortId id_;
  PortStatus status_ = PortStatus::kClosed;

  size_t data_size_ = 0;
  unsigned int listener_count_ = 0;
};

}  // namespace internal

}  // namespace habitify

#endif  // HABITIFY_EVENT_BUS_INCLUDE_PORT_H_