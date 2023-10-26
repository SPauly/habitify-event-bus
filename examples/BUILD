load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "minimal_event_bus_example",
    srcs = ["minimal_event_bus_example.cpp"],
    copts = ["-std=c++20"],
    deps = [
        "//:habitify_event_bus",
    ],
)
