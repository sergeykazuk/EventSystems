# ADR 0003: Timer Single-Shot Policy

- Status: Implemented
- Date: 2026-03-20
- Scope: StaticEventSystem

## Context

StaticEventSystem now supports timed events through `EventSystemTimer` and core `EventSystem` APIs:

- `sendTimedEvent(event, timeout)`
- `stopTimedEvent(event)`

Timed events currently target one-shot scheduling behavior. The timer queue may contain many timers, but each scheduled timer contributes exactly one timeout callback when it expires.

`TimedData` is the typed payload for timed events and currently carries:

- `triggers_count`

Because repetitive timers are not implemented yet, emitted timed payloads use `triggers_count = 1`.

## Decision

Adopt a single-shot timer policy for the current implementation:

- Each `startTimer` call schedules one timeout occurrence.
- Expired callbacks deliver a vector of expired event ids, one entry per expired timer occurrence.
- Core event system emits `TimedData{1}` for each expired timed event.
- No repetition, interval re-arming, or aggregated hit-count reporting is supported in this phase.

## Rationale

- Keeps timer behavior simple and deterministic while integrating into existing event queue/dispatch flow.
- Avoids premature complexity around repetition state, missed-tick policy, and lifecycle edge cases.
- Preserves a typed event contract for timed events via `TimedData`.

## Consequences

- Timed payloads are forward-compatible but currently low-information (`triggers_count` is always `1`).
- Applications that need periodic behavior must currently reschedule explicitly.
- Repetitive scheduling semantics are intentionally deferred.

## Future Plans

When repetitive timers are introduced, this ADR should be superseded or amended with explicit policy choices:

- API shape: one-shot and repetitive timer start methods, including interval specification.
- Investigate whether timer accuracy can be improved from current state.

## Notes

This decision aligns with current implementation goals: practical correctness first, then staged feature expansion.