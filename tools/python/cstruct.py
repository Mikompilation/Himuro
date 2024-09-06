from __future__ import annotations

import io
import math
import ctypes
import numpy as np
import numpy.typing as npt
from collections.abc import Sized, Sequence
from typing import Type, TypeVar, Any, cast, TextIO
from ctypes import LittleEndianStructure, Union, Array, c_uint32

_G = TypeVar("_G")

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


class c_addr(c_uint32):
    def __str__(self):
        # if self.value == 0:
        #     return "NULL"

        # return f"0x{self.value:08x}"
        return f"0x{self.value:x}"


class c_addr_ptr(c_uint32):
    _addresses: dict[int, str] | None = None

    @classmethod
    def set_addresses(cls, addresses: dict[int, str] | None):
        cls._addresses = addresses

    def __str__(self):
        if self.value == 0:
            return "NULL"

        if self._addresses and self.value in self._addresses:
            return self._addresses[self.value]

        print(f"warning: no address for pointer 0x{self.value:x}")

        return f"0x{self.value:x}"


def print_arr(arr: npt.NDArray[np.int_], lst: list[Any], file: TextIO):
    if arr.ndim > 1:
        for x in arr:
            file.write("{")
            print_arr(x, lst, file)
            file.write("},")
    else:
        file.write(",".join(str(lst[i]) for i in arr))


def print_carr(arr: Sequence[Any], file: TextIO):
    if hasattr(arr[0], "_length_"):
        for a in arr:
            file.write("{")
            print_carr(a, file)
            file.write("},")
    else:
        v = str(str([x for x in arr])).replace("[", "").replace("]", "")
        file.write(v)


def chunks(lst: Sequence[_G], n: int):
    """Yield successive n-sized chunks from lst."""
    for i in range(0, len(lst), n):
        yield lst[i : i + n]


def format_array(lst: Sequence[_G], dims: Sequence[int], file: TextIO):
    if len(dims) > 1:
        for ll in chunks(lst, len(lst) // dims[0]):
            file.write("{")
            format_array(ll, dims[1:], file)
            file.write("},")
    else:
        for n, x in enumerate(lst):
            sep = ", " if n < len(lst) - 1 else ""
            file.write(f"{x}{sep}")


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
    def dumps(
        cls,
        name: str,
        data: bytes,
        numel: int | list[int],
        static: bool = False,
        nosize: bool = False,
        noarray: bool = False,
    ):
        cstructs = cls.parse(data)
        stream = io.StringIO()
        if static:
            stream.write("static ")
        stream.write(f"{cls.__name__} {name}")
        assert not (noarray and isinstance(numel, list))
        if not noarray:
            if isinstance(numel, int):
                assert len(cstructs) == numel, (len(cstructs), numel)
                numel_str = f"{len(cstructs)}" if not nosize else ""
                stream.write(f"[{numel_str}]")
            else:
                numel_str = "".join([f"[{num if n > 0 or not nosize else ''}]" for n, num in enumerate(numel)])
                stream.write(numel_str)
        tot_numel = max(1, numel) if isinstance(numel, int) else math.prod(numel)
        # nmdim = 1 if isinstance(numel, int) else len(numel)
        assert len(cstructs) == tot_numel, (len(cstructs), tot_numel)

        stream.write(" = ")
        if not noarray:
            stream.write("{\n")
        if isinstance(numel, int):
            for n, s in enumerate(cstructs):
                is_last = n == len(cstructs) - 1
                if is_last and noarray:
                    stream.write(f"{s}\n")
                else:
                    stream.write(f"{s},\n")

        else:
            idxs = np.arange(tot_numel).reshape(numel)
            print_arr(idxs, cstructs, stream)
        if not noarray:
            stream.write("}")
        stream.write(";\n\n")
        return stream.getvalue()

    def __str__(self):
        stream = io.StringIO()
        stream.write("    {\n")
        for f, *_ in self._fields_:
            v = getattr(self, f)
            if not isinstance(v, Array):
                stream.write(f"        .{f} = {v},\n")
            else:
                arr = cast(Sized, v)
                if len(arr) and isinstance(v[0], CStructure):
                    arr = cast(list[CStructure], arr)
                    stream.write(f"        .{f} = {{\n")
                    for elem in arr:
                        stream.write(f"        {elem},\n")
                    stream.write("        },\n")
                else:
                    arr = cast(Sequence[Any], arr)
                    if isinstance(arr[0], Array):
                        # multidimensional ctypes array
                        stream.write(f"        .{f} = {{")
                        print_carr(arr, stream)
                        stream.write("},\n")
                    else:
                        stream.write(f"        .{f} = {{")
                        dims = [len(arr)]
                        format_array(arr, dims, stream)

                        stream.write("},\n")
        stream.write("    }")
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
