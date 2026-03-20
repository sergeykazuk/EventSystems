from __future__ import annotations

import json
from pathlib import Path
from typing import Any, Dict


def load_json(path: Path) -> Dict[str, Any]:
    with path.open("r", encoding="utf-8") as file:
        return json.load(file)


def write(path: Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content.rstrip() + "\n", encoding="utf-8")


def resolve_type(type_name: str, base_aliases: Dict[str, str]) -> str:
    return base_aliases.get(type_name, type_name)


def render_enum(namespace: str, enum_name: str, base_type: str, members: Dict[str, str]) -> str:
    lines = [
        "#pragma once",
        "#include <cstdint>",
        "",
        f"namespace {namespace} {{",
        "",
        f"enum class {enum_name} : {base_type}",
        "{",
    ]
    for key, value in members.items():
        if value != "":
            lines.append(f"    {key} = {value},")
        else:
            lines.append(f"    {key},")
                         
    lines += [
        "};",
        "",
        f"inline constexpr const char* toString(const {enum_name} value)",
        "{",
        "    switch (value)",
        "    {",
    ]
    for key in members.keys():
        lines.append(f"    case {enum_name}::{key}: return \"{key}\";")
    lines += [
        "    default: return \"Unknown\";",
        "    }",
        "}",
        "",
        "}",
    ]
    return "\n".join(lines)


def render_type_header(
    namespace: str,
    type_name: str,
    type_def: Dict[str, Any],
    base_aliases: Dict[str, str],
    user_defined_types: set[str],
) -> str:
    includes = {"#include <cstdint>"}

    kind = type_def.get("type")
    lines = ["#pragma once"]

    if kind == "enum":
        base_type = resolve_type(type_def.get("base_type", "uint16_t"), base_aliases)
        values = type_def.get("values", {})
        body = [
            "",
            f"namespace {namespace} {{",
            "",
            f"enum class {type_name} : {base_type}",
            "{",
        ]
        for key, value in values.items():
            body.append(f"    e{key} = {value},")
        body += [
            "};",
            "",
            f"inline constexpr const char* toString(const {type_name} value)",
            "{",
            "    switch (value)",
            "    {",
        ]
        for key in values.keys():
            body.append(f"    case {type_name}::e{key}: return \"e{key}\";")
        body += [
            "    default: return \"Unknown\";",
            "    }",
            "}",
            "",
            "}",
        ]
        lines.extend(sorted(includes))
        lines.extend(body)
        return "\n".join(lines)

    if kind == "struct":
        fields = type_def.get("values", {})
        for field_type in fields.values():
            resolved = resolve_type(field_type, base_aliases)
            if resolved == "std::string":
                includes.add("#include <string>")
            elif resolved == "size_t":
                includes.add("#include <cstddef>")
            elif resolved in user_defined_types and resolved != type_name:
                includes.add(f'#include "types/{resolved}.hpp"')
        body = [
            "",
            f"namespace {namespace} {{",
            "",
            f"struct {type_name}",
            "{",
        ]
        for field_name, field_type in fields.items():
            resolved = resolve_type(field_type, base_aliases)
            body.append(f"    {resolved} {field_name}{{}};")
        body += [
            "};",
            "",
            "}",
        ]
        lines.extend(sorted(includes))
        lines.extend(body)
        return "\n".join(lines)

    raise ValueError(f"Unsupported type kind for {type_name}: {kind}")
