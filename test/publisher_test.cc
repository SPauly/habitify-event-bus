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

#include <habitify_event_bus/publisher.h>

namespace habitify_event_bus {
namespace habitify_testing {
namespace {

/// TODO: Create a Mock object of EventBusImpl to test that the Publisher calls
/// the correct functions and provides the correct events to the EventBroker.
class PublisherTest : public ::testing::Test {
 protected:
  void SetUp() override {}

 protected:
};

}  // namespace
}  // namespace habitify_testing
}  // namespace habitify_event_bus