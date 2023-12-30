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
#include <gtest/gtest.h>

#include <habitify_event_bus/impl/event_base.h>
#include <habitify_event_bus/impl/channel.h>

namespace habitify_event_bus {
namespace habitify_testing {
namespace {

class ChannelTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Create a channel
    channel_ = std::make_shared<internal::Channel>(std::type_index(typeid(int)),
                                                   sizeof(int));

    // Create some events and push them to the channel
    for (int i = 0; i < 20; i++) {
      internal::EventBase event(typeid(int));
      events_.push_back(std::make_shared<internal::EventBase>(event));
      channel_->Push(events_.back());
    }
  }

 protected:
  internal::ChannelPtr channel_;

  // Test data
  std::vector<internal::EventConstBasePtr> events_;
};

TEST_F(ChannelTest, Initialization) {
  // Check if the channel was created correctly
  ASSERT_EQ(channel_->get_event_type(), std::type_index(typeid(int)));
  ASSERT_EQ(channel_->get_type_size(), sizeof(int));

  // See if the Events were pushed correctly
  EXPECT_EQ(channel_->get_event_count(), 20);
  EXPECT_EQ(channel_->get_data_size(), 20 * sizeof(int));

  EXPECT_NE(channel_->get_cv(), nullptr);
}

TEST_F(ChannelTest, Push) {
  // Push a new event
  internal::EventBase event(typeid(int));
  auto event_ptr = std::make_shared<internal::EventBase>(event);
  channel_->Push(event_ptr);

  // Check if the event was pushed correctly
  EXPECT_EQ(channel_->get_event_count(), 21);
  EXPECT_EQ(channel_->get_data_size(), 21 * sizeof(int));
}

TEST_F(ChannelTest, PullLatest) {
  // Push a new event
  internal::EventBase event(typeid(int));
  auto event_ptr = std::make_shared<internal::EventBase>(event);
  channel_->Push(event_ptr);

  // Pull the event
  auto pulled_event = channel_->PullLatest();
  EXPECT_EQ(pulled_event, event_ptr);

  // Push another event
  internal::EventBase event2(typeid(int));
  auto event_ptr2 = std::make_shared<internal::EventBase>(event2);
  channel_->Push(event_ptr2);

  // Pull the event and check that it is not equal to the previous one
  pulled_event = channel_->PullLatest();
  EXPECT_NE(pulled_event, event_ptr);
}

TEST_F(ChannelTest, PullNext) {
  // Push a new event
  internal::EventBase event(typeid(int));
  auto event_ptr = std::make_shared<internal::EventBase>(event);
  channel_->Push(event_ptr);

  // Attempt to pull the latest event. Should return nullptr
  auto pulled_event = channel_->PullNext(20);
  EXPECT_EQ(pulled_event, nullptr);

  // Try to pull a valid event
  auto pulled_event2 = channel_->PullNext(19);
  EXPECT_NE(pulled_event2, nullptr);

  // Try to access an invalid position
  auto pulled_event3 = channel_->PullNext(22);
  EXPECT_EQ(pulled_event3, nullptr);
}

}  // namespace
}  // namespace habitify_testing

}  // namespace habitify_event_bus
