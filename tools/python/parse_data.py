# pyright: reportInvalidTypeForm=false

import os
import re
import io
import enum
import math
import argparse
import pydantic
import subprocess

from ctypes import c_uint16, c_int16, c_uint64, c_uint32, c_int32, c_uint8, c_float, sizeof as c_sizeof
from typing import Protocol, BinaryIO, Type, Iterable, cast
from pathlib import Path

from cstruct import CStructure, ctypes_types, CTypeType, c_addr_ptr


def vram2offset(vram: int):
    return vram - 0xFF000


sceVu0FVECTOR = c_float * 4
sceVu0IVECTOR = c_int32 * 4


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


# typedef struct { // 0x40
# 	/* 0x00 */ sceVu0FVECTOR fall_speed;
# 	/* 0x10 */ sceVu0IVECTOR wind;
# 	/* 0x20 */ sceVu0FVECTOR rotate;
# 	/* 0x30 */ int stop_time;
# 	/* 0x34 */ short int r;
# 	/* 0x36 */ short int g;
# 	/* 0x38 */ short int b;
# 	/* 0x3a */ short int a;
# } FALL_TABLE;
class FALL_TABLE(CStructure, align=64):
    fall_speed: sceVu0FVECTOR
    wind: sceVu0IVECTOR
    rotate: sceVu0FVECTOR
    stop_time: c_int32
    r: c_int16
    g: c_int16
    b: c_int16
    a: c_int16


# typedef struct { // 0xe
# 	/* 0x0 */ short int u;
# 	/* 0x2 */ short int v;
# 	/* 0x4 */ short int w;
# 	/* 0x6 */ short int h;
# 	/* 0x8 */ short int x;
# 	/* 0xa */ short int y;
# 	/* 0xc */ u_char pri;
# 	/* 0xd */ u_char alp;
# } SPRT_SDAT;
class SPRT_SDAT(CStructure):
    u: c_int16
    v: c_int16
    w: c_int16
    h: c_int16
    x: c_int16
    y: c_int16
    pri: c_uint8
    alp: c_uint8


# typedef struct { // 0x8
# 	/* 0x0 */ short int cx;
# 	/* 0x2 */ short int cy;
# 	/* 0x4 */ float rot;
# } SPRT_SROT;
class SPRT_SROT(CStructure):
    cx: c_int16
    cy: c_int16
    rot: c_float


# typedef struct { // 0x8
# 	/* 0x0 */ short int cx;
# 	/* 0x2 */ short int cy;
# 	/* 0x4 */ short int sw;
# 	/* 0x6 */ short int sh;
# } SPRT_SSCL;
class SPRT_SSCL(CStructure):
    cx: c_int16
    cy: c_int16
    sw: c_int16
    sh: c_int16


# typedef struct { // 0x14
#     /* 0x00 */ short int x_trans;
#     /* 0x02 */ short int y_trans;
#     /* 0x04 */ short int x_scale;
#     /* 0x06 */ short int y_scale;
#     /* 0x08 */ short int rotation;
#     /* 0x0a */ short int alpha;
#     /* 0x0c */ short int counter;
#     /* 0x10 */ u_int attribute;
# } ANM2D_DAT_TABLE;
class ANM2D_DAT_TABLE(CStructure):
    x_trans: c_int16
    y_trans: c_int16
    x_scale: c_int16
    y_scale: c_int16
    rotation: c_int16
    alpha: c_int16
    counter: c_int16
    attribute: c_uint32


# typedef struct { // 0x110
#     /* 0x000 */ int anm_index;
#     /* 0x004 */ SPRT_SDAT *sdat_p;
#     /* 0x008 */ SPRT_SDAT sdat[8];
#     /* 0x078 */ SPRT_SROT srot[8];
#     /* 0x0b8 */ SPRT_SSCL sscl[8];
#     /* 0x0f8 */ int sp_no;
#     /* 0x0fc */ void *table_p;
#     /* 0x100 */ short int x_speed;
#     /* 0x102 */ short int y_speed;
#     /* 0x104 */ short int xscal_off;
#     /* 0x106 */ short int yscal_off;
#     /* 0x108 */ short int rot_off;
#     /* 0x10a */ short int alpha_off;
#     /* 0x10c */ short int counter;
#     /* 0x10e */ short int pad;
# } ANM2D_WRK_TABLE;
class ANM2D_WRK_TABLE(CStructure):
    anm_index: c_int32
    sdat_p: c_addr_ptr  # SPRT_SDAT
    sdat: SPRT_SDAT * 8
    srot: SPRT_SROT * 8
    sscl: SPRT_SSCL * 8
    sp_no: c_int32
    table_p: c_addr_ptr
    x_speed: c_int16
    y_speed: c_int16
    xscal_off: c_int16
    yscal_off: c_int16
    rot_off: c_int16
    alpha_off: c_int16
    counter: c_int16
    pad: c_int16


# typedef struct { // 0x17b0
# 	/* 0x0000 */ sceVu0FVECTOR mpos_keep;
# 	/* 0x0010 */ void *pos_p;
# 	/* 0x0014 */ short int at_ground[600];
# 	/* 0x04c4 */ short int rgba[600][4];
# 	/* 0x1784 */ int fnum_keep;
# 	/* 0x1788 */ int fall_count;
# 	/* 0x178c */ float fall_rate;
# 	/* 0x1790 */ float rate_remain;
# 	/* 0x1794 */ short int mode_keep;
# 	/* 0x1796 */ short int area_keep;
# 	/* 0x1798 */ short int dist;
# 	/* 0x179a */ short int init_flg;
# 	/* 0x179c */ short int offs[3];
# } LEAVES_TABLE;
class LEAVES_TABLE(CStructure):
    mpos_keep: sceVu0FVECTOR
    pos_p: c_addr_ptr
    at_ground: c_int16 * 600
    rgba: c_int16 * 4 * 600
    fnum_keep: c_int32
    fall_count: c_int32
    fall_rate: c_float
    rate_remain: c_float
    mode_keep: c_int16
    area_keep: c_int16
    dist: c_int16
    init_flg: c_int16
    offs: c_int16 * 3


# typedef struct { // 0x10
# 	/* 0x0 */ u_long tex_keep;
# 	/* 0x8 */ short int tm2_id;
# 	/* 0xa */ short int mode;
# 	/* 0xc */ short int load_id;
# 	/* 0xe */ short int pad;
# } BTL_ANM_LOAD;
class BTL_ANM_LOAD(CStructure):
    tex_keep: c_uint64
    tm2_id: c_int16
    mode: c_int16
    load_id: c_int16
    pad: c_int16


elf_names: dict[str, str] = {
    "us": "SLUS_203.88",
    "eu": "SLES_508.21",
}

# matches "var_name = 0x12345678; // attr1:val1 attr2:val2 ... attrN:valN"
line_pattern = re.compile(r"^([^\s=]+)\s*=\s*(0x[0-9a-fA-F]+);\s*\/\/((?:\s*[0-9a-zA-Z_]+(:?:[0-9a-zA-Z_]+)?)+)\s*$")
# matches multiple attributes in the form of attr:val or attr
attr_pattern = re.compile(r"([0-9a-zA-Z_]+)(?::([0-9a-zA-Z_]+))?")
# match address lines: "0xaabbccdd = &var"
addr_pattern = re.compile(r"^\s*(0x[0-9a-fA-F]+)\s*=\s*(.*)\s*;\s*(\/\/.*)?$")


class DataVar(pydantic.BaseModel):
    model_config = pydantic.ConfigDict(extra="forbid")

    address: int
    name: str
    type: Type[CStructure] | CTypeType
    numel: int | list[int] = 0
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

    def data_var_dumps(self, elf: BinaryIO, addresses: dict[int, str]):
        if isinstance(self.numel, list):
            numel = math.prod(self.numel)
        else:
            numel = max(1, self.numel)
        if issubclass(self.type, CStructure):
            return self.type.dumps(
                self.name,
                elf.read(numel * self.type.sizeof()),
                numel=self.numel,
                static=self.static,
                nosize=self.nosize,
                noarray=self.numel == 0,
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
    addr_vals: dict[int, str] = {}

    with open(data_vars_txt, mode="r") as fh:
        for n, line in enumerate(fh):
            line = line.strip()

            # skip empty lines and comments
            if not line or line.startswith("//"):
                continue

            valid = False

            if addr_match := addr_pattern.match(line):
                address = int(addr_match.group(1), 16)
                value = addr_match.group(2)
                addr_vals[address] = value
                valid = True

            elif line_match := line_pattern.match(line):
                name = line_match.group(1)
                address = int(line_match.group(2), 16)
                options = line_match.group(3)
                if attr_match := attr_pattern.findall(options):
                    attrs: dict[str, str | list[str] | bool] = {}
                    for attr_name, attr_val in cast(list[tuple[str, str]], attr_match):
                        if attr_name not in attrs:
                            attrs[attr_name] = attr_val
                        elif not isinstance(attrs[attr_name], list):
                            attrs[attr_name] = cast(list[str], [attrs[attr_name], attr_val])
                        else:
                            cast(list[str], attrs[attr_name]).append(attr_val)
                    for key, value in attrs.items():
                        if not isinstance(value, bool) and len(value) == 0:
                            attrs[key] = True
                    try:
                        data_var = DataVar(address=address, name=name, **attrs)  # pyright: ignore[reportArgumentType]
                        data_vars.append(data_var)
                        valid = True
                    except pydantic.ValidationError as ve:
                        print(f"syntax error at line {n}")
                        raise ve

            if strict and not valid:
                raise ValueError(f"syntax error at line {n}: {line}")

    return data_vars, addr_vals


def parse_data(lang: str):
    data_vars_txt = Path("config") / lang / "data_vars.txt"

    data_vars, addr_vals = parse_data_vars(data_vars_txt)

    if not data_vars:
        return

    elf_path = Path("config") / lang / elf_names[lang]

    include_path = Path("config") / lang / "include" / "data"
    include_path.mkdir(parents=True, exist_ok=True)

    c_addr_ptr.set_addresses(addr_vals)

    with open(elf_path, mode="rb") as elf:
        for data_var in data_vars:
            elf.seek(vram2offset(data_var.address), os.SEEK_SET)
            header_path = include_path / f"{data_var.name}.h"
            with open(header_path, mode="w") as fw:
                fw.write(data_var.data_var_dumps(elf, addr_vals))

            try:
                subprocess.run(["clang-format", "-i", "--style=Microsoft", header_path])
            except Exception:
                # formatting is best effort for now
                pass


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
