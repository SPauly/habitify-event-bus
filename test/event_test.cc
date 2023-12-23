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
    event_base_ = std::make_shared<const EventBase>(111);
    event_base_int_ = std::make_shared<const EventBase>(typeid(int), 222);
  }

 protected:
  EventConstBasePtr event_base_, event_base_int_;
  EventConstPtr<int> event_int_;
  EventConstPtr<char> event_char_;
};

TEST_F(EventTest, BaseInitialization) {
  ASSERT_NE(event_base_, nullptr);
  EXPECT_EQ(event_base_->get_id(), 111);

  ASSERT_NE(event_base_int_, nullptr);
  EXPECT_EQ(event_base_int_->get_id(), 222);
  EXPECT_EQ(event_base_int_->get_event_type(), typeid(int));
}

TEST_F(EventTest, BaseSetters) {
  ASSERT_NE(event_base_, nullptr);
  ASSERT_NE(event_base_int_, nullptr);

  // Test EventId
  EXPECT_EQ(event_base_->set_id(1), false);
  EXPECT_NE(event_base_->get_id(), 1);

  // Test EventType
  EXPECT_EQ(event_base_int_->get_event_type(), typeid(int));
  event_base_int_->set_event_type(typeid(char));
  EXPECT_EQ(event_base_int_->get_event_type(), typeid(char));

  // Test PublisherId
  EXPECT_EQ(event_base_->set_publisher_id(123), true);
  EXPECT_EQ(event_base_->get_publisher_id(), 123);
}
}  // namespace
}  // namespace habitify_testing

}  // namespace habitify_event_bus
