from __future__ import annotations

import os
import argparse
import pydantic

from pathlib import Path
from typing import Protocol, cast


CONFIG_ROOT = Path(__file__).parent.parent.parent.resolve() / "config"

# enumerate languages from config dir
LANGUAGES = [Path(f.path).name for f in os.scandir(CONFIG_ROOT) if f.is_dir()]


class BytePatch(pydantic.RootModel[tuple[int, int]]):
    root: tuple[int, int]

    @property
    def address(self):
        return self.root[0]

    @property
    def size(self):
        return self.root[1]


class BINPatch(pydantic.RootModel[dict[str, BytePatch]]):
    root: dict[str, BytePatch]


class PatchDB(pydantic.RootModel[dict[str, BINPatch]]):
    root: dict[str, BINPatch]


def fix_elf(orig_elf_path: Path, built_elf_path: Path, bin_patch: BytePatch):
    with orig_elf_path.open("rb") as fh:
        fh.seek(bin_patch.address)
        data = fh.read(bin_patch.size)

    with built_elf_path.open(mode="r+b") as wh:
        wh.seek(bin_patch.address)
        wh.write(data)


def main():
    class ArgsProtocol(Protocol):
        language: str
        elf_file: Path

    parser = argparse.ArgumentParser(description="apply asm patches to assembly files")
    parser.add_argument("language", type=str, choices=LANGUAGES, help="language of the elf that is being patched")
    parser.add_argument("elf_file", type=Path, help="elf file to patch (relative to build dir)")

    args = cast(ArgsProtocol, parser.parse_args())

    built_elf_file = CONFIG_ROOT / args.language / args.elf_file

    if not built_elf_file.exists():
        print(f"ERROR: cannot find elf file {built_elf_file}")
        exit(1)

    orig_elf_path = CONFIG_ROOT / args.language / args.elf_file.name

    patch_db_path = CONFIG_ROOT / args.language / "bin_patches.json"

    if not patch_db_path.exists():
        exit(0)

    patch_db = PatchDB.model_validate_json(patch_db_path.read_text())

    for _tu_name, bin_patch in patch_db.root.items():
        for _func_name, byte_patch in bin_patch.root.items():
            fix_elf(orig_elf_path, built_elf_file, byte_patch)


if __name__ == "__main__":
    main()
