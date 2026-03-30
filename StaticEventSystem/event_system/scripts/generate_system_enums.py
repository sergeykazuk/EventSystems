#!/usr/bin/env python3
from __future__ import annotations

import argparse
from pathlib import Path

from gen_common import load_json, render_enum, write


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Generate system enum headers")
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[1], help="StaticEventSystem root")
    parser.add_argument("--events-enum", type=Path, default=None, help="Path to events_enum.json")
    parser.add_argument("--handlers", type=Path, default=None, help="Path to event_handlers.json")
    parser.add_argument("--out-include", type=Path, default=None, help="Output include dir")
    return parser.parse_args()


def generate(root: Path, events_enum_path: Path, handlers_path: Path, out_include: Path) -> None:
    events_enum = load_json(events_enum_path)
    handlers = load_json(handlers_path)

    namespace = "event_system"

    event_ids = events_enum.get("event_ids", {})
    timed_event_ids = events_enum.get("")
    handler_names = handlers.get("handlers", {}).keys()
    queue_names = handlers.get("EventQueues", [])

    event_enum_members = {"eUnknown": "0"}
    event_enum_members.update({f"e{name}": value for name, value in event_ids.items()})
    event_enum_members.update({f"eCount": ""})


    handler_members = {}
    for idx, name in enumerate(handler_names, start=0):
        handler_members[f"e{name}"] = str(idx)
    handler_members[f"eHandlersCount"] = ""

    queue_members = {"eUnknown": "0"}
    for idx, name in enumerate(queue_names, start=1):
        queue_members[f"e{name[:1].upper() + name[1:]}"] = str(idx)

    write(out_include / "core" / "EventTypeEnum.hpp", render_enum(namespace, "EventTypeEnum", "uint16_t", event_enum_members))
    write(out_include / "core" / "EventHandlerId.hpp", render_enum(namespace, "EventHandlerId", "uint16_t", handler_members))
    write(out_include / "core" / "EventQueueId.hpp", render_enum(namespace, "EventQueueId", "uint16_t", queue_members))


def main() -> int:
    args = parse_args()
    root = args.root.resolve()
    events_enum_path = (args.events_enum or root / "configs" / "events_enum.json").resolve()
    handlers_path = (args.handlers or root / "configs" / "event_handlers.json").resolve()
    out_include = (args.out_include or root / "generated" / "include").resolve()

    generate(root, events_enum_path, handlers_path, out_include)
    print(f"Generated system enums in: {out_include / 'core'}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
