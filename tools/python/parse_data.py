import os
import re
import enum
import argparse
import pydantic

from ctypes import c_uint16, c_uint64, c_int32, c_uint8
from typing import Protocol, Type, cast
from pathlib import Path

from cstruct import CStructure


def vram2offset(vram: int):
    return vram - 0xFF000


# struct SPRT_DAT { // size:0x20
#     /* 0x00 */ u_long tex0;
#     /* 0x08 */ u_short u;
#     /* 0x0a */ u_short v;
#     /* 0x0c */ u_short w;
#     /* 0x0e */ u_short h;
#     /* 0x10 */ int x;
#     /* 0x14 */ int y;
#     /* 0x18 */ int pri;
#     /* 0x1c */ u_char alpha;
# };
class SPRT_DAT(CStructure):
    tex0: c_uint64
    u: c_uint16
    v: c_uint16
    w: c_uint16
    h: c_uint16
    x: c_int32
    y: c_int32
    pri: c_int32
    alpha: c_uint8


# typedef struct { // 0x8
#     /* 0x0 */ int file_no;
#     /* 0x4 */ u_short vol;
# } ADPCM_VOL;
class ADPCM_VOL(CStructure):
    file_no: c_int32
    vol: c_uint16


elf_names: dict[str, str] = {
    "us": "SLUS_203.88",
    "eu": "SLES_508.21",
}

# matches "var_name = 0x12345678; // attr1:val1 attr2:val2 ... attrN:valN"
line_pattern = re.compile(r"^([^\s=]+)\s*=\s*(0x[0-9a-fA-F]+);\s*\/\/((?:\s*[0-9a-zA-Z_]+:[0-9a-zA-Z_]+)+)\s*$")
# matches multiple attributes in the form of attr:val
attr_pattern = re.compile(r"([0-9a-zA-Z_]+):([0-9a-zA-Z_]+)")


class DataVar(pydantic.BaseModel):
    model_config = pydantic.ConfigDict(extra="forbid")

    address: int
    name: str
    type: type[CStructure]
    numel: int = 0

    @pydantic.field_validator("type", mode="before")
    @classmethod
    def type_from_str(cls, v: str) -> Type[CStructure]:
        class_type = globals()[v]
        if issubclass(class_type, CStructure):
            return class_type
        raise ValueError(f"{v} is unknown/not a valid type")


def parse_data_vars(data_vars_txt: Path, strict: bool = True):
    data_vars: list[DataVar] = []

    with open(data_vars_txt, mode="r") as fh:
        for n, line in enumerate(fh):
            line = line.strip()

            # skip empty lines and comments
            if not line or line.startswith("//"):
                continue

            valid = False

            if line_match := line_pattern.match(line):
                name = line_match.group(1)
                address = int(line_match.group(2), 16)
                options = line_match.group(3)
                if attr_match := attr_pattern.findall(options):
                    attrs = dict(attr_match)
                    try:
                        data_var = DataVar(address=address, name=name, **attrs)
                        data_vars.append(data_var)
                        valid = True
                    except pydantic.ValidationError as ve:
                        print(f"syntax error at line {n}")
                        raise ve

            if strict and not valid:
                raise ValueError(f"syntax error at line {n}")

    return data_vars


def parse_data(lang: str):
    data_vars_txt = Path("config") / lang / "data_vars.txt"

    data_vars = parse_data_vars(data_vars_txt)

    if not data_vars:
        return

    elf_path = Path("config") / lang / elf_names[lang]

    include_path = Path("config") / lang / "include" / "data"
    include_path.mkdir(parents=True, exist_ok=True)

    with open(elf_path, mode="rb") as elf:
        for data_var in data_vars:
            elf.seek(vram2offset(data_var.address), os.SEEK_SET)
            numel = max(1, data_var.numel)
            var_str = data_var.type.dumps(data_var.name, elf.read(numel * data_var.type.sizeof()))
            with open(include_path / f"{data_var.name}.h", mode="w") as fw:
                fw.write(var_str)


def main():
    class Language(str, enum.Enum):
        US = "us"
        EU = "eu"

    languages = [lang.value for lang in Language]

    class ArgsProtocol(Protocol):
        language: Language

    parser = argparse.ArgumentParser(description="fixes asm removing gp_rel macro")
    parser.add_argument("language", type=str, choices=languages, help="language to extract data for")

    args = cast(ArgsProtocol, parser.parse_args())

    parse_data(args.language)


if __name__ == "__main__":
    main()
