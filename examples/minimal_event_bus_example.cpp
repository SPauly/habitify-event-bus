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

// This is a minimal example of how to use the event bus.
// Refer to this example as documentation for now.

#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "include/habitify_event_bus.h"

int main() {
  int amount_of_events = 0;
  std::shared_ptr<habitify_event_bus::EventBus> event_bus =
      habitify_event_bus::EventBus::Create();

  std::cout << "Enter amount of events to share: " << std::endl;
  std::cin >> amount_of_events;

  // Create a thread that publishes amount_of_events to channel 0 every 10ms
  std::cout << "Starting Publisher: " << std::endl;
  std::thread publisher_thread([event_bus, amount_of_events]() {
    int event_count = 0;
    std::shared_ptr<habitify_event_bus::Publisher<int>> publisher =
        event_bus->CreatePublisher<int>(0);
    while (event_count < amount_of_events) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      std::cout << "Publishing event: " << event_count++ << std::endl;
      publisher->Publish(std::make_unique<const habitify_event_bus::Event<int>>(
          habitify_event_bus::EventType::TEST, 0, &event_count));
    }
  });

  // Create a thread that subscribes to channel 0 and subscibes to the incomming
  // events
  std::cout << "Starting Listener: " << std::endl;
  std::thread listener_thread([event_bus, amount_of_events]() {
    std::shared_ptr<habitify_event_bus::Listener> listener =
        event_bus->CreateSubscriber(0);
    int event_count = 0;

    while (event_count < amount_of_events) {
      auto current_event = listener->ReadLatest<int>();
      std::cout << "Received event: " << *current_event->GetData<int>()
                << std::endl;
      event_count = *current_event->GetData<int>();
    }
  });

  publisher_thread.join();
  listener_thread.join();

  std::cout << "Joined threads" << std::endl;
  return 0;
}