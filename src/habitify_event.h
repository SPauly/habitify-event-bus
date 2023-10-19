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

#ifndef HABITIFY_EVENT_BUS_SRC_HABITIFY_EVENT_H_
#define HABITIFY_EVENT_BUS_SRC_HABITIFY_EVENT_H_

namespace habitify {

enum EventType { TEST, TEST2 };

using ChannelIdType = int;

namespace internal {
class EventBase {
 public:
  EventBase() = default;
  EventBase(EventType etype, ChannelIdType channel_id = 0)
      : event_type_(etype), channel_id_(channel_id) {}
  virtual ~EventBase() {}

  inline const EventType &get_event_type() const { return event_type_; }
  inline const ChannelIdType &get_channel_id() const { return channel_id_; }

  inline void set_event_type(const EventType &etype) { event_type_ = etype; }
  inline void set_channel_id(const ChannelIdType &id) { channel_id_ = id; }

  // TODO: Add assert to check for missmatch of type. Do this after fixing the
  // error in PublisherTest
  template <typename T>
  T *MutableGetData() {
    return static_cast<T *>(GetMutableDataImpl());
  }

  template <typename T>
  const T *const GetData() const {
    return static_cast<const T *const>(GetDataImpl());
  }

 protected:
  virtual void *GetMutableDataImpl() { return nullptr; }
  virtual const void *const GetDataImpl() const { return nullptr; }

 private:
  EventType event_type_;
  // TODO: remove channel ID since this is handled by the Publisher and Listener
  // anyways.
  ChannelIdType channel_id_ = 0;
};
}  // namespace internal

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

}  // namespace habitify

#endif  // HABITIFY_EVENT_BUS_SRC_HABITIFY_EVENT_H_
