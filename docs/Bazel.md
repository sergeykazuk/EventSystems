# Bazel Build Guide

This page describes how to build the projects with Bazel, and why Bazel can be a strong choice as systems grow.

## Why Bazel Is Interesting

For this repository size, CMake is already fully workable and often simpler to start with.

Bazel becomes more valuable when projects get larger, especially when you have:

- many targets and transitive dependencies
- frequent incremental builds in active development
- multi-platform CI pipelines that must stay consistent
- multiple teams changing shared modules in parallel

In those cases Bazel typically improves:

- build reproducibility (same inputs, same outputs)
- caching and incremental build speed
- dependency boundary clarity between modules
- CI reliability across machines/environments

## Current Scope In This Repository

At this project size, Bazel is still useful as:

- an additional build path for validation
- a portability check beyond CMake
- a foundation for future scaling

It is not mandatory to replace CMake. Both can coexist.

## Prerequisites

- Bazel installed (Bazel 9.x is currently used)
- C++ compiler toolchain available on your OS

## Build Targets

From repository root, build each implementation:

```bash
bazel build //PubSub:test_app
bazel build //DynamicEventSystem:test_app
bazel build //StaticEventSystem:test_app
```

Build all three in one command:

```bash
bazel build //PubSub:test_app //DynamicEventSystem:test_app //StaticEventSystem:test_app
```

## Platform Notes

The repository uses platform-specific settings in `.bazelrc`.

### macOS

- `--cxxopt=-std=c++17`
- `--macos_minimum_os=10.15`

The minimum macOS version is important because `std::filesystem` availability in Apple libc++ depends on deployment target. Without `10.15` or newer, builds can fail with filesystem availability errors.

### Linux

- `--cxxopt=-std=c++17`

### Windows

- `--cxxopt=/std:c++17`
- `--host_cxxopt=/std:c++17`

## Running Binaries

After build, Bazel outputs are under Bazel-managed output directories. You can run binaries with:

```bash
bazel run //PubSub:test_app
bazel run //DynamicEventSystem:test_app
bazel run //StaticEventSystem:test_app
```

For this repository, keeping both options is a pragmatic approach.
