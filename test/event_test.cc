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

#include <memory>
#include <string>
#include <typeinfo>

#include <habitify_event_bus/impl/event_base.h>
#include <habitify_event_bus/event.h>

namespace habitify_event_bus {
namespace habitify_testing {
namespace {

class EventTest : public ::testing::Test {
 protected:
  void SetUp() override {
    event_base_ = std::make_shared<const EventBase>();
    event_base_int_ = std::make_shared<const EventBase>(typeid(int));

    event_int_ = std::make_shared<const Event<int>>(1);
    event_int_2_ = std::make_shared<const Event<int>>(2);
    event_char_ = std::make_shared<const Event<char>>('A');
  }

 protected:
  EventConstBasePtr event_base_, event_base_int_;
  EventConstPtr<int> event_int_, event_int_2_;
  EventConstPtr<char> event_char_;
};

TEST_F(EventTest, BaseInitialization) {
  ASSERT_NE(event_base_, nullptr);
  EXPECT_EQ(event_base_->get_id(), 0);

  ASSERT_NE(event_base_int_, nullptr);
  EXPECT_EQ(event_base_int_->get_event_type(), typeid(int));
}

TEST_F(EventTest, BaseSetters) {
  ASSERT_NE(event_base_, nullptr);
  ASSERT_NE(event_base_int_, nullptr);

  // Test EventId
  EXPECT_EQ(event_base_->set_id(1), true);
  EXPECT_EQ(event_base_->get_id(), 1);
  EXPECT_EQ(event_base_->set_id(2), false);
  EXPECT_EQ(event_base_->get_id(), 1);

  // Test EventType
  EXPECT_EQ(event_base_int_->get_event_type(), typeid(int));
  event_base_int_->set_event_type(typeid(char));
  EXPECT_EQ(event_base_int_->get_event_type(), typeid(char));

  // Test PublisherId
  EXPECT_EQ(event_base_->set_publisher_id(123), true);
  EXPECT_EQ(event_base_->get_publisher_id(), 123);
}

TEST_F(EventTest, EventTypeComparison) {
  EXPECT_TRUE(::internal::IsSameEventType<event_int_, event_int_2_>, true);
  EXPECT_FALSE(::internal::IsSameEventType<event_int_, event_char_>);

  // Test behaviour with const difference
  EventConstPtr<const int> const_int_e =
      std::make_shared<const Event<const int>>(1);
  EXPECT_FALSE(::internal::IsSameEventType<event_int_, const_event_e>);

  // Test behaviour with pointer difference
  EventConstPtr<int*> ptr_int_e =
      std::make_shared<const Event<int*>>(&event_int_);
  EXPECT_FALSE(::internal::IsSameEventType<event_int_, ptr_int_e>);
}

TEST_F(EventTest, Initialization) {
  EXPECT_EQ(event_int_->GetData<int>(), 1);
  EXPECT_EQ(event_int_2_->GetData<int>(), 2);
  EXPECT_EQ(event_char_->GetData<char>(), 'A');

  /// TODO: Test static_asserts somewhat
}
}  // namespace
}  // namespace habitify_testing

}  // namespace habitify_event_bus
