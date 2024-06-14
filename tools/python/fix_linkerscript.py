import re
import sys
import yaml
import tqdm

from typing import cast, Any
from pathlib import Path


# e.g.: build/src/main/main.c.o(.text);
#       build/asm/data/rodata_4.rodata.s.o(.rodata);
#       ...
# NOTE: also account for optional '/data/' in path as chunks of data
#       not belonging to a c file are put into 'build/asm/data/' folder.
re_subsegment_line = re.compile(
    r"^(?P<indent> +)build/(?:asm|src)(?:/data)?/(?P<name>.*)\.[sc]\.o\(\.(?P<section>.+)\);$"
)

# e.g.: .main 0x100000 : AT(main_ROM_START) SUBALIGN(2)
#       .main_bss (NOLOAD) : SUBALIGN(4)
#       ...
re_section_line = re.compile(r"^(?P<indent> +)\.(?P<section>[^ ]+) .* SUBALIGN\((?P<subalign>[0-9]+)\)$")


def get_align(address: int):
    return (
        128 * ((address % 128) == 0)
        or 64 * ((address % 64) == 0)
        or 32 * ((address % 32) == 0)
        or 16 * ((address % 16) == 0)
        or 8 * ((address % 8) == 0)
        or 4 * ((address % 4) == 0)
        or 2 * ((address % 2) == 0)
        or 1
    )


def make_align_map(config: dict[str, Any]):
    segments = cast(list[dict[str, Any] | list[Any]] | None, config["segments"])
    assert segments
    main_segment = next(
        (segment for segment in segments if isinstance(segment, dict) and segment.get("name") == "main"), None
    )
    assert main_segment, "cannot find main segment"

    subsegments = cast(list[dict[str, Any] | list[Any]] | None, main_segment["subsegments"])
    assert subsegments, "cannot find main subsegments"

    align_map: dict[str, int] = {}

    for subsegment in subsegments:
        if not isinstance(subsegment, dict):
            continue

        s_type = cast(str | None, subsegment.get("type"))
        vram = cast(int | None, subsegment.get("vram"))
        name = cast(str | None, subsegment.get("name"))
        if not s_type or not vram or not name:
            continue

        if name.endswith("bin"):
            continue

        align = get_align(vram)

        if not s_type.startswith("."):
            name = f"{s_type}#{name}.{s_type}"
        else:
            name = f"{s_type[1:]}#{name}"

        align_map[name] = align

    return align_map


def fix_linkerscript(config: dict[str, Any], linkerscript_path: Path):
    align_map = make_align_map(config)

    section_subalign = cast(dict[str, int], config["_section_subalign"])

    line_count = 0
    with open(linkerscript_path, mode="r") as fh:
        for line in fh:
            line_count += 1

    patched_lines: list[str] = []

    with open(linkerscript_path, mode="r") as fh:
        for line in tqdm.tqdm(fh, desc="Fixing linker script", total=line_count):
            if match := re_subsegment_line.match(line):
                indent = cast(str, match["indent"])
                name = cast(str, match["name"])
                section = cast(str, match["section"])

                # force each subsegment in the following sections to have align 8
                if section == "text":
                    patched_lines.append(f"{indent}. = ALIGN(., 8);\n")

                key = f"{section}#{name}"
                if align := align_map.get(key):
                    patched_lines.append(f"{indent}. = ALIGN(., {align});\n")

            if match := re_section_line.match(line):
                indent = cast(str, match["indent"])
                section = cast(str, match["section"])
                subalign = cast(str, match["subalign"])

                # force each section to have the subalign specified in the yaml
                if section in section_subalign:
                    current_subalign = f"SUBALIGN({subalign})"
                    fixed_subalign = f"SUBALIGN({section_subalign[section]})"
                    line = line.replace(current_subalign, fixed_subalign)

            patched_lines.append(line)

    with open(linkerscript_path, mode="w") as fh:
        fh.writelines(patched_lines)


def main():
    if len(sys.argv) != 3:
        print("usage: fix_linkerscript.py CONFIG_YAML_PATH LINKERSCRIPT_PATH")
        exit(1)

    config_path = Path(sys.argv[1])
    linkerscript_path = Path(sys.argv[2])

    with open(config_path, mode="r") as fh:
        try:
            config = cast(dict[str, Any], yaml.safe_load(fh))
        except yaml.YAMLError as e:
            print(e)
            raise e

    fix_linkerscript(config, linkerscript_path)


if __name__ == "__main__":
    main()
