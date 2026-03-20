#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Generate EventHandlers headers/sources from Cheetah templates and JSON configs")
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[1], help="StaticEventSystem root path")
    parser.add_argument("--header-template", type=Path, default=None, help="Path to EventHandlers.hpp.tmpl")
    parser.add_argument("--source-template", type=Path, default=None, help="Path to EventHandlers.cpp.tmpl")
    parser.add_argument("--event-handlers-config", type=Path, default=None, help="Path to event_handlers.json")
    parser.add_argument("--events-types-config", type=Path, default=None, help="Path to events_types.json")
    parser.add_argument("--out-header-dir", type=Path, default=None, help="Output directory for generated headers")
    parser.add_argument("--out-source-dir", type=Path, default=None, help="Output directory for generated sources")
    return parser.parse_args()


def load_json(path: Path) -> dict:
    with path.open("r", encoding="utf-8") as file:
        return json.load(file)


def main() -> int:
    args = parse_args()

    root = args.root.resolve()
    header_template_path = (args.header_template or (root / "templates" / "EventHandlers.hpp.tmpl")).resolve()
    source_template_path = (args.source_template or (root / "templates" / "EventHandlers.cpp.tmpl")).resolve()
    event_handlers_config_path = (args.event_handlers_config or (root / "configs" / "event_handlers.json")).resolve()
    events_types_config_path = (args.events_types_config or (root / "configs" / "events_types.json")).resolve()
    out_header_dir = (args.out_header_dir or (root / "generated" / "include" / "handlers")).resolve()
    out_source_dir = (args.out_source_dir or (root / "generated" / "src" / "handlers")).resolve()

    missing = [
        str(path)
        for path in (header_template_path, source_template_path, event_handlers_config_path, events_types_config_path)
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
    events_types_config = load_json(events_types_config_path)

    handlers = event_handlers_config.get("handlers", {})
    events_payload_map = events_types_config.get("events", {})
    all_types_map = events_types_config.get("types", {})
    base_datatypes = all_types_map.get("base_datatypes", {})

    out_header_dir.mkdir(parents=True, exist_ok=True)
    out_source_dir.mkdir(parents=True, exist_ok=True)

    for handler_name, handler_cfg in handlers.items():
        handler_events = handler_cfg.get("events", [])
        payload_types: set[str] = set()
        for event_name in handler_events:
            payload_type = events_payload_map.get(event_name, "")
            if payload_type:
                payload_types.add(payload_type)

        payload_decls: list[dict[str, str]] = []
        for payload_type in sorted(payload_types):
            payload_cfg = all_types_map.get(payload_type, {})
            payload_kind = payload_cfg.get("type", "struct")

            payload_decl: dict[str, str] = {
                "name": payload_type,
                "kind": payload_kind,
            }

            if payload_kind == "enum":
                base_type_name = payload_cfg.get("base_type", "int")
                payload_decl["base_type"] = base_datatypes.get(base_type_name, base_type_name)

            payload_decls.append(payload_decl)

        context = {
            "namespace_name": "event_system",
            "handler_name": handler_name,
            "handler_events": handler_events,
            "events_payload_map": events_payload_map,
            "payload_types": sorted(payload_types),
            "payload_decls": payload_decls,
            "uses_size_t": False,
        }

        try:
            rendered_header = str(Template(file=str(header_template_path), searchList=[context]))
            rendered_source = str(Template(file=str(source_template_path), searchList=[context]))
        except Exception as error:
            print(f"Template rendering failed for {handler_name}: {error}", file=sys.stderr)
            return 3

        out_header_path = out_header_dir / f"I{handler_name}.hpp"
        out_source_path = out_source_dir / f"I{handler_name}.cpp"
        out_header_path.write_text(rendered_header, encoding="utf-8")
        out_source_path.write_text(rendered_source, encoding="utf-8")

        print(f"Generated: {out_header_path}")
        print(f"Generated: {out_source_path}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
