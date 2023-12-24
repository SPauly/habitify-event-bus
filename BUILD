load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "habitify_event_bus",
    srcs = [
        "src/Channel.cc",
        "src/event_bus_impl.cc",
        "src/listener.cc",
        "src/publisher.cc",
    ],
    includes = ["include"],
    visibility = ["//visibility:public"],
)
