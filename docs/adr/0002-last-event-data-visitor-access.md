# ADR 0002: Last Event Data Access via Visitor

- Status: Implemented
- Date: 2026-03-19
- Scope: StaticEventSystem

## Context

StaticEventSystem stores last event payloads as erased byte buffers.

A direct "return payload" API creates tradeoffs:

- Returning references can expose lifetime and synchronization hazards.
- Returning deep-copied raw buffers might create condition, when source data stored might become invalid.
- Returning typed values from the core queue would couple core code to payload type headers.

At the same time, the generated API should remain typed and convenient for app code.

## Decision

Expose callback-based last-event access in queue/system core:

- `getLastEventData(eventId, visitor)` is the single low-level access path.
- Core keeps erased payload storage and does not expose typed payload dependencies.
- Generated `system/LastEventGetters.*` performs typed conversion and returns typed results via `bool + out parameter`.

## Rationale

- Keeps critical section small and scoped to callback execution.
- Avoids returning raw deep-copy buffers to callers.
- Preserves layering: core remains type-erased, generated system layer handles payload typing.
- Reduces API surface duplication by standardizing on one low-level access pattern.

## Consequences

- Callers of core/system low-level API must use a visitor lambda.
- Typed convenience remains available through generated last-event getters.
- Header bloat is reduced by using forward declarations in generated getter headers.

## Notes

This decision complements ADR 0001 (discard-on-stop policy) and follows the same safety-first shutdown and lifetime model.
