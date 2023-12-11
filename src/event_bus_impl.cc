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
#include <habitify_event_bus/impl/event_bus_impl.h>

namespace habitify {
namespace internal {
EventBusImpl::EventBusImpl() {
  // instantiate default ports here etc...
  // do some basic setup
}
EventBusImpl::~EventBusImpl() {
  std::unique_lock<std::shared_mutex> lock(mux_publisher_);

  // close all open ports and remove them from the map
  for (std::pair<const PortId, PortPtr>& p : ports_) {
    p.second->Close();
  }
  ports_.clear();

  // Remove the all Publisher and Listener instances to call their dtors
  publishers_.clear();
  listeners_.clear();
}

std::shared_ptr<Listener> EventBusImpl::CreateListener(const PortId id) {
  std::unique_lock<std::shared_mutex> lock(mux_listener_);

  // This is used to retreive/or create the respective port object
  std::shared_ptr<Port> port;

  // create the port in case it does not exist
  if (ports_.find(id) == ports_.end()) {
    port = std::make_shared<Port>(id);
    ports_.emplace(id, port);
  } else {
    port = ports_.at(id);
  }

  // if the port is blocked we want to create the listener but set an error flag
  // TODO: Set error flag
  if (port->get_status() == PortStatus::kBlocked)
    // Set the error flag for later
    ;

  // create the listener object
  std::shared_ptr<Listener> listener =
      std::make_shared<Listener>(GetFreeListenerId(), shared_from_this(), port);

  // store the listener object in the map
  listeners_.emplace(listener->get_id(), listener);

  return listener;
}

const PortId EventBusImpl::GetFreePortId() {
  std::lock_guard<std::mutex> lock(mux_pid_counter_);
  /// TODO: check if this needs a more advanced implementation to handle edge
  /// cases
  /// TODO: Reserve certain port ids for special purposes
  return portid_counter_++;
}

}  // namespace internal
}  // namespace habitify
