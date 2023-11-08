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
#include "include/event_bus_impl.h"

namespace habitify {
namespace internal {
EventBusImpl::EventBusImpl() {
  // instantiate default ports here etc...
  // do some basic setup
}
EventBusImpl::~EventBusImpl() {
  // close all open ports and remove them from the map
  for (std::pair<const PortId, std::shared_ptr<Port>>& p : ports_) {
    p.second->Close();
  }
  ports_.clear();

  // Remove the all Publisher and Listener instances to call their dtors
  publishers_.clear();
  listeners_.clear();
}

}  // namespace internal
}  // namespace habitify
