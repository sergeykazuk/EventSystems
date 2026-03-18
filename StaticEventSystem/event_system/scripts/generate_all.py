#!/usr/bin/env python3
from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Run all StaticEventSystem generators")
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

    parser.add_argument("--map-template", type=Path, default=None, help="Path to EventHandlersMapBuilder.cpp.tmpl")
    parser.add_argument("--out-map-cpp", type=Path, default=None, help="Output path for generated EventHandlersMapBuilder.cpp")

    parser.add_argument("--event-handlers-header-template", type=Path, default=None, help="Path to EventHandlers.hpp.tmpl")
    parser.add_argument("--event-handlers-source-template", type=Path, default=None, help="Path to EventHandlers.cpp.tmpl")
    parser.add_argument("--out-event-handlers-include-dir", type=Path, default=None, help="Output directory for generated handler headers")
    parser.add_argument("--out-event-handlers-source-dir", type=Path, default=None, help="Output directory for generated handler sources")

    parser.add_argument("--skip-static-headers", action="store_true", help="Skip static headers generation")
    parser.add_argument("--skip-map-builder", action="store_true", help="Skip EventHandlersMapBuilder generation")
    parser.add_argument("--skip-event-handlers", action="store_true", help="Skip EventHandlers.hpp generation")
    return parser.parse_args()


def run_command(command: list[str]) -> int:
    print("Running:", " ".join(command))
    result = subprocess.run(command)
    return result.returncode


def main() -> int:
    args = parse_args()
    root = args.root.resolve()
    scripts_dir = Path(__file__).resolve().parent

    events_types_path = (args.events_types or root / "configs" / "events_types.json").resolve()
    events_enum_path = (args.events_enum or root / "configs" / "events_enum.json").resolve()
    handlers_path = (args.handlers or root / "configs" / "event_handlers.json").resolve()

    senders_template_path = (args.senders_template or args.helpers_template or root / "templates" / "EventSenders.hpp.tmpl").resolve()
    senders_cpp_template_path = (args.senders_cpp_template or args.helpers_cpp_template or root / "templates" / "EventSenders.cpp.tmpl").resolve()
    payload_helpers_template_path = (args.payload_helpers_template or root / "templates" / "EventPayloadHelpers.hpp.tmpl").resolve()
    payload_helpers_cpp_template_path = (args.payload_helpers_cpp_template or root / "templates" / "EventPayloadHelpers.cpp.tmpl").resolve()
    out_include_path = (args.out_include or root / "generated" / "include").resolve()
    out_source_path = (args.out_source or root / "generated" / "src").resolve()

    map_template_path = (args.map_template or root / "templates" / "EventHandlersMapBuilder.cpp.tmpl").resolve()
    out_map_cpp_path = (args.out_map_cpp or root / "generated" / "src" / "EventHandlersMapBuilder.cpp").resolve()

    event_handlers_header_template_path = (args.event_handlers_header_template or root / "templates" / "EventHandlers.hpp.tmpl").resolve()
    event_handlers_source_template_path = (args.event_handlers_source_template or root / "templates" / "EventHandlers.cpp.tmpl").resolve()
    out_event_handlers_include_dir = (args.out_event_handlers_include_dir or root / "generated" / "include" / "handlers").resolve()
    out_event_handlers_source_dir = (args.out_event_handlers_source_dir or root / "generated" / "src" / "handlers").resolve()

    python_executable = sys.executable

    if not args.skip_static_headers:
        static_headers_cmd = [
            python_executable,
            str(scripts_dir / "generate_static_headers.py"),
            "--root",
            str(root),
            "--events-types",
            str(events_types_path),
            "--events-enum",
            str(events_enum_path),
            "--handlers",
            str(handlers_path),
            "--senders-template",
            str(senders_template_path),
            "--senders-cpp-template",
            str(senders_cpp_template_path),
            "--payload-helpers-template",
            str(payload_helpers_template_path),
            "--payload-helpers-cpp-template",
            str(payload_helpers_cpp_template_path),
            "--out-include",
            str(out_include_path),
            "--out-source",
            str(out_source_path),
        ]
        code = run_command(static_headers_cmd)
        if code != 0:
            return code

    if not args.skip_map_builder:
        map_builder_cmd = [
            python_executable,
            str(scripts_dir / "generate_event_handlers_map_builder.py"),
            "--root",
            str(root),
            "--template",
            str(map_template_path),
            "--event-handlers-config",
            str(handlers_path),
            "--events-enum-config",
            str(events_enum_path),
            "--output",
            str(out_map_cpp_path),
        ]
        code = run_command(map_builder_cmd)
        if code != 0:
            return code

    if not args.skip_event_handlers:
        event_handlers_cmd = [
            python_executable,
            str(scripts_dir / "generate_event_handlers.py"),
            "--root",
            str(root),
            "--header-template",
            str(event_handlers_header_template_path),
            "--source-template",
            str(event_handlers_source_template_path),
            "--event-handlers-config",
            str(handlers_path),
            "--events-types-config",
            str(events_types_path),
            "--out-header-dir",
            str(out_event_handlers_include_dir),
            "--out-source-dir",
            str(out_event_handlers_source_dir),
        ]
        code = run_command(event_handlers_cmd)
        if code != 0:
            return code

    print("All selected generators completed successfully")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
