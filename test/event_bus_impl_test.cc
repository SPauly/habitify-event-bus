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
#include <gtest/gtest.h>

#ifndef HABITIFY_EVENT_BUS_TESTING
#define HABITIFY_EVENT_BUS_TESTING
#endif

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
  ::TestEvents::OK ok_event_;
  ::TestEvents::ERROR error_event_;
  ::TestEvents::TEST test_event_ = {42, "Test String"};
  ::TestEvents::TEST test_event_2 = {23, "Test String 2"};
};

TEST_F(EventBusImplTest, Initialization) {
  ASSERT_TRUE(event_bus_impl_ != nullptr);

  // Check the empty state of a newly created EventBusImpl
  // This might change over time, but for now we expect the following:
  EXPECT_EQ(event_bus_impl_->get_channel_count(), 0);
  EXPECT_EQ(event_bus_impl_->get_data_size(), 0);

  // Test that all the initial setup is done correctly
  // There is non for now
}

TEST_F(EventBusImplTest, ChannelCreation) {
  // Create a new channel
  auto channel = event_bus_impl_->Publish(
      std::make_shared<::Event<const TestEvents::TEST>>(test_event_));
  ASSERT_TRUE(channel != nullptr);

  // Check that the channel is correctly added to the EventBusImpl
  EXPECT_EQ(event_bus_impl_->get_channel_count(), 1);
  EXPECT_NE(event_bus_impl_->get_data_size(), 0);

  auto channel2 = event_bus_impl_->Publish(
      std::make_shared<::Event<const TestEvents::OK>>(ok_event_));
  ASSERT_TRUE(channel2 != nullptr);

  // The channel count should be 2 now
  EXPECT_EQ(event_bus_impl_->get_channel_count(), 2);

  auto channel3 = event_bus_impl_->Publish(
      std::make_shared<::Event<const TestEvents::ERROR>>(error_event_));
  ASSERT_TRUE(channel3 != nullptr);

  // The channel count should be 3 now and all channels should be different
  EXPECT_EQ(event_bus_impl_->get_channel_count(), 3);
  EXPECT_NE(channel, channel2);
  EXPECT_NE(channel2, channel3);
}

TEST_F(EventBusImplTest, EventBrokerTypeDeduction) {
  // Test if the event type is correctly deduced
  auto channel = event_bus_impl_->Publish(
      std::make_shared<::Event<const TestEvents::TEST>>(test_event_));
  ASSERT_TRUE(channel != nullptr);
  EXPECT_EQ(channel->get_event_type(), typeid(const TestEvents::TEST));

  // Test deduction for two similar events
  auto channel2 = event_bus_impl_->Publish(
      std::make_shared<::Event<const TestEvents::OK>>(ok_event_));
  auto channel3 = event_bus_impl_->Publish(
      std::make_shared<::Event<const TestEvents::ERROR>>(error_event_));

  ASSERT_FALSE(channel2->get_event_type() == channel3->get_event_type());
  EXPECT_EQ(channel2->get_event_type(), typeid(const TestEvents::OK));
  EXPECT_EQ(channel3->get_event_type(), typeid(const TestEvents::ERROR));

  // Test deduction for two equal events
  channel4 = event_bus_impl_->Publish(
      std::make_shared<::Event<const TestEvents::TEST>>(test_event_2));

  EXPECT_TRUE(channel->get_event_type() == channel4->get_event_type());
}

TEST_F(EventBusImplTest, ChannelRetrieval) {
  // Publish an event
  auto channel = event_bus_impl_->Publish(
      std::make_shared<::Event<const TestEvents::TEST>>(test_event_));
  ASSERT_TRUE(channel != nullptr);
  auto channel2 = event_bus_impl_->Publish(
      std::make_shared<::Event<const TestEvents::TEST>>(test_event_2));

  // Retrieve the right channel
  auto channel_receive = event_bus_impl_->GetChannel<const TestEvents::TEST>();
  EXPECT_EQ(channel, channel_receive);

  // Check that it matches with the second channel
  EXPECT_EQ(channel, channel2);

  // Check for a non existing channel
  auto channel_receive2 = event_bus_impl_->GetChannel<const TestEvents::OK>();
  EXPECT_EQ(channel_receive2, nullptr);
}

TEST_F(EventBusImplTest, ChannelRemoval) {
  // Publish an event
  auto channel = event_bus_impl_->Publish(
      std::make_shared<::Event<const TestEvents::TEST>>(test_event_));
  ASSERT_TRUE(channel != nullptr);

  // Check that the channel is correctly added to the EventBusImpl
  EXPECT_EQ(event_bus_impl_->get_channel_count(), 1);
  EXPECT_NE(event_bus_impl_->get_data_size(), 0);

  // Remove the channel
  event_bus_impl_->RemoveChannel<const TestEvents::TEST>();

  // Check that the channel is correctly removed from the EventBusImpl
  EXPECT_EQ(event_bus_impl_->get_channel_count(), 0);
  EXPECT_EQ(event_bus_impl_->get_data_size(), 0);
}
}  // namespace
}  // namespace habitify_testing
}  // namespace habitify_event_bus
