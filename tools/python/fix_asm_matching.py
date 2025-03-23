import os
import re
import argparse
import pydantic

from pathlib import Path
from typing import Protocol, cast


CONFIG_ROOT = Path(__file__).parent.parent.parent.resolve() / "config"

# enumerate languages from config dir
LANGUAGES = [Path(f.path).name for f in os.scandir(CONFIG_ROOT) if f.is_dir()]

RE_INSTR = re.compile(r"^\t[^.]")


class InstructionPatch(pydantic.RootModel[tuple[int, str, str]]):
    root: tuple[int, str, str]

    @property
    def instr_no(self):
        return self.root[0]

    @property
    def instr_org(self):
        return self.root[1]

    @property
    def instr_patch(self):
        return self.root[2]


class ASMPatch(pydantic.RootModel[dict[str, list[InstructionPatch]]]):
    root: dict[str, list[InstructionPatch]]


class PatchDB(pydantic.RootModel[dict[str, ASMPatch]]):
    root: dict[str, ASMPatch]


def fix_asm(asm_file: Path, asm_patch: ASMPatch):
    lines = asm_file.read_text().splitlines()

    def find_func(func: str):
        for i, line in enumerate(lines):
            if re.match(rf"^{func}:", line):
                return i

        return -1

    def find_line(start: int, num: int):
        offset = 0
        instr_no = -1
        for line in lines[start:]:
            if RE_INSTR.match(line):
                instr_no += 1

            if instr_no == num:
                return start + offset

            offset += 1

        raise RuntimeError("cannot find instruction!")

    for func, patch_lst in asm_patch.root.items():
        n = find_func(func)

        if n == -1:
            print(f"WARNING: cannot find function {func} in {asm_file.name}")
            continue

        for patch in patch_lst:
            line_no = find_line(n, patch.instr_no)
            line_org = lines[line_no]

            line_org_clean = re.sub(r"\s+", " ", line_org).strip()
            instr_org_clean = re.sub(r"\s+", " ", patch.instr_org).strip()
            instr_patch_clean = re.sub(r"\s+", " ", patch.instr_patch).strip()

            if line_org_clean != instr_org_clean:
                print(f"WARNING: wrong line: {asm_file.name}:{func}:{line_no + 1}: {line_org} != {patch.instr_org}")
                continue

            lines[line_no] = f"\t{instr_patch_clean}"

    asm_file.write_text("\n".join(lines))


def main():
    class ArgsProtocol(Protocol):
        language: str
        asm_file: Path

    parser = argparse.ArgumentParser(description="apply asm patches to assembly files")
    parser.add_argument("language", type=str, choices=LANGUAGES, help="language of the asm that is being patched")
    parser.add_argument("asm_file", type=Path, help="generated assembly file to patch (relative to build dir)")

    args = cast(ArgsProtocol, parser.parse_args())

    asm_file = CONFIG_ROOT / args.language / args.asm_file

    if not asm_file.exists():
        print(f"ERROR: cannot find assembly file {asm_file}")
        exit(1)

    patch_db_path = CONFIG_ROOT / args.language / "asm_patches.json"

    if not patch_db_path.exists():
        print("ERROR: patch db file doesn't exist. aborting patching asm")
        exit(1)

    patch_db = PatchDB.model_validate_json(patch_db_path.read_text())

    if asm_file.name not in patch_db.root:
        print(f"ERROR: no patches found for file {asm_file.name}")
        exit(1)

    asm_patch = patch_db.root[asm_file.name]

    fix_asm(asm_file, asm_patch)


if __name__ == "__main__":
    main()
