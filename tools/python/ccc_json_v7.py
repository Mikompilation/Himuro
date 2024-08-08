from __future__ import annotations

import pydantic

from typing import Optional, Literal, Any


class AddressRange(pydantic.BaseModel):
    low: int
    high: int


class ValueType(pydantic.BaseModel):
    descriptor: Optional[str] = None
    source: Optional[str] = None
    type_name: Optional[str] = None
    referenced_file_index: Optional[int] = None
    referenced_stabs_type_number: Optional[int] = None
    value_type: Optional[ValueType] = None
    return_type: Optional[ReturnType] = None
    modifier: Optional[str] = None
    vtable_index: Optional[int] = None
    is_constructor: Optional[bool] = None
    element_type: Optional[ElementType] = None
    element_count: Optional[int] = None


class DeduplicatedTypeValueType(pydantic.BaseModel):
    descriptor: Literal["function_type", "type_name"]
    source: Optional[str] = None
    type_name: Optional[str] = None
    referenced_file_index: Optional[int] = None
    referenced_stabs_type_number: Optional[int] = None
    value_type: Optional[ValueType] = None
    return_type: Optional[ReturnType] = None
    modifier: Optional[str] = None
    vtable_index: Optional[int] = None
    is_constructor: Optional[bool] = None
    element_type: Optional[ElementType] = None
    element_count: Optional[int] = None


class ParameterType(pydantic.BaseModel):
    descriptor: str
    source: Optional[str] = None
    type_name: Optional[str] = None
    referenced_file_index: Optional[int] = None
    referenced_stabs_type_number: Optional[int] = None
    value_type: Optional[ValueType] = None


class Parameter(pydantic.BaseModel):
    descriptor: str
    name: str
    class_: str = pydantic.Field(..., alias="class")
    storage: Storage
    block_low: int
    block_high: int
    type: ParameterType


class ReturnType(pydantic.BaseModel):
    descriptor: str
    source: Optional[str] = None
    type_name: Optional[str] = None
    referenced_file_index: Optional[int] = None
    referenced_stabs_type_number: Optional[int] = None
    value_type: Optional[ValueType] = None


class FunctionType(pydantic.BaseModel):
    descriptor: str
    parameters: list[Parameter]
    modifier: str
    vtable_index: int
    is_constructor: bool
    return_type: Optional[ReturnType] = None


class Storage(pydantic.BaseModel):
    type: str
    register_: Optional[str] = pydantic.Field(None, alias="register")
    register_class: Optional[str] = None
    dbx_register_number: Optional[int] = None
    register_index: Optional[int] = None
    is_by_reference: Optional[bool] = None
    stack_offset: Optional[int] = None
    global_location: Optional[str] = None
    global_address: Optional[int] = None


class Constant(pydantic.BaseModel):
    value: int
    name: str


class ElementType(pydantic.BaseModel):
    descriptor: Literal["array", "pointer", "type_name", "enum"]
    source: Optional[str] = None
    type_name: Optional[str] = None
    referenced_file_index: Optional[int] = None
    referenced_stabs_type_number: Optional[int] = None
    element_type: Optional[ElementType] = None
    element_count: Optional[int] = None
    value_type: Optional[ValueType] = None
    constants: Optional[list[Constant]] = None

    def parsed_size(self) -> tuple[int, str]:
        if self.descriptor == "pointer":
            return 1, "pointer"

        elif self.descriptor == "array":
            assert self.element_type
            assert self.element_count is not None
            if self.element_count == 0:
                # implicit size array
                _, type_name = self.element_type.parsed_size()
                return 0, type_name
            n, type_name = self.element_type.parsed_size()
            return self.element_count * n, type_name

        elif self.descriptor == "enum":
            return 1, "enum"

        else:  # "type_name"
            assert self.type_name
            return 1, self.type_name


class Local(pydantic.BaseModel):
    descriptor: str
    name: str
    class_: str = pydantic.Field(..., alias="class")
    storage: Storage
    block_low: int
    block_high: int
    type: ElementType
    storage_class: Optional[str] = None

    @property
    def parsed_size(self) -> tuple[int, str]:
        return self.type.parsed_size()


class SubSourceFile(pydantic.BaseModel):
    address: int
    path: str


class Function(pydantic.BaseModel):
    descriptor: str
    name: str
    address_range: AddressRange
    type: FunctionType
    locals: list[Local]
    line_numbers: list[list[int]]
    sub_source_files: list[SubSourceFile]
    storage_class: Optional[str] = None
    relative_path: Optional[str] = None


class Global(pydantic.BaseModel):
    descriptor: str
    name: str
    class_: str = pydantic.Field(..., alias="class")
    storage: Storage
    block_low: int
    block_high: int
    type: ElementType
    storage_class: Optional[str] = None

    @property
    def parsed_size(self) -> tuple[int, str]:
        return self.type.parsed_size()


class File(pydantic.BaseModel):
    descriptor: str
    path: str
    relative_path: str
    text_address: int
    types: list[Any]
    functions: list[Function]
    globals: list[Global]
    stabs_type_number_to_deduplicated_type_index: dict[str, int]


class UnderlyingType(pydantic.BaseModel):
    descriptor: str
    source: str
    type_name: str
    referenced_file_index: int
    referenced_stabs_type_number: int


class Field(pydantic.BaseModel):
    descriptor: str
    name: str
    relative_offset_bytes: int
    absolute_offset_bytes: int
    size_bits: int
    bitfield_offset_bits: Optional[int] = None
    underlying_type: Optional[UnderlyingType] = None
    source: Optional[str] = None
    type_name: Optional[str] = None
    referenced_file_index: Optional[int] = None
    referenced_stabs_type_number: Optional[int] = None
    value_type: Optional[ValueType] = None
    base_classes: Optional[list[Any]] = None
    fields: Optional[list[Field]] = None
    member_functions: Optional[list[Any]] = None
    element_type: Optional[ElementType] = None
    element_count: Optional[int] = None


class FieldModel(pydantic.BaseModel):
    descriptor: str
    name: str
    relative_offset_bytes: int
    absolute_offset_bytes: int
    size_bits: int
    value_type: Optional[ValueType] = None
    source: Optional[str] = None
    type_name: Optional[str] = None
    referenced_file_index: Optional[int] = None
    referenced_stabs_type_number: Optional[int] = None
    element_type: Optional[ElementType] = None
    element_count: Optional[int] = None
    bitfield_offset_bits: Optional[int] = None
    underlying_type: Optional[UnderlyingType] = None
    fields: Optional[list[Field]] = None
    member_functions: Optional[list[Any]] = None
    base_classes: Optional[list[Any]] = None


class DeduplicatedType(pydantic.BaseModel):
    descriptor: Literal["array", "builtin", "enum", "pointer", "struct", "type_name", "union"]
    name: Optional[str] = None
    storage_class: Optional[Literal["typedef"]] = None
    stabs_type_number: int
    files: list[int]
    class_: Optional[str] = pydantic.Field(None, alias="class")
    size_bits: Optional[int] = None
    base_classes: Optional[list[Any]] = None
    fields: Optional[list[FieldModel]] = None
    member_functions: Optional[list[Any]] = None
    source: Optional[str] = None
    type_name: Optional[str] = None
    referenced_file_index: Optional[int] = None
    referenced_stabs_type_number: Optional[int] = None
    value_type: Optional[DeduplicatedTypeValueType] = None
    conflict: Optional[bool] = None
    element_type: Optional[ElementType] = None
    element_count: Optional[int] = None
    constants: Optional[list[Constant]] = None


class CCCJSONv7Model(pydantic.BaseModel):
    version: Literal[7]
    files: list[File]
    deduplicated_types: list[DeduplicatedType]


# def test(stdump_json_path: str):
#     import json
#
#     with open(stdump_json_path, mode="r") as fh:
#         json_data = fh.read()
#
#     model = CCCJSONv7Model.model_validate_json(json_data)
#
#     type_map: dict[str, int] = {}
#
#     for n, dt in enumerate(model.deduplicated_types):
#         if dt.descriptor == "builtin":
#             assert dt.name and dt.name not in type_map
#             assert dt.class_ is not None
#             size_bits = int(dt.class_.split("-", maxsplit=1)[0])
#             assert size_bits % 8 == 0
#             type_map[dt.name] = size_bits // 8
#
#         elif dt.descriptor == "type_name":
#             assert dt.name is not None
#             assert dt.type_name is not None
#             if dt.name in type_map:
#                 if dt.size_bits:
#                     assert type_map[dt.name] == dt.size_bits // 8
#             if dt.name == dt.type_name == "void":
#                 type_map["void"] = type_map["int"]
#                 continue
#             assert dt.type_name in type_map, (n, dt.type_name)
#             type_map[dt.name] = type_map[dt.type_name]
#
#         elif dt.descriptor == "pointer":
#             assert dt.name is not None
#             assert dt.value_type is not None
#             if dt.value_type.descriptor == "function_type":
#                 assert dt.name not in type_map
#                 type_map[dt.name] = type_map["int"]
#             elif dt.value_type.descriptor == "type_name":
#                 assert dt.value_type.type_name
#                 assert dt.value_type.type_name in type_map
#                 type_map[dt.name] = type_map[dt.value_type.type_name]
#
#         elif dt.descriptor == "struct":
#             assert dt.name is not None
#             if not dt.conflict:
#                 assert dt.name not in type_map, n
#             else:
#                 if dt.name in type_map:
#                     continue
#             assert dt.size_bits is not None
#             assert dt.size_bits % 8 == 0
#             type_map[dt.name] = dt.size_bits // 8
#
#         elif dt.descriptor == "array":
#             assert dt.name is not None
#             element_count = 1
#             element_type = dt.element_type
#             type_name = None
#             while element_type:
#                 if element_type.element_count is not None:
#                     element_count *= element_type.element_count
#                 if element_type.type_name is not None:
#                     type_name = element_type.type_name
#                     assert type_name in type_map
#                     element_count *= type_map[type_name]
#                 element_type = element_type.element_type
#             assert type_name and type_name in type_map, n
#             type_map[dt.name] = type_map[type_name] * element_count
#
#         elif dt.descriptor == "enum":
#             if dt.name:
#                 if not dt.conflict:
#                     assert dt.name not in type_map, n
#                 type_map[dt.name] = type_map["int"]
#
#         elif dt.descriptor == "union":
#             assert dt.name
#             assert dt.size_bits
#             assert dt.size_bits % 8 == 0
#             type_map[dt.name] = dt.size_bits // 8
#
#         else:
#             assert False, f"unknown {n}"
#
#     print(json.dumps(type_map, indent=2))


# if __name__ == "__main__":
#     test(path-to-stdump-json)
