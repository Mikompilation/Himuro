from __future__ import annotations

import io
import ctypes
from typing import Type, TypeVar, Any
from ctypes import LittleEndianStructure, Union, Array, c_uint

CTypeType = (
    type[ctypes.c_int8]
    | type[ctypes.c_uint8]
    | type[ctypes.c_int16]
    | type[ctypes.c_uint16]
    | type[ctypes.c_int32]
    | type[ctypes.c_uint32]
    | type[ctypes.c_float]
    | type[ctypes.c_double]
)

ctypes_types: dict[str, CTypeType] = {
    "char": ctypes.c_int8,
    "u_char": ctypes.c_uint8,
    "short": ctypes.c_int16,
    "u_short": ctypes.c_uint16,
    "int": ctypes.c_int32,
    "u_int": ctypes.c_uint32,
    "float": ctypes.c_float,
    "double": ctypes.c_double,
}


class c_addr(c_uint):
    def __str__(self):
        # if self.value == 0:
        #     return "NULL"

        return f"0x{self.value:08X}"


class LittleEndianStructureFieldsFromTypeHints(type(LittleEndianStructure)):
    def __new__(
        cls: Type[type],
        name: str,
        bases: tuple[type, ...],
        namespace: dict[str, Any],
        /,
        *,
        align: int = 0,
        pack: int = 0,
    ) -> LittleEndianStructureFieldsFromTypeHints:
        annotations = namespace.get("__annotations__", {})
        namespace["_align_"] = align
        namespace["_pack_"] = pack
        namespace["_fields_"] = list(annotations.items())
        return type(LittleEndianStructure).__new__(cls, name, bases, namespace)


class CStructure(LittleEndianStructure, metaclass=LittleEndianStructureFieldsFromTypeHints):
    @classmethod
    def sizeof(cls) -> int:
        align = getattr(cls, "_align_", 0)
        c_size = ctypes.sizeof(cls)
        if align > 0:
            rem = c_size % align
            if rem:
                rem = align - rem
                return c_size + rem

        return c_size

    @classmethod
    def parse(cls, data: bytes):
        cstruct_size = sizeof(cls)
        assert len(data) % cstruct_size == 0, f"{len(data)}, {cstruct_size}"
        cstruct_num = len(data) // cstruct_size
        stream = io.BytesIO(data)
        cstructs = [cls.from_buffer_copy(stream.read(cstruct_size)) for _ in range(cstruct_num)]
        return cstructs

    @classmethod
    def dumps(cls, name: str, data: bytes, static: bool = False, nosize: bool = False):
        cstructs = cls.parse(data)
        stream = io.StringIO()
        if static:
            stream.write("static ")
        numel = f"{len(cstructs)}" if not nosize else ""
        stream.write(f"{cls.__name__} {name}[{numel}] = {{\n")
        for s in cstructs:
            stream.write(f"{s}\n")
        stream.write("};\n\n")
        return stream.getvalue()

    def __str__(self):
        stream = io.StringIO()
        stream.write("    {\n")
        for f, *_ in self._fields_:
            v = getattr(self, f)
            if not isinstance(v, Array):
                stream.write(f"        .{f} = {v},\n")
            else:
                v = str(str([x for x in v])).replace("[", "{").replace("]", "}")
                stream.write(f"        .{f} = {v},\n")
        stream.write("    },")
        return stream.getvalue()


class UnionFieldsFromTypeHints(type(Union)):
    def __new__(
        cls: Type[type],
        name: str,
        bases: tuple[type, ...],
        namespace: dict[str, Any],
        /,
        *,
        align: int = 0,
        pack: int = 0,
    ) -> UnionFieldsFromTypeHints:
        annotations = namespace.get("__annotations__", {})
        namespace["_align_"] = align
        namespace["_pack_"] = pack
        namespace["_fields_"] = list(annotations.items())
        return type(Union).__new__(cls, name, bases, namespace)


class CUnion(Union, metaclass=UnionFieldsFromTypeHints):
    pass


_T = TypeVar("_T", bound=CStructure)


def sizeof(cstruct_type: Type[_T]) -> int:
    align = getattr(cstruct_type, "_align_", 0)
    c_size = ctypes.sizeof(cstruct_type)
    if align > 0:
        rem = c_size % align
        if rem:
            rem = align - rem
            return c_size + rem

    return c_size


def parse_cstruct(cstruct_type: Type[_T], data: bytes) -> list[_T]:
    cstruct_size = sizeof(cstruct_type)
    assert len(data) % cstruct_size == 0, f"{len(data)}, {cstruct_size}"
    cstruct_num = len(data) // cstruct_size
    stream = io.BytesIO(data)
    cstructs = [cstruct_type.from_buffer_copy(stream.read(cstruct_size)) for _ in range(cstruct_num)]
    return cstructs


def print_cstruct(name: str, cstruct_type: Type[CStructure], data: bytes):
    cstructs = parse_cstruct(cstruct_type, data)
    print(f"{cstruct_type.__name__} {name}[{len(cstructs)}] = {{")
    for s in cstructs:
        print(s)
    print("};\n")
