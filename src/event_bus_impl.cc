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
#include <habitify_event_bus/impl/event_bus_impl.h>

namespace habitify_event_bus {
namespace internal {
EventBusImpl::EventBusImpl() {}
EventBusImpl::~EventBusImpl() {
  std::unique_lock<std::shared_mutex> lock(mux_channels_);
  // Close all the channels before removing them to inform all eventual
  // listeners that the bus is shutting down
  for (auto& channel : channels_) {
    channel.second->Close();
  }

  channels_.clear();
}

const BusLoad& EventBusImpl::get_load() {
  // since a lot of threads may write to the channel at once and each trying to
  // aquiring the lock for load_ would be time intensive the load is calculated
  // when requested.
  std::shared_lock<std::shared_mutex> lock(mux_channels_);
  load_.channel_count = channels_.size();

  for (const auto& channel : channels_) {
    load_.event_count += channel.second->get_event_count();
    load_.data_size += channel.second->get_data_size();
  }

  return load_;
}

const size_t EventBusImpl::get_channels() const {
  std::shared_lock<std::shared_mutex> lock(mux_channels_);
  return channels_.size();
}

void EventBusImpl::FreeEvents(const unsigned int n_keep) {
  std::shared_lock<std::shared_mutex> lock(mux_channels_);
  for (auto& channel : channels_) {
    channel.second->FreeEvents(n_keep);
  }
}

void EventBusImpl::DynamicFreeSpace(const unsigned int n_max_bytes) {
  std::shared_lock<std::shared_mutex> lock(mux_channels_);
  // Determine the maximum amount of bytes per channel
  const unsigned int n_max_bytes_per_channel = n_max_bytes / channels_.size();

  for (auto& channel : channels_) {
    // Calculate how many events can be kept per event type
    unsigned int keep =
        n_max_bytes_per_channel / channel.second->get_data_size();
    channel.second->FreeEvents(keep);
  }
}

}  // namespace internal
}  // namespace habitify_event_bus
