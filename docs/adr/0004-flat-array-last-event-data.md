# ADR 0004: Flat Static Array for Last Event Data

- Status: Implemented
- Date: 2026-03-30
- Scope: StaticEventSystem

## Context

StaticEventSystem must efficiently store and retrieve the last payload for each event type. The system is designed for predictable, low-overhead access patterns and minimal runtime allocations. Previous ADRs (see ADR-0002) established that last event data is accessed via a visitor/callback API, with type erasure at the core layer.

## Decision

Store last event data in a flat static array, indexed directly by event id (enum value):

- The array is allocated at system startup, sized to the maximum event id + 1.
- Each slot holds a type-erased buffer for the last payload of that event.
- Access is O(1) and does not require hashing or dynamic lookup.

## Rationale

- **Performance:** Direct indexing is faster than map or vector search, especially for small, dense event id spaces.
- **Predictability:** Memory layout and access time are fixed and easy to reason about.
- **Simplicity:** No need for dynamic allocation or resizing after startup; avoids fragmentation and allocator overhead.
- **Safety:** Eliminates risk of missing/uninitialized slots for valid event ids.
- **Compatibility:** Works seamlessly with the visitor-based access pattern (ADR-0002) and type-erased storage.

## Consequences

- Memory usage is proportional to the maximum event id, not the number of active events. For sparse id spaces, this may waste some memory.
- Adding new event ids increases the static array size at next code generation/build.
- Access patterns are cache-friendly and suitable for real-time or embedded use cases.
- The implementation is simple to audit and debug.

