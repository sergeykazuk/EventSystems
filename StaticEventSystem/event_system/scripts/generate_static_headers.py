#!/usr/bin/env python3
from __future__ import annotations

import argparse
import sys
from pathlib import Path
from typing import Any
from collections import defaultdict

from gen_common import load_json, render_type_header, write
from generate_system_enums import generate as generate_system_enums


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Generate static event-system headers from JSON configs")
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[1], help="StaticEventSystem root")
    parser.add_argument("--events-types", type=Path, default=None, help="Path to events_types.json")
    parser.add_argument("--events-enum", type=Path, default=None, help="Path to events_enum.json")
    parser.add_argument("--handlers", type=Path, default=None, help="Path to event_handlers.json")
    parser.add_argument("--helpers-template", type=Path, default=None, help="Deprecated: old helper header template path")
    parser.add_argument("--helpers-cpp-template", type=Path, default=None, help="Deprecated: old helper cpp template path")
    parser.add_argument("--senders-template", type=Path, default=None, help="Path to EventSenders.hpp.tmpl")
    parser.add_argument("--senders-cpp-template", type=Path, default=None, help="Path to EventSenders.cpp.tmpl")
    parser.add_argument("--payload-helpers-template", type=Path, default=None, help="Path to EventPayloadHelpers.hpp.tmpl")
    parser.add_argument("--payload-helpers-cpp-template", type=Path, default=None, help="Path to EventPayloadHelpers.cpp.tmpl")
    parser.add_argument("--out-include", type=Path, default=None, help="Output include dir")
    parser.add_argument("--out-source", type=Path, default=None, help="Output source dir")
    return parser.parse_args()


def render_cheetah_template(template_path: Path, context: dict[str, Any]) -> str:
    try:
        from Cheetah.Template import Template
    except Exception as error:
        print(f"Failed to import Cheetah.Template: {error}", file=sys.stderr)
        print("Install dependency with: pip install Cheetah3", file=sys.stderr)
        raise SystemExit(2)

    try:
        return str(Template(file=str(template_path), searchList=[context]))
    except Exception as error:
        print(f"Template rendering failed ({template_path.name}): {error}", file=sys.stderr)
        raise SystemExit(3)


def main() -> int:
    args = parse_args()
    root = args.root.resolve()

    events_types_path = (args.events_types or root / "configs" / "events_types.json").resolve()
    events_enum_path = (args.events_enum or root / "configs" / "events_enum.json").resolve()
    handlers_path = (args.handlers or root / "configs" / "event_handlers.json").resolve()
    senders_template_path = (args.senders_template or args.helpers_template or root / "templates" / "EventSenders.hpp.tmpl").resolve()
    senders_cpp_template_path = (args.senders_cpp_template or args.helpers_cpp_template or root / "templates" / "EventSenders.cpp.tmpl").resolve()
    payload_helpers_template_path = (args.payload_helpers_template or root / "templates" / "EventPayloadHelpers.hpp.tmpl").resolve()
    payload_helpers_cpp_template_path = (args.payload_helpers_cpp_template or root / "templates" / "EventPayloadHelpers.cpp.tmpl").resolve()
    out_include = (args.out_include or root / "generated" / "include").resolve()
    out_source = (args.out_source or root / "generated" / "src").resolve()

    missing_templates = [
        path
        for path in (
            senders_template_path,
            senders_cpp_template_path,
            payload_helpers_template_path,
            payload_helpers_cpp_template_path,
        )
        if not path.exists()
    ]
    if missing_templates:
        for missing in missing_templates:
            print(f"Missing helpers template: {missing}", file=sys.stderr)
        return 1

    events_types = load_json(events_types_path)
    generate_system_enums(root, events_enum_path, handlers_path, out_include)

    namespace = "event_system"

    types_section = events_types.get("types", {})
    base_aliases = types_section.get("base_datatypes", {})
    user_defined_types = {name for name in types_section.keys() if name != "base_datatypes"}
    events_map = events_types.get("events", {})

    for type_name, type_def in types_section.items():
        if type_name == "base_datatypes":
            continue
        write(
            out_include / "types" / f"{type_name}.hpp",
            render_type_header(namespace, type_name, type_def, base_aliases, user_defined_types),
        )

    payload_types = {payload for payload in events_map.values() if payload}

    excluded_sender_events = {"EventSystemReady", "EventSystemShutdown"}
    payload_to_events: dict[str, list[str]] = defaultdict(list)
    for event_name, payload_type in events_map.items():
        if event_name in excluded_sender_events or not payload_type:
            continue
        payload_to_events[payload_type].append(event_name)

    ambiguous_payloads = {
        payload_type: sorted(event_names)
        for payload_type, event_names in payload_to_events.items()
        if len(event_names) > 1
    }
    if ambiguous_payloads:
        print(
            "Cannot generate overloaded sendEvent(...) API: payload type reused by multiple events.",
            file=sys.stderr,
        )
        for payload_type, event_names in sorted(ambiguous_payloads.items()):
            print(
                f"- Payload '{payload_type}' is used by events: {', '.join(event_names)}",
                file=sys.stderr,
            )
        print(
            "Use unique payload types per event or keep explicit per-event sender names.",
            file=sys.stderr,
        )
        return 4

    payload_decls: list[dict[str, str]] = []
    for payload_type in sorted(payload_types):
        payload_cfg = types_section.get(payload_type, {})
        payload_kind = payload_cfg.get("type", "struct")

        payload_decl: dict[str, str] = {
            "name": payload_type,
            "kind": payload_kind,
        }

        if payload_kind == "enum":
            base_type_name = payload_cfg.get("base_type", "int")
            payload_decl["base_type"] = base_aliases.get(base_type_name, base_type_name)

        payload_decls.append(payload_decl)

    helper_context = {
        "namespace_name": namespace,
        "events_map": events_map,
        "payload_types": sorted(payload_types),
        "payload_decls": payload_decls,
    }

    write(
        out_include / "system" / "EventSenders.hpp",
        render_cheetah_template(senders_template_path, helper_context),
    )
    write(
        out_source / "system" / "EventSenders.cpp",
        render_cheetah_template(senders_cpp_template_path, helper_context),
    )
    write(
        out_include / "core" / "EventPayloadHelpers.hpp",
        render_cheetah_template(payload_helpers_template_path, helper_context),
    )
    write(
        out_source / "core" / "EventPayloadHelpers.cpp",
        render_cheetah_template(payload_helpers_cpp_template_path, helper_context),
    )

    print(f"Generated headers in: {out_include}")
    print(f"Generated sources in: {out_source}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
