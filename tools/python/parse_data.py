# pyright: reportInvalidTypeForm=false

import os
import re
import io
import enum
import argparse
import pydantic

from ctypes import c_uint16, c_uint64, c_uint32, c_int32, c_uint8, sizeof as c_sizeof
from typing import Protocol, BinaryIO, Type, Iterable, cast
from pathlib import Path

from cstruct import CStructure, ctypes_types, CTypeType


def vram2offset(vram: int):
    return vram - 0xFF000


# struct SPRT_DAT { // size:0x20
#     u_long tex0;
#     u_short u;
#     u_short v;
#     u_short w;
#     u_short h;
#     int x;
#     int y;
#     int pri;
#     u_char alpha;
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
#     int file_no;
#     u_short vol;
# } ADPCM_VOL;
class ADPCM_VOL(CStructure):
    file_no: c_int32
    vol: c_uint16


# typedef struct { // 0x4
#     u_short se_no0;
#     u_short se_no1;
# } SE_STE;
class SE_STE(CStructure):
    se_no0: c_uint16
    se_no1: c_uint16


# typedef struct { // 0x8
#     int file_no;
#     u_char fg_no;
# } FG_FILE_TBL;
class FG_FILE_TBL(CStructure):
    file_no: c_int32
    fg_no: c_uint8


# typedef struct {
# 	int file_no;
# 	u_short vol;
# } ADPCM_ROOM_PLAY;
class ADPCM_ROOM_PLAY(CStructure):
    file_no: c_int32
    vol: c_uint16


# typedef struct {
# 	u_short rvol;
# 	u_char door_no;
# 	u_char foot_no;
# 	u_char srund_no;
# 	u_char event_no;
# 	u_char adpcm[5];
# } ROOM_SOUND_INFO;
class ROOM_SOUND_INFO(CStructure):
    rvol: c_uint16
    door_no: c_uint8
    foot_no: c_uint8
    srund_no: c_uint8
    event_no: c_uint8
    adpcm: c_uint8 * 5


# typedef struct {
#   u_int fno[3];
# } ROOM_DOOR_SE;
class ROOM_DOOR_SE(CStructure):
    fno: c_uint32 * 3


# typedef struct {
#   u_int fno[5];
# } ROOM_FOOT_SE;
class ROOM_FOOT_SE(CStructure):
    fno: c_uint32 * 5


# typedef struct {
#   u_char num;
#   u_char rate[4];
# } SE_FOOT_RAND_ITA;
class SE_FOOT_RAND_ITA(CStructure):
    num: c_uint8
    rate: c_uint8 * 4


elf_names: dict[str, str] = {
    "us": "SLUS_203.88",
    "eu": "SLES_508.21",
}

# matches "var_name = 0x12345678; // attr1:val1 attr2:val2 ... attrN:valN"
line_pattern = re.compile(r"^([^\s=]+)\s*=\s*(0x[0-9a-fA-F]+);\s*\/\/((?:\s*[0-9a-zA-Z_]+(:?:[0-9a-zA-Z_]+)?)+)\s*$")
# matches multiple attributes in the form of attr:val or attr
attr_pattern = re.compile(r"([0-9a-zA-Z_]+)(?::([0-9a-zA-Z_]+))?")


class DataVar(pydantic.BaseModel):
    model_config = pydantic.ConfigDict(extra="forbid")

    address: int
    name: str
    type: Type[CStructure] | CTypeType
    numel: int = 0
    nosize: bool = False
    static: bool = False

    @pydantic.field_validator("type", mode="before")
    @classmethod
    def type_from_str(cls, v: str) -> Type[CStructure] | CTypeType:
        if v in ctypes_types:
            return ctypes_types[v]
        class_type = globals()[v]
        if issubclass(class_type, CStructure):
            return class_type
        raise ValueError(f"{v} is unknown/not a valid type")

    def data_var_dumps(self, elf: BinaryIO):
        numel = max(1, self.numel)
        if issubclass(self.type, CStructure):
            return self.type.dumps(
                self.name,
                elf.read(numel * self.type.sizeof()),
                static=self.static,
                nosize=self.nosize,
            )
        else:
            var_data = (self.type * numel).from_buffer_copy(elf.read(numel * c_sizeof(self.type)))
            type_str = next(k for k, v in ctypes_types.items() if getattr(v, "_type_") == getattr(self.type, "_type_"))
            stream = io.StringIO()
            if self.static:
                stream.write("static ")
            stream.write(f"{type_str} {self.name}")
            if numel > 1:
                numel = f"{self.numel}" if not self.nosize else ""
                var_str = f"{{ {', '.join(f'{var}' for var in cast(Iterable[CTypeType], var_data))} }}"
                stream.write(f"[{numel}]")
            else:
                var_str = f"{var_data.value}"
            stream.write(f" = {var_str};")
        return stream.getvalue()


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
                    for key, value in attrs.items():
                        if len(value) == 0:
                            attrs[key] = True
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
            with open(include_path / f"{data_var.name}.h", mode="w") as fw:
                fw.write(data_var.data_var_dumps(elf))


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
