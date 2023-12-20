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

namespace TestEvents {
struct OK {};
struct ERROR {};
struct TEST {
  int a;
  std::string b;
};
}  // namespace TestEvents

class EventBusTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Create event bus, listener and publisher
    listener_ = event_bus_->CreateListener();
    publisher_ = event_bus_->CreatePublisher();
  }

 protected:
  // Utils for main Test case:
  EventBus event_bus_;
  Listener listener_;
  Publisher publisher_;

  // Events
  TestEvents::OK ok_event_;
  TestEvents::ERROR error_event_;
  TestEvents::TEST test_event_ = {42, "Test String"};
};

TEST_F(EventBusTest, Initialization) {
  // Check if all actors are initialized properly when using default create on
  // stack.
  EXPECT_TRUE(listener_.is_initialized());
  EXPECT_TRUE(publisher_.is_initialized());

  // Check if all actors are initialized properly when creating them on the heap
  EventBusPtr event_bus_shared = std::make_shared<EventBus>();
  ListenerPtr listener_shared = event_bus_shared->CreateSharedListener();
  PublisherPtr publisher_shared = event_bus_shared->CreateSharedPublisher();

  EXPECT_TRUE(listener_shared->is_initialized());
  EXPECT_TRUE(publisher_shared->is_initialized());
}

TEST_F(EventBusTest, PublishAndReceive) {
  // Publish an event and check if it is received
  Event<const TestEvents::Test> event;
  ASSERT_TRUE(publisher_.Publish(test_event_));
  EXPECT_TRUE(listener_.GetEvent(event));

  EventConstPtr<EventType::TEST> latest_event_ =
      listener_.ReadLatest<EventType::TEST>();
  EXPECT_EQ(latest_event_->GetData(), test_event_);

  // More in depth test of the Publish and Receive functions are performed in
  // the Publisher and Listener tests. Here it is more important to test the
  // integration of the different actors in an asynchronous environment.
}

TEST_F(EventBusTest, ThreadSafety) {
  int received_messages = 0, latest_data;
  // Test threadsafety of the event bus
  std::thread listener_thread([&]() {
    while (listener_.Listen([&received_messages](const TestEvents::TEST& e) {
      latest_data = e.GetData().a;
    }) == ListenerStatus::kOK) {
      received_messages++;
    };
  });

  std::thread publisher_thread([&]() {
    // Publish 100 events
    for (int i = 0; i < 100; i++) {
      TestEvents::TEST test_event = {i, "Test String"};
      EXPECT_TRUE(publisher_.Publish(test_event));
    }
    publisher_.CloseChannel<TestEvents::TEST>();
  });

  publisher_thread.join();
  listener_thread.join();

  EXPECT_EQ(received_messages, 100);
  EXPECT_EQ(latest_data, 99);
}

}  // namespace

}  // namespace habitify_testing

}  // namespace habitify_event_bus

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
