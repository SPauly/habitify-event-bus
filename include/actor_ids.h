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
#ifndef HABITIFY_EVENT_BUS_INCLUDE_ACTOR_IDS_H_
#define HABITIFY_EVENT_BUS_INCLUDE_ACTOR_IDS_H_

namespace habitify {
using PortId = int;
using PublisherId = int;
using ListenerId = int;

namespace internal {
// implementation detail in src/create_ids.cc
const PortId GetPortId();
const PublisherId GetPublisherId();
const ListenerId GetListenerId();
}  // namespace internal

}  // namespace habitify

#endif  // HABITIFY_EVENT_BUS_INCLUDE_ACTOR_IDS_H_