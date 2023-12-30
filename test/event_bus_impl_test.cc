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

#ifndef HABITIFY_EVENT_BUS_TESTING
#define HABITIFY_EVENT_BUS_TESTING
#endif  // HABITIFY_EVENT_BUS_TESTING

#include <memory>
#include <string>

#include <habitify_event_bus/event.h>

namespace habitify_event_bus {
namespace habitify_testing {
namespace {

class EventBusImplTest : public ::testing::Test {
 protected:
  void SetUp() override { event_bus_impl_ = std::make_shared<EventBusImpl>(); }

 protected:
  EventBusImplPtr event_bus_impl_;

  // Events
  TestEvents::OK ok_event_;
  TestEvents::ERROR error_event_;
  TestEvents::TEST test_event_ = {42, "Test String"};
  TestEvents::TEST test_event_2 = {23, "Test String 2"};
};

TEST_F(EventBusImplTest, Initialization) {
  ASSERT_TRUE(event_bus_impl_ != nullptr);

  // Check the empty state of a newly created EventBusImpl
  // This might change over time, but for now we expect the following:
  EXPECT_EQ(event_bus_impl_->get_channel_count(), 0);

  internal::BusLoad load = event_bus_impl_->get_load();
  EXPECT_EQ(load.event_count, 0);
  EXPECT_EQ(load.data_size, 0);

  // Test that all the initial setup is done correctly
  // There is non for now
}

TEST_F(EventBusImplTest, ChannelCreation) {
  // Create a new channel
  auto channel = event_bus_impl_->Publish(
      std::make_shared<const Event<TestEvents::TEST>>(test_event_));
  ASSERT_TRUE(channel != nullptr);

  // Check that the channel is correctly added to the EventBusImpl
  EXPECT_EQ(event_bus_impl_->get_channel_count(), 1);
  internal::BusLoad load = event_bus_impl_->get_load();
  EXPECT_EQ(load.event_count, 1);
  EXPECT_EQ(load.data_size, sizeof(test_event_));

  auto channel2 = event_bus_impl_->Publish(
      std::make_shared<const Event<TestEvents::OK>>(ok_event_));
  ASSERT_TRUE(channel2 != nullptr);

  // The channel count should be 2 now
  EXPECT_EQ(event_bus_impl_->get_channel_count(), 2);

  auto channel3 = event_bus_impl_->Publish(
      std::make_shared<const Event<TestEvents::ERROR>>(error_event_));
  ASSERT_TRUE(channel3 != nullptr);

  // The channel count should be 3 now and all channels should be different
  EXPECT_EQ(event_bus_impl_->get_channel_count(), 3);
  EXPECT_NE(channel, channel2);
  EXPECT_NE(channel2, channel3);
}

TEST_F(EventBusImplTest, EventBrokerTypeDeduction) {
  // Test if the event type is correctly deduced
  auto channel = event_bus_impl_->Publish(
      std::make_shared<const Event<TestEvents::TEST>>(test_event_));
  ASSERT_TRUE(channel != nullptr);
  EXPECT_EQ(channel->get_event_type(), typeid(TestEvents::TEST));

  // Test deduction for two similar events
  auto channel2 = event_bus_impl_->Publish(
      std::make_shared<const Event<TestEvents::OK>>(ok_event_));
  auto channel3 = event_bus_impl_->Publish(
      std::make_shared<const Event<TestEvents::ERROR>>(error_event_));

  ASSERT_FALSE(channel2->get_event_type() == channel3->get_event_type());
  EXPECT_EQ(channel2->get_event_type(), typeid(TestEvents::OK));
  EXPECT_EQ(channel3->get_event_type(), typeid(TestEvents::ERROR));

  // Test deduction for two equal events
  channel4 = event_bus_impl_->Publish(
      std::make_shared<const Event<TestEvents::TEST>>(test_event_2));

  EXPECT_EQ(channel->get_event_type(), channel4->get_event_type());
  EXPECT_EQ(channel, channel4);
}

TEST_F(EventBusImplTest, ChannelRetrieval) {
  // Publish an event
  auto channel = event_bus_impl_->Publish(
      std::make_shared<const Event<TestEvents::TEST>>(test_event_));
  ASSERT_TRUE(channel != nullptr);
  auto channel2 = event_bus_impl_->Publish(
      std::make_shared<const Event<TestEvents::TEST>>(test_event_2));
  ASSERT_TRUE(channel2 != nullptr);

  // Retrieve the right channel
  auto channel_receive = event_bus_impl_->GetChannel<TestEvents::TEST>();
  EXPECT_EQ(channel, channel_receive);

  // Check that it matches with the second channel
  EXPECT_EQ(channel, channel2);

  // Check for a non existing channel
  auto channel_receive2 = event_bus_impl_->GetChannel<TestEvents::OK>();
  EXPECT_NE(channel_receive2, nullptr);
}

TEST_F(EventBusImplTest, ChannelRemoval) {
  // Publish an event
  auto channel = event_bus_impl_->Publish(
      std::make_shared<const Event<TestEvents::TEST>>(test_event_));
  ASSERT_TRUE(channel != nullptr);

  // Check that the channel is correctly added to the EventBusImpl
  EXPECT_EQ(event_bus_impl_->get_channel_count(), 1);
  EXPECT_NE(event_bus_impl_->get_data_size(), 0);

  // Remove the channel
  event_bus_impl_->RemoveChannel<TestEvents::TEST>();

  // Check that the channel is correctly removed from the EventBusImpl
  EXPECT_EQ(event_bus_impl_->get_channel_count(), 0);
  BusLoad load = event_bus_impl_->get_load();
  EXPECT_EQ(load.event_count, 0);
  EXPECT_EQ(load.data_size, 0);
}
}  // namespace
}  // namespace habitify_testing
}  // namespace habitify_event_bus
