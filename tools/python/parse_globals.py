# pyright: reportUnknownMemberType=false

import re
import argparse

from typing import cast
from pathlib import Path
from elftools.elf.elffile import ELFFile
from elftools.elf.sections import SymbolTableSection


"""
matches variable declaration:
/* SECTION ADDRESS */ VAR_TYPE NAME[NUMEL];
"""
re_glob = re.compile(
    r"^/\* (?P<section>.*?) (?P<address>.*?) \*/ (?P<var_type>[^(]*) (?P<name>.*?)(?:\[(?P<numel>.*?)\])?;"
)

"""
matches structs (or unions) with no typedef:
struct NAME { // SIZE
    ...
};
"""
re_struct = re.compile(
    r"^(?:struct|union) (?P<name>.*?) \{ // (?P<size>0x[0-9a-f]+)\n.*?^\};", flags=re.MULTILINE | re.DOTALL
)

"""
matches structs (or unions) with typedef:
typedef struct { // SIZE
    ...
} NAME;
"""
re_typedef_struct = re.compile(
    r"^typedef (?:struct|union) \{ // (?P<size>0x[0-9a-f]+)\n.*?^\} (?P<name>.*?);", flags=re.MULTILINE | re.DOTALL
)

sizes: dict[str, int] = {
    "char": 1,
    "u_char": 1,
    "short": 2,
    "short int": 2,
    "u_short": 2,
    "int": 4,
    "u_int": 4,
    "float": 4,
    "u_long128": 8,
    "sceVu0FMATRIX": 4 * 4 * 4,
    "sceVu0FVECTOR": 4 * 4,
    "sceSifClientData": 0x2C,
}

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


class GlobalVarLineMatch:
    max_section_len: int = 0
    max_address_len: int = 0
    max_var_type_len: int = 0
    max_name_len: int = 0
    max_numel_len: int = 0

    _name_cache: list[str] = []

    @classmethod
    def _get_unique_name(cls, name: str):
        i = 1
        unique_name = f"{name}__local_{i}"
        while unique_name in cls._name_cache:
            i += 1
            unique_name = f"{name}__local_{i}"
        cls._name_cache.append(unique_name)
        return unique_name

    def __init__(
        self,
        symtab: SymbolTableSection,
        /,
        *,
        section: str,
        address: str | int,
        var_type: str,
        name: str,
        numel: str | int | None = None,
    ):
        self.symtab: SymbolTableSection = symtab

        self.section: str = section
        self.address: int = address if isinstance(address, int) else int(address, 16)
        self.var_type: str = var_type
        self.name: str = name
        self.numel: int

        while self.name.startswith("*"):
            self.name = self.name[1:]
            self.var_type += "*"

        # resolve function pointers (e.g., 'void (*SpecialEventInitTbl[0])...')
        if match := re.match(r"\(\*(.*?)\[.*?\]\).*", self.name):
            self.name = match.group(1)

        self.name = self.name.replace("*", "")

        if numel is not None:
            if isinstance(numel, int):
                self.numel = numel
            else:
                prod_numel = 1
                numels = numel.split("][")
                for n in numels:
                    prod_numel *= int(n)
                self.numel = prod_numel

        if numel is None:
            self.numel = 1

        # get symbol
        symbol = self.symtab.get_symbol_by_name(self.name)

        assert isinstance(symbol, list)

        symbol = next(
            (sym for sym in symbol if sym.name == self.name and sym.entry["st_value"] == self.address),
            None,
        )

        assert symbol

        self.symbol = symbol

        if not self.is_global:
            self.name = self._get_unique_name(self.name)

        escaped_name_len = len(str(self.name))
        if self.name in command_script_keywords:
            escaped_name_len += 2

        GlobalVarLineMatch.max_section_len = max(GlobalVarLineMatch.max_section_len, len(str(self.section)))
        GlobalVarLineMatch.max_address_len = max(GlobalVarLineMatch.max_address_len, len(str(self.address)))
        GlobalVarLineMatch.max_var_type_len = max(GlobalVarLineMatch.max_var_type_len, len(str(self.var_type)))
        GlobalVarLineMatch.max_name_len = max(GlobalVarLineMatch.max_name_len, escaped_name_len)
        GlobalVarLineMatch.max_numel_len = max(GlobalVarLineMatch.max_numel_len, len(str(self.numel)))

    @property
    def is_global(self):
        return cast(str, self.symbol["st_info"]["bind"]) == "STB_GLOBAL"

    @property
    def is_hidden(self):
        return cast(str, self.symbol["st_other"]["visibility"]) == "STV_HIDDEN"

    @property
    def size(self):
        size = cast(int, self.symbol["st_size"])
        if size == 0:
            if "*" in self.var_type:
                size = 4
            elif self.var_type in sizes:
                size = sizes[self.var_type] * self.numel
        return size

    def to_string(self, as_linker_command_file: bool):
        name = self.name if self.name not in command_script_keywords else f'"{self.name}"'
        cls_str = f"{name:{GlobalVarLineMatch.max_name_len}s} = 0x{self.address:08x};"

        if not as_linker_command_file:
            if self.size > 0:
                cls_str = f"{cls_str} // size:0x{self.size:x}"
            else:
                cls_str = f"{cls_str} //0 {self.var_type} * {self.numel}"

            # cls_str += f" bind:{'global' if self.is_global else 'local'}"
            # cls_str += f" visibility:{'hidden' if self.is_hidden else 'visible'}"

        return cls_str

    def __str__(self):
        return self.to_string(as_linker_command_file=False)


def parse_globals(elf_path: Path, globals_path: Path, types_path: Path, as_linker_command_file: bool):
    with open(elf_path, mode="rb") as fh:
        elf = ELFFile(fh)

        # Find the symbol table.
        symtab = elf.get_section_by_name(".symtab")
        assert isinstance(symtab, SymbolTableSection)

        with open(types_path, mode="r") as f:
            types_data = f.read()

            for struct_type, size_hex_str in re_struct.findall(types_data):
                struct_type = cast(str, struct_type)
                struct_size = int(cast(str, size_hex_str), 16)
                # assert not (struct_type in sizes and sizes[struct_type] != struct_size)
                sizes[struct_type] = struct_size

            for size_hex_str, struct_type in re_typedef_struct.findall(types_data):
                struct_type = cast(str, struct_type)
                struct_size = int(cast(str, size_hex_str), 16)
                # assert not (struct_type in sizes and sizes[struct_type] != struct_size), (
                #     struct_type,
                #     sizes[struct_type],
                #     struct_size,
                # )
                sizes[struct_type] = struct_size

            if "tagSE_WRK" in sizes:
                sizes["SE_WRK"] = sizes["tagSE_WRK"]

        with open(globals_path, mode="r") as f:
            lines = [line.strip() for line in f.readlines() if line.startswith("/*") and line.strip().endswith(";")]

        matches = [GlobalVarLineMatch(symtab, **match.groupdict()) for line in lines if (match := re_glob.match(line))]
        matches.sort(key=lambda match: match.address)

        for match in matches:
            print(match.to_string(as_linker_command_file=as_linker_command_file))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--language", required=True, choices=["us", "eu"])
    parser.add_argument("--as-linker-command-file", action="store_true")

    args = parser.parse_args()

    if args.language == "us":
        elf_path = Path("config/us/SLUS_203.88")
        globals_path = Path("ccc/SLUS_203.88/globals.h")
        types_path = Path("ccc/SLUS_203.88/types.h")
    else:
        elf_path = Path("config/eu/SLES_508.21")
        globals_path = Path("ccc/SLES_508.21/globals.h")
        types_path = Path("ccc/SLES_508.21/types.h")

    parse_globals(elf_path, globals_path, types_path, args.as_linker_command_file)


if __name__ == "__main__":
    main()
