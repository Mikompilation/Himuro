import re
import glob
import tqdm
import argparse

from typing import Protocol, cast
from pathlib import Path


def fix_assets(asm_data_path: Path, asset_rel_path: Path):
    asm_files = glob.glob(str(asm_data_path / "**/*.s"), recursive=True)
    for asm_file in tqdm.tqdm(asm_files, desc="Fixing data asm"):
        n: int = 0
        with open(asm_file, mode="r") as fh:
            data_asm: str = fh.read()
            data_asm, n = re.subn(rf'\.incbin "{asset_rel_path}/', '.incbin "assets/', data_asm)

        if n > 0:
            with open(asm_file, mode="w") as wh:
                wh.write(data_asm)


def main():
    class ArgsProtocol(Protocol):
        asm_data_path: Path
        asset_rel_path: Path

    parser = argparse.ArgumentParser(description="fixes data asm include path")
    parser.add_argument("asm_path", metavar="asm-path", type=Path, help="data path in assembly root to patch")
    parser.add_argument("asset_rel_path", metavar="asset-path", type=Path, help="relative asset path")

    args = cast(ArgsProtocol, parser.parse_args())

    fix_assets(args.asm_data_path, args.asset_rel_path)


if __name__ == "__main__":
    main()
