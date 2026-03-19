# ADR 0001: Static Event Queue Stop Policy

- Status: Implemented
- Date: 2026-03-19
- Scope: StaticEventSystem

## Context

During event system shutdown, handler instances may start destruction and call unregister logic while queue worker thread still has pending events.

If queued events are drained during this phase, event dispatch can race with handler lifetime transitions. This can produce non-deterministic shutdown behavior and makes safety guarantees unclear.

## Decision

For StaticEventSystem, shutdown uses discard policy for the event queue:

- Stop accepting new events.
- Discard queued but not-yet-dispatched events.
- Stop worker thread.

## Rationale

- Prioritize lifetime safety over best-effort delivery during shutdown.
- Keep shutdown deterministic and easy to reason about.
- Avoid dispatching to handlers that may already be unregistering.

## Consequences

- Some in-flight queued events may be dropped at shutdown.
- Runtime behavior is clearer and safer for this architecture.
- If drain semantics are required in future use cases, they should be opt-in and guarded by stronger lifetime guarantees.

## Notes

This decision applies to StaticEventSystem only.
Other implementations can make independent shutdown policy decisions.
