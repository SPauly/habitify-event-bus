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

#include <habitify_event_bus/event.h>

namespace habitify_event_bus {
namespace habitify_testing {
namespace {

class EventTest : public ::testing::Test {
 protected:
  void SetUp() override {}

 protected:
  std::string test_string_{"test_string"};
  int test_int = 42;

  enum class CustomEvents : habitify_event_bus::EventType {
    kString = 0,  // String
    kInt = 1      // Int
  };
};

TEST_F(EventTest, Initialization) {
  ::habitify_event_bus::Event<std::string> string_event(CustomEvents::kString,
                                                        test_string_);
  EXPECT_EQ(string_event.get_event_type(), EventType::kString);
  EXPECT_EQ(string_event.GetData<std::string>(), test_string_);
}

}  // namespace
}  // namespace habitify_testing

}  // namespace habitify_event_bus
