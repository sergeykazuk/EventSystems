#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Generate EventHandlersMapBuilder.cpp from Cheetah template and JSON configs")
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[1], help="StaticEventSystem root path")
    parser.add_argument("--template", type=Path, default=None, help="Path to EventHandlersMapBuilder.cpp.tmpl")
    parser.add_argument("--event-handlers-config", type=Path, default=None, help="Path to event_handlers.json")
    parser.add_argument("--events-enum-config", type=Path, default=None, help="Path to events_enum.json")
    parser.add_argument("--output", type=Path, default=None, help="Output path for generated cpp")
    return parser.parse_args()


def load_json(path: Path) -> dict:
    with path.open("r", encoding="utf-8") as file:
        return json.load(file)


def main() -> int:
    args = parse_args()

    root = args.root.resolve()
    template_path = (args.template or (root / "templates" / "EventHandlersMapBuilder.cpp.tmpl")).resolve()
    event_handlers_config_path = (args.event_handlers_config or (root / "configs" / "event_handlers.json")).resolve()
    events_enum_config_path = (args.events_enum_config or (root / "configs" / "events_enum.json")).resolve()
    output_path = (args.output or (root / "generated" / "src" / "EventHandlersMapBuilder.cpp")).resolve()

    missing = [
        str(path)
        for path in (template_path, event_handlers_config_path, events_enum_config_path)
        if not path.exists()
    ]
    if missing:
        print("Missing required input files:", file=sys.stderr)
        for path in missing:
            print(f"- {path}", file=sys.stderr)
        return 1

    try:
        from Cheetah.Template import Template
    except Exception as error:
        print(f"Failed to import Cheetah.Template: {error}", file=sys.stderr)
        print("Install dependency with: pip install Cheetah3", file=sys.stderr)
        return 2

    event_handlers_config = load_json(event_handlers_config_path)
    events_enum_config = load_json(events_enum_config_path)

    context = {
        "event_handlers_config": event_handlers_config,
        "events_enum_config": events_enum_config,
    }

    try:
        rendered = str(Template(file=str(template_path), searchList=[context]))
    except Exception as error:
        print(f"Template rendering failed: {error}", file=sys.stderr)
        return 3

    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(rendered, encoding="utf-8")

    print(f"Generated: {output_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
