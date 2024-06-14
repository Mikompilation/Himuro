import re
import glob
import tqdm
import argparse
import functools

from typing import Protocol, cast
from pathlib import Path


@functools.lru_cache(maxsize=1024)  # pyright: ignore[reportUntypedFunctionDecorator]
def get_symbol_address(symbol_addrs: Path, symbol: str):
    if match := re.match(r"^ *([^+ ]+) *\+ *(0x[0-9a-fA-F]+)$", symbol):
        symbol = match.group(1)
        offset = int(match.group(2), 16)
    else:
        offset = 0

    with open(symbol_addrs, mode="r") as fh:
        for line in fh:
            if line.startswith(f"{symbol} "):
                addr = int(line.split("=")[1].split(";")[0].strip(), 16)
                return addr + offset

    assert False, f"{symbol} not found"


def fix_gp(asm_path: Path, gp_value: int, symbol_addrs: Path):
    if gp_value <= 0:
        return

    asm_files = glob.glob(str(asm_path / "**/*.s"), recursive=True)
    for asm_file in tqdm.tqdm(asm_files, desc="Fixing gp_rel"):
        lines: list[str] = []
        with open(asm_file, mode="r") as fh:
            for line in fh:
                if match := re.match(r"^(.*)%gp_rel\(([^)]+)\)(.*)$", line):
                    # ol = line
                    instr_pre = match.group(1)
                    instr_post = match.group(3)
                    address_str = match.group(2)
                    if address_str.startswith("D_"):
                        address_str = address_str.replace("D_", "0x")
                        res = eval(address_str)
                        address = res
                    else:
                        address = get_symbol_address(symbol_addrs, address_str)
                    gp_rel = address - gp_value
                    line = f"{instr_pre}{hex(gp_rel)}{instr_post}\n"
                lines.append(line)
        with open(asm_file, mode="w") as wh:
            wh.writelines(lines)


def main():
    class ArgsProtocol(Protocol):
        asm_path: Path
        gp_value: int
        symbol_addrs: Path

    def hex_int(x: str):
        return int(x, 16)

    parser = argparse.ArgumentParser(description="fixes asm removing gp_rel macro")
    parser.add_argument("asm_path", metavar="asm-path", type=Path, help="assembly root path to patch")
    parser.add_argument("gp_value", metavar="gp", type=hex_int, help="gp value in hex")
    parser.add_argument("symbol_addrs", metavar="symbol-addrs", type=Path, help="path of symbol_addrs.txt")

    args = cast(ArgsProtocol, parser.parse_args())

    fix_gp(args.asm_path, args.gp_value, args.symbol_addrs)


if __name__ == "__main__":
    main()
