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

#ifndef HABITIFY_EVENT_BUS_SRC_HABITIFY_EVENT_H_
#define HABITIFY_EVENT_BUS_SRC_HABITIFY_EVENT_H_

#include <habitify_event_bus/impl/event_base.h>

namespace habitify_event_bus {

/// TODO: Add documentation + Move getters and setters from base class here for
/// a better interface
template <typename T>
class Event : public internal::EventBase {
 public:
  Event(EventType etype, ChannelIdType channel_id, T *data)
      : internal::EventBase(etype, channel_id), data_(data) {}
  ~Event() {}

 protected:
  virtual void *GetMutableDataImpl() override { return data_; }
  virtual const void *const GetDataImpl() const override { return data_; }

 private:
  T *data_;
};

}  // namespace habitify_event_bus

#endif  // HABITIFY_EVENT_BUS_SRC_HABITIFY_EVENT_H_
