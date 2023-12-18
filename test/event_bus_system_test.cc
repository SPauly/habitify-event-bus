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
#include <thread>

#include <habitify_event_bus/event.h>
#include <habitify_event_bus/event_bus.h>

namespace habitify_event_bus {
namespace habitify_testing {
namespace {

class EventBusTest : public ::testing::Test {
 protected:
  void SetUp() override {
    event_bus_ = EventBus::Create();

    listener_ = event_bus_->CreateListener();
    publisher_ = event_bus_->CreatePublisher();
  }

 protected:
  // utils
  std::shared_ptr<EventBus> event_bus_;

  // Listeners
  std::shared_ptr<Listener> listener_;

  // Publishers
  std::shared_ptr<Publisher<int>> publisher_;
};

TEST_F(EventBusTest, Initialization) {
  // Check if all pointers are initialized properly
  EXPECT_TRUE(event_bus_ != nullptr);
  EXPECT_TRUE(listener_ != nullptr);
  EXPECT_TRUE(publisher_ != nullptr);
}

TEST_F(EventBusTest, PublishAndReceive) {
  // Publish an event and check if it is received
  ASSERT_TRUE(publisher_->Publish(EventType::TEST, "Test String"));
  EXPECT_TRUE(listener_->HasUnreadEvent());
  auto latest_event_ = listener_->ReadLatest(EventType::TEST);
  EXPECT_EQ(*latest_event_->GetData<std::string>(), "Test String");
}

TEST_F(EventBusTest, ThreadSafety) {
  int received_messages = 0, latest_data;
  // Test threadsafety of the event bus
  std::thread listener_thread([&]() {
    listener_->listen<int>(EventType::TEST,
                           [&received_messages](Event<int> &e) {
                             received_messages++;
                             latest_data = e.GetData<int>();
                           });
  });

  std::thread publisher_thread([&]() {
    // Publish 100 events
    for (int i = 0; i < 100; i++) {
      EXPECT_TRUE(publisher_->Publish(EventType::TEST, i));
    }
  });

  publisher_thread.join();
  listener_thread.join();

  EXPECT_EQ(received_messages, 100);
  EXPECT_EQ(latest_data, 99);
}

}  // namespace

}  // namespace habitify_testing

}  // namespace habitify_event_bus

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
