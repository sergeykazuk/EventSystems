#!/usr/bin/env python3
from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Run all StaticEventSystem generators")
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[1], help="StaticEventSystem root")
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

    events_types_path = (root / "configs" / "events_types.json").resolve()
    events_enum_path = (root / "configs" / "events_enum.json").resolve()
    handlers_path = (root / "configs" / "event_handlers.json").resolve()

    senders_template_path = (root / "templates" / "EventSenders.hpp.tmpl").resolve()
    senders_cpp_template_path = (root / "templates" / "EventSenders.cpp.tmpl").resolve()
    payload_helpers_template_path = (root / "templates" / "EventPayloadHelpers.hpp.tmpl").resolve()
    payload_helpers_cpp_template_path = (root / "templates" / "EventPayloadHelpers.cpp.tmpl").resolve()
    last_event_getters_template_path = (root / "templates" / "LastEventGetters.hpp.tmpl").resolve()
    last_event_getters_cpp_template_path = (root / "templates" / "LastEventGetters.cpp.tmpl").resolve()
    last_event_helpers_cpp_template_path = (root / "templates" / "LastEventHelpers.ipp.tmpl").resolve()
    out_include_path = (root / "generated" / "include").resolve()
    out_source_path = (root / "generated" / "src").resolve()

    map_template_path = (root / "templates" / "EventHandlersMapBuilder.cpp.tmpl").resolve()
    out_map_cpp_path = (root / "generated" / "src" / "EventHandlersMapBuilder.cpp").resolve()

    event_handlers_header_template_path = (root / "templates" / "EventHandlers.hpp.tmpl").resolve()
    event_handlers_source_template_path = (root / "templates" / "EventHandlers.cpp.tmpl").resolve()
    out_event_handlers_include_dir = (root / "generated" / "include" / "handlers").resolve()
    out_event_handlers_source_dir = (root / "generated" / "src" / "handlers").resolve()

    python_executable = sys.executable

    if not args.skip_static_headers:
        static_headers_cmd = [
            python_executable,
            str(scripts_dir / "generate_static_headers.py"),
            "--root",
            str(root),
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
        ]
        code = run_command(event_handlers_cmd)
        if code != 0:
            return code

    print("All selected generators completed successfully")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
