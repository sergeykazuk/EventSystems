# Event Systems: A Comparative Study

This repository demonstrates different approaches to event-driven architecture in C++17, comparing their design trade-offs, performance characteristics, and use cases.

## Project Overview

The goal is to showcase multiple event-passing mechanisms side-by-side.
Each implementation will be benchmarked and documented to help developers choose the right approach for their needs.

---

## Phase 1: Publisher-Subscriber Pattern

### What is Pub/Sub?

The Publisher-Subscriber pattern is a messaging paradigm where:
- **Publishers** emit events without knowing who will consume them
- **Subscribers** register callbacks to receive events they're interested in
- **Loose coupling**: Publishers and subscribers are independent

### When to Use Pub/Sub

**Good use cases:**
- Event logging and monitoring systems
- UI event handling (button clicks, notifications)
- Asynchronous workflows (file processing, data pipelines)
- Plugin architectures with unknown subscribers at compile-time
- Decoupling system components

**Not ideal for:**
- Direct request-response communication
- Performance-critical hot paths (callback overhead)
- Simple 1:1 communication (use direct calls)
- Guaranteed delivery requirements (needs queuing)

### Advantages

- **Decoupling**: Publishers don't depend on subscriber implementations
- **Extensibility**: Add new subscribers without modifying publishers
- **Runtime flexibility**: Subscribe/unsubscribe dynamically
- **Broadcast**: One event → multiple handlers

### Disadvantages

- **Debugging difficulty**: Event flow is implicit, harder to trace
- **Memory management**: Callbacks can create lifetime issues
- **No delivery guarantees**: Fire-and-forget model
- **Blocking callback chain**: The processing thread is blocked until all callbacks finish; slow handlers or downstream component calls can delay subsequent events
- **Performance**: Slower than direct calls or static dispatch

---

## Architecture

### Class Diagram

![class diagram](PubSub/images/class_diagram.png)

### Sequence Diagram

![sequence diagram](PubSub/images/sequence_diagram.png)

---

## Building the Project

### Prerequisites

- **CMake** 3.16 or higher
- **C++17** compatible compiler:
  - GCC 7+
  - Clang 5+
  - MSVC 2017+
- **macOS / Linux / Windows**

### Build Instructions

```bash
# Clone the repository
git clone <repository-url>
cd EventSystems

# Configure and build
cd PubSub
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . -j4

# Run the example
./PubSubExample
```

Press **Ctrl+C** to gracefully shut down.

---

## Quick Start

### Usage Example

```cpp
#include "Publisher.hpp"
#include "Subscriber.hpp"
#include "CsvDataReader.hpp"

int main() {
    // Create publisher
    PubSub::Publisher publisher;
    
    // Create subscriber (auto-registers callbacks)
    PubSub::Subscriber subscriber(publisher);
    
    // Start reading data from CSV files
    PubSub::CsvDataReader reader(publisher);
    reader.startReadingData();
    
    // Main thread waits for Ctrl+C...
    
    reader.stop();
    return 0;
}
```

The `Subscriber` will automatically print events as they arrive:

```
1 {name: Alice'}
2 {name: Bob'}
1 {user 'Alice' ordered 'Laptop purchase'}
2 {user 'Bob' ordered 'Software license'}
```

**Testing tip:** While the application is running, you can append new entries to `PubSub/Data/users.csv` and `PubSub/Data/orders.csv`. The CSV reader keeps polling the files and will process newly added lines when they are terminated with `;`.

---

## Learn More

- [Observer Pattern](https://refactoring.guru/design-patterns/observer)
- [Event-Driven Architecture](https://martinfowler.com/articles/201701-event-driven.html)
- [C++ Callbacks and Signals](https://www.boost.org/doc/libs/1_84_0/doc/html/signals2.html)

---

## License

MIT License - See LICENSE file for details

---

## Contributing

Contributions welcome! Please open an issue first to discuss proposed changes.

---

**Author**: Sergey Kazuk  
**Started**: February 2026
