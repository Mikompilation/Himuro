# pyright: reportUnknownMemberType=false

from __future__ import annotations

import re
import json
import argparse
import itertools

from typing import Protocol, TextIO, cast
from pathlib import Path
from elftools.elf.elffile import ELFFile
from elftools.elf.sections import SymbolTableSection, Symbol

import ccc_json_v7

Range = tuple[int, int]

command_script_keywords = (
    "VERSION",
    "SECTIONS",
    "ABSOLUTE",
    "LOADADDR",
    "ALIGN",
    "DEFINED",
    "NEXT",
    "SIZEOF",
    "SIZEOF_HEADERS",
    "MAX",
    "MIN",
    "PHDRS",
    "CREATE_OBJECT_SYMBOLS",
    "BYTE",
    "SHORT",
    "LONG",
    "SQUAD",
    "FILL",
    "BLOCK",
    "NOLOAD",
    "AT",
    "OVERLAY",
    "NOCROSSREFS",
    "PT_NULL",
    "PT_LOAD",
    "PT_DYNAMIC",
    "PT_INTERP",
    "PT_NOTE",
    "PT_SHLIB",
    "PT_PHDR",
    "ENTRY",
    "FLOAT",
    "NOFLOAT",
    "FORCE_COMMON_ALLOCATION",
    "INCLUDE",
    "INPUT",
    "GROUP",
    "OUTPUT",
    "OUTPUT_ARCH",
    "OUTPUT_FORMAT",
    "SEARCH_DIR",
    "STARTUP",
    "TARGET",
    "NOCROSSREFS",
)

skip_symbols = (
    "_fbss",
    "_gp",
)

size_exceptions = {
    "dorcon": 0x1C,
}


def parse_types(ccc_model_v7: ccc_json_v7.CCCJSONv7Model):
    type_map: dict[str, int] = {}

    for n, dt in enumerate(ccc_model_v7.deduplicated_types):
        if dt.descriptor == "builtin":
            assert dt.name and dt.name not in type_map
            assert dt.class_ is not None
            size_bits = int(dt.class_.split("-", maxsplit=1)[0])
            assert size_bits % 8 == 0
            type_map[dt.name] = size_bits // 8

        elif dt.descriptor == "type_name":
            assert dt.name is not None
            assert dt.type_name is not None
            if dt.name in type_map:
                if dt.size_bits:
                    assert type_map[dt.name] == dt.size_bits // 8
            if dt.name == dt.type_name == "void":
                type_map["void"] = type_map["int"]
                continue
            assert dt.type_name in type_map, (n, dt.type_name)
            type_map[dt.name] = type_map[dt.type_name]

        elif dt.descriptor == "pointer":
            assert dt.name is not None
            assert dt.value_type is not None
            if dt.value_type.descriptor == "function_type":
                assert dt.name not in type_map
                type_map[dt.name] = type_map["int"]
            elif dt.value_type.descriptor == "type_name":
                assert dt.value_type.type_name
                assert dt.value_type.type_name in type_map
                type_map[dt.name] = type_map[dt.value_type.type_name]

        elif dt.descriptor == "struct":
            assert dt.name is not None
            if not dt.conflict:
                assert dt.name not in type_map, n
            else:
                if dt.name in type_map:
                    continue
            assert dt.size_bits is not None
            assert dt.size_bits % 8 == 0
            type_map[dt.name] = dt.size_bits // 8

        elif dt.descriptor == "array":
            assert dt.name is not None
            element_count = 1
            element_type = dt.element_type
            type_name = None
            while element_type:
                if element_type.element_count is not None:
                    element_count *= element_type.element_count
                if element_type.type_name is not None:
                    type_name = element_type.type_name
                    assert type_name in type_map
                    element_count *= type_map[type_name]
                element_type = element_type.element_type
            assert type_name and type_name in type_map, n
            type_map[dt.name] = type_map[type_name] * element_count

        elif dt.descriptor == "enum":
            if dt.name:
                if not dt.conflict:
                    assert dt.name not in type_map, n
                type_map[dt.name] = type_map["int"]

        elif dt.descriptor == "union":
            assert dt.name
            assert dt.size_bits
            assert dt.size_bits % 8 == 0
            type_map[dt.name] = dt.size_bits // 8

        else:
            assert False, f"unknown {n}"

    return type_map


def in_range(ranges: list[Range], address: int):
    if not ranges:
        return True

    in_range = any((ra[0] <= address <= ra[1]) for ra in ranges)

    return in_range


def parse_ccc_model_v7(ccc_model_v7: ccc_json_v7.CCCJSONv7Model, ranges: list[Range]):
    type_sizes = parse_types(ccc_model_v7)

    if "pointer" not in type_sizes:
        type_sizes["pointer"] = type_sizes["int"]

    static_locals: list[ccc_json_v7.Local] = []
    global_vars: list[ccc_json_v7.Global] = []

    for file in ccc_model_v7.files:
        for global_var in file.globals:
            assert global_var.storage.global_address
            if in_range(ranges, global_var.storage.global_address):
                global_vars.append(global_var)

        for function in file.functions:
            for local in function.locals:
                if local.storage_class == "static":
                    assert local.storage.global_address
                    if in_range(ranges, local.storage.global_address):
                        static_locals.append(local)

    return type_sizes, global_vars, static_locals


class SymbolWithNoNameException(Exception): ...


class ParsedSymbol:
    _name_map: dict[str, list[ParsedSymbol]] = {}
    _max_name_len: int = 0

    def __init__(self, symtab: SymbolTableSection, symbol: Symbol) -> None:
        self.symtab = symtab
        self.symbol = symbol

        if not self.name:
            raise SymbolWithNoNameException

        if self.name not in ParsedSymbol._name_map:
            ParsedSymbol._name_map[self.name] = []

        ParsedSymbol._name_map[self.name].append(self)
        ParsedSymbol._max_name_len = max(ParsedSymbol._max_name_len, len(self.name))

    @property
    def name(self) -> str:
        return re.sub(r"^(.*?)(\.\d+)?$", r"\1", self.symbol.name)

    @property
    def size(self) -> int:
        return cast(int, self.symbol["st_size"])

    @property
    def address(self) -> int:
        return cast(int, self.symbol.entry["st_value"])

    def in_range(self, range: Range):
        return range[0] <= self.address <= range[1]

    def to_undefined_syms(self) -> str:
        homonyms = ParsedSymbol._name_map[self.name]
        assert len(homonyms) > 0
        if len(homonyms) == 1:
            assert homonyms[0] == self
            name = self.name
        else:
            idx = homonyms.index(self)
            name = f"{self.name}__local_{idx + 1}"

        name = name if name not in command_script_keywords else f'"{name}"'

        # name_len_fmt = ParsedSymbol._max_name_len + len("__local_9999")
        name_len_fmt = 40

        return f"{name:{name_len_fmt}s} = 0x{self.address:08x};"

    def to_symbol_addrs(
        self,
        type_sizes: dict[str, int],
        global_vars: list[ccc_json_v7.Global],
        static_locals: list[ccc_json_v7.Local],
    ) -> str:
        homonyms = ParsedSymbol._name_map[self.name]
        assert len(homonyms) > 0
        if len(homonyms) == 1:
            assert homonyms[0] == self
            name = self.name
        else:
            idx = homonyms.index(self)
            name = f"{self.name}__local_{idx + 1}"

        # name_len_fmt = ParsedSymbol._max_name_len + len("__local_9999")
        name_len_fmt = 40

        size = size_exceptions.get(self.name, self.size)
        if size == 0:
            var = next(
                (global_ for global_ in global_vars if global_.storage.global_address == self.address), None
            ) or next((local for local in static_locals if local.storage.global_address == self.address), None)

            if not var:
                print(f"cannot find {self.name} with size 0 in json")
            else:
                element_count, type_name = var.parsed_size
                type_size = type_sizes.get(type_name)
                assert type_size is not None, type_name
                size = element_count * type_size

                if not size:
                    print(f"size of {self.name} is also 0 using json")

        size_str = f"size:0x{size:x}" if size else ""

        return f"{name:{name_len_fmt}s} = 0x{self.address:08x}; // {size_str}"


def parse_symbols_safe(elf_path: Path, dest_path: Path, json_path: Path, ranges: list[Range]):
    if not dest_path.is_dir():
        raise RuntimeError(f"{dest_path} is not a directory")

    symbol_addrs_path = dest_path / "symbols_addrs.txt"
    undefined_syms_path = dest_path / "undefined_syms.txt"

    if symbol_addrs_path.exists() or undefined_syms_path.exists():
        raise RuntimeError("symbols_addrs.txt or undefined_syms.txt already exist in dest folder")

    with open(elf_path, mode="rb") as elf_fh, open(json_path, mode="r") as json_fh:
        elf = ELFFile(elf_fh)

        json_data = json.load(json_fh)
        ccc_model_v7 = ccc_json_v7.CCCJSONv7Model.model_validate(json_data)
        type_sizes, global_vars, static_locals = parse_ccc_model_v7(ccc_model_v7, ranges)

        with open(symbol_addrs_path, mode="w") as symbol_addrs, open(undefined_syms_path, mode="w") as undefined_syms:
            parse_symbols(elf, symbol_addrs, undefined_syms, ranges, type_sizes, global_vars, static_locals)


def parse_symbols(
    elf: ELFFile,
    symbol_addrs: TextIO,
    undefined_syms: TextIO,
    ranges: list[Range],
    type_sizes: dict[str, int],
    global_vars: list[ccc_json_v7.Global],
    static_locals: list[ccc_json_v7.Local],
):
    # find symbol table
    symtab = elf.get_section_by_name(".symtab")
    assert isinstance(symtab, SymbolTableSection)

    parsed_symbols: list[ParsedSymbol] = []

    for symbol in symtab.iter_symbols():
        try:
            parsed_symbol = ParsedSymbol(symtab, symbol)
            if parsed_symbol.name in skip_symbols:
                continue
            parsed_symbols.append(parsed_symbol)
        except SymbolWithNoNameException:
            pass

    parsed_symbols.sort(key=lambda x: x.address)

    for parsed_symbol in parsed_symbols:
        if ranges:
            in_range = any(parsed_symbol.in_range(ra) for ra in ranges)
            if not in_range:
                continue

        parsed_symbol.address
        symbol_addrs.write(parsed_symbol.to_symbol_addrs(type_sizes, global_vars, static_locals))
        symbol_addrs.write("\n")

        undefined_syms.write(parsed_symbol.to_undefined_syms())
        undefined_syms.write("\n")


def main():
    class ArgsProtocol(Protocol):
        elf_path: Path
        dest_path: Path
        json_path: Path
        ranges: list[Range]

    def range_type(arg: str) -> Range:
        if m := re.match(r"^([0-9a-f]+)-([0-9a-f]+)$", arg):
            range_start, range_end = int(m.group(1), 16), int(m.group(2), 16)
            if not (range_end > range_start):
                raise argparse.ArgumentTypeError("non monotonic range")
            return range_start, range_end
        raise argparse.ArgumentTypeError("invalid range")

    parser = argparse.ArgumentParser()
    parser.add_argument("elf_path", metavar="ELF", type=Path, help="path to ELF file")
    parser.add_argument("--json", dest="json_path", type=Path, help="path to CCC json (v7)")
    parser.add_argument(
        "--dest",
        dest="dest_path",
        type=Path,
        required=True,
        help="folder where symbol_addrs.txt and undefined_syms.txt will be created "
        "(existing files will not be overwritten)",
    )
    parser.add_argument("--range", action="append", dest="ranges", nargs="+", type=range_type)

    args = parser.parse_args()
    args.ranges = list(itertools.chain(*args.ranges))
    args = cast(ArgsProtocol, args)

    parse_symbols_safe(args.elf_path, args.dest_path, args.json_path, args.ranges)


if __name__ == "__main__":
    main()
