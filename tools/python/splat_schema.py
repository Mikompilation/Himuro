from __future__ import annotations

from pathlib import Path
from typing import Literal, Optional
from pydantic import RootModel, BaseModel, Field


class Compiler(BaseModel):
    name: str
    asm_function_macro: str = Field("glabel", description="")
    asm_function_alt_macro: str = Field("glabel", description="")
    asm_jtbl_label_macro: str = Field("glabel", description="")
    asm_data_macro: str = Field("glabel", description="")
    asm_end_label: str = Field("", description="")
    c_newline: str = Field("\n", description="")
    asm_inc_header: str = Field("", description="")
    include_macro_inc: bool = Field(True, description="")
    asm_emit_size_directive: Optional[bool] = Field(None, description="")


class SplatOpts(BaseModel):
    # Debug / logging
    verbose: Optional[bool] = Field(None, description="Verbose")
    dump_symbols: Optional[bool] = Field(None, description="Dump symbols")
    modes: Optional[list[str]] = Field(None, description="modes")

    # Project configuration
    base_path: Path = Field(
        None, description="Determines the base path of the project. Everything is relative to this path"
    )
    target_path: Optional[Path] = Field(None, description="Determines the path to the target binary")
    elf_path: Optional[Path] = Field(None, description="Path to the final elf target")
    platform: Optional[str] = Field(None, description="Determines the platform of the target binary")
    compiler: Optional[Compiler | str] = Field(
        None, description="Determines the compiler used to compile the target binary"
    )
    endianness: Optional[Literal["big", "little"]] = Field(
        None, description="Determines the endianness of the target binary"
    )
    section_order: Optional[list[str]] = Field(
        None,
        description="Determines the default section order of the target binary. This can be overridden per-segment",
    )
    generated_c_preamble: Optional[str] = Field(
        None, description="Determines the code that is inserted by default in generated .c files"
    )
    generated_s_preamble: Optional[str] = Field(
        None, description="Determines the code that is inserted by default in generated .s files"
    )
    use_o_as_suffix: Optional[bool] = Field(
        None, description="Determines whether to use .o as the suffix for all binary files?... TODO document"
    )
    gp_value: Optional[int] = Field(
        None, description="the value of the $gp register to correctly calculate offset to %gp_rel relocs"
    )
    check_consecutive_segment_types: Optional[bool] = Field(
        None, description="Checks and errors if there are any non consecutive segment types"
    )

    # Paths
    asset_path: Optional[Path] = Field(None, description="")
    symbol_addrs_paths: Optional[list[Path]] = Field(
        None,
        description="""Determines the path to the symbol addresses file(s)
A symbol_addrs file is to be updated/curated manually and contains addresses of symbols
as well as optional metadata such as rom address, type, and more

It's possible to use more than one file by supplying a list instead of a string""",
    )
    reloc_addrs_paths: Optional[list[Path]] = Field(None, description="")
    build_path: Optional[Path] = Field(None, description="Determines the path to the project build directory")
    src_path: Optional[Path] = Field(None, description="Determines the path to the source code directory")
    asm_path: Optional[Path] = Field(None, description="Determines the path to the asm code directory")
    data_path: Optional[Path] = Field(None, description="Determines the path to the asm data directory")
    nonmatchings_path: Optional[Path] = Field(None, description="Determines the path to the asm nonmatchings directory")
    cache_path: Optional[Path] = Field(
        None, description="Determines the path to the cache file (used when supplied --use-cache via the CLI)"
    )
    hasm_in_src_path: Optional[bool] = Field(
        None, description="Tells splat to consider `hasm` files to be relative to `src_path` instead of `asm_path`."
    )
    create_undefined_funcs_auto: Optional[bool] = Field(
        None,
        description="Determines whether to create an automatically-generated undefined functions fil. "
        "This file stores all functions that are referenced in the code but are not defined as seen by splat",
    )
    undefined_funcs_auto_path: Optional[Path] = Field(
        None, description="Determines the path to the undefined_funcs_auto file"
    )

    create_undefined_syms_auto: Optional[bool] = Field(
        None,
        description="Determines whether to create an automatically-generated undefined symbols file. "
        "This file stores all symbols that are referenced in the code but are not defined as seen by splat",
    )
    undefined_syms_auto_path: Optional[Path] = Field(
        None, description="Determines the path to the undefined_symbols_auto file"
    )

    extensions_path: Optional[Path] = Field(
        None, description="Determines the path in which to search for custom splat extensions"
    )

    lib_path: Optional[Path] = Field(
        None, description="Determines the path to library files that are to be linked into the target binary"
    )

    # TODO document
    elf_section_list_path: Optional[Path] = Field(None, description="")

    # Linker script
    subalign: Optional[int] = Field(
        None, description="Determines the default subalign value to be specified in the generated linker script"
    )

    auto_all_sections: Optional[list[str]] = Field(
        None,
        description="The following option determines whether to automatically configure the linker script to link "
        'against specified sections for all "base" (asm/c) files when the yaml doesn\'t have manual configurations '
        "for these sections.",
    )
    ld_script_path: Optional[Path] = Field(
        None, description="Determines the desired path to the linker script that splat will generate"
    )
    ld_symbol_header_path: Optional[Path] = Field(
        None,
        description="Determines the desired path to the linker symbol header, which exposes externed definitions "
        "for all segment ram/rom start/end locations",
    )
    ld_discard_section: Optional[bool] = Field(
        None, description="Determines whether to add a discard section with a wildcard to the linker script"
    )
    ld_sections_allowlist: Optional[list[str]] = Field(
        None,
        description="A list of sections to preserve during link time. It can be useful to preserve debugging sections",
    )
    ld_sections_denylist: Optional[list[str]] = Field(
        None,
        description="A list of sections to discard during link time. It can be useful to avoid using the wildcard "
        "discard. Note that this option does not turn off `ld_discard_section`",
    )
    ld_wildcard_sections: Optional[bool] = Field(
        None,
        description="Determines whether to add wildcards for section linking in the linker script "
        "(.rodata* for example)",
    )
    ld_use_symbolic_vram_addresses: Optional[bool] = Field(
        None,
        description="Determines whether to use `follows_vram` (segment option) and `vram_symbol` / `follows_classes` "
        "(vram_class options) to calculate vram addresses in the linker script. If disabled, this uses the plain "
        "integer values for vram addresses defined in the yaml.",
    )
    ld_partial_linking: Optional[bool] = Field(
        None,
        description="Change linker script generation to allow partially linking segments. Requires both "
        "`ld_partial_scripts_path` and `ld_partial_build_segments_path` to be set.",
    )
    ld_partial_scripts_path: Optional[Path] = Field(
        None, description="Folder were each intermediary linker script will be written to."
    )
    ld_partial_build_segments_path: Optional[Path] = Field(
        None, description="Folder where the built partially linked segments will be placed by the build system."
    )
    ld_dependencies: Optional[bool] = Field(
        None,
        description="Generate a dependency file for every linker script generated. Dependency files will have the "
        "same path and name as the corresponding linker script, but changing the extension to `.d`. Requires "
        "`elf_path` to be set.",
    )
    ld_legacy_generation: Optional[bool] = Field(
        None,
        description="Legacy linker script generation does not impose the section_order specified in the yaml "
        "options or per-segment options.",
    )
    segment_end_before_align: Optional[bool] = Field(
        None,
        description="If enabled, the end symbol for each segment will be placed before the alignment directive "
        "for the segment",
    )
    segment_symbols_style: Optional[str] = Field(
        None,
        description="Controls the style of the auto-generated segment symbols in the linker script. "
        "Possible values: Optional[splat, makerom",
    )
    ld_rom_start: Optional[int] = Field(
        None, description="Specifies the starting offset for rom address symbols in the linker script."
    )
    ld_fill_value: Optional[int] = Field(
        None,
        description="The value passed to the FILL statement on each segment. `None` disables using FILL "
        "statements on the linker script. Defaults to a fill value of 0.",
    )
    ld_bss_is_noload: Optional[bool] = Field(
        None,
        description="Allows to control if `bss` sections (and derivative sections) will be put on a `NOLOAD` "
        "segment on the generated linker script or not.",
    )
    ld_align_segment_vram_end: Optional[bool] = Field(
        None, description="Allows to toggle aligning the `*_VRAM_END` linker symbol for each segment."
    )
    ld_align_section_vram_end: Optional[bool] = Field(
        None, description="Allows to toggle aligning the `*_END` linker symbol for each section of each section."
    )
    ld_generate_symbol_per_data_segment: Optional[bool] = Field(
        None, description="If enabled, the generated linker script will have a linker symbol for each data file"
    )
    ld_bss_contains_common: Optional[bool] = Field(
        None, description="Sets the default option for the `bss_contains_common` attribute of all segments."
    )

    ################################################################################
    # C file options
    ################################################################################
    create_c_files: Optional[bool] = Field(
        None, description="Determines whether to create new c files if they don't exist"
    )
    auto_decompile_empty_functions: Optional[bool] = Field(
        None, description='Determines whether to "auto-decompile" empty functions'
    )
    do_c_func_detection: Optional[bool] = Field(
        None,
        description="Determines whether to detect matched/unmatched functions in existing c files so we can avoid "
        "creating .s files for already-decompiled functions",
    )
    c_newline: Optional[str] = Field(None, description="Determines the newline char(s) to be used in c files")

    ################################################################################
    # (Dis)assembly-related options
    ################################################################################
    symbol_name_format: Optional[str] = Field(
        None, description="The following options determine the format that symbols should be named by default"
    )
    symbol_name_format_no_rom: Optional[str] = Field(
        None, description="Same as above but for symbols with no rom address"
    )
    find_file_boundaries: Optional[bool] = Field(
        None,
        description="Determines whether to detect and hint to the user about likely file splits " "when disassembling",
    )
    pair_rodata_to_text: Optional[bool] = Field(
        None,
        description="Determines whether to detect and hint to the user about possible rodata sections "
        "corresponding to a text section",
    )
    migrate_rodata_to_functions: Optional[bool] = Field(
        None,
        description="Determines whether to attempt to automatically migrate rodata into functions "
        "(only works in certain circumstances)",
    )
    asm_inc_header: Optional[str] = Field(
        None, description="Determines the header to be used in every asm file that's included from c files"
    )
    asm_function_macro: Optional[str] = Field(
        None, description="Determines the macro used to declare functions in asm files"
    )
    asm_function_alt_macro: Optional[str] = Field(
        None,
        description="Determines the macro used to declare symbols in the middle of functions in asm files "
        "(which may be alternative entries)",
    )
    asm_jtbl_label_macro: Optional[str] = Field(
        None, description="Determines the macro used to declare jumptable labels in asm files"
    )
    asm_data_macro: Optional[str] = Field(
        None, description="Determines the macro used to declare data symbols in asm files"
    )
    asm_end_label: Optional[str] = Field(
        None, description="Determines the macro used at the end of a function, such as endlabel or .end"
    )
    asm_emit_size_directive: Optional[bool] = Field(
        None, description="Toggles the .size directive emitted by the disassembler"
    )
    include_macro_inc: Optional[bool] = Field(
        None, description="Determines including the macro.inc file on non-migrated rodata variables"
    )
    mnemonic_ljust: Optional[int] = Field(
        None, description="Determines the number of characters to left align before the TODO finish documenting"
    )
    rom_address_padding: Optional[bool] = Field(None, description="Determines whether to pad the rom address")
    mips_abi_gpr: Optional[str] = Field(
        None, description="Determines which ABI names to use for general purpose registers"
    )
    mips_abi_float_regs: Optional[str] = Field(
        None,
        description="""Determines which ABI names to use for floating point registers
Valid values: 'numeric', 'o32', 'n32', 'n64'
o32 is highly recommended, as it provides logically named registers for floating point instructions
For more info, see https://gist.github.com/EllipticEllipsis/27eef11205c7a59d8ea85632bc49224d""",
    )
    named_regs_for_c_funcs: Optional[bool] = Field(
        None, description="Determines whether functions inside c files should have named registers"
    )
    add_set_gp_64: Optional[bool] = Field(None, description='Determines whether to add ".set gp=64" to asm/hasm files')
    create_asm_dependencies: Optional[bool] = Field(
        None,
        description="Generate .asmproc.d dependency files for each C file which still reference functions "
        "in assembly files",
    )
    string_encoding: Optional[str] = Field(
        None, description="Global option for rodata string encoding. This can be overridden per segment"
    )
    data_string_encoding: Optional[str] = Field(
        None, description="Global option for data string encoding. This can be overridden per segment"
    )
    rodata_string_guesser_level: Optional[int] = Field(
        None, description="Global option for the rodata string guesser. 0 disables the guesser completely."
    )
    data_string_guesser_level: Optional[int] = Field(
        None, description="Global option for the data string guesser. 0 disables the guesser completely."
    )
    allow_data_addends: Optional[bool] = Field(
        None,
        description="Global option for allowing data symbols using addends on symbol references. "
        "It can be overridden per symbol",
    )
    disasm_unknown: Optional[bool] = Field(
        None,
        description="Tells the disassembler to try disassembling functions with unknown instructions instead of "
        "falling back to disassembling as raw data",
    )
    detect_redundant_function_end: Optional[bool] = Field(
        None,
        description="Tries to detect redundant and unreferenced functions ends and merge them together. "
        "This option is ignored if the compiler is not set to IDO.",
    )
    disassemble_all: Optional[bool] = Field(
        None, description="Don't skip disassembling already matched functions and migrated sections"
    )

    ################################################################################
    # N64-specific options
    ################################################################################
    header_encoding: Optional[str] = Field(None, description="Determines the encoding of the header")
    gfx_ucode: Optional[str] = Field(
        None,
        description="""Determines the type gfx ucode (used by gfx segments)
Valid options are ['f3d', 'f3db', 'f3dex', 'f3dexb', 'f3dex2']""",
    )
    libultra_symbols: Optional[bool] = Field(
        None,
        description="Use named libultra symbols by default. Those will need to be added to a linker script "
        "manually by the user",
    )
    ique_symbols: Optional[bool] = Field(
        None,
        description="Use named libultra symbols by default. Those will need to be added to a linker script "
        "manually by the user",
    )
    hardware_regs: Optional[bool] = Field(
        None,
        description="Use named hardware register symbols by default. Those will need to be added to a linker script "
        "manually by the user",
    )
    image_type_in_extension: Optional[bool] = Field(
        None, description="Append the image type to the output file extension"
    )

    ################################################################################
    # Compiler-specific options
    ################################################################################
    use_legacy_include_asm: Optional[bool] = Field(
        None,
        description="Determines whether to use a legacy INCLUDE_ASM macro "
        "format in c files only applies to GCC/SN64",
    )


class VramClass(BaseModel):
    name: str = Field(..., description="")
    vram: Optional[int] = Field(None, description="")


class DictSegment(BaseModel):
    start: int = Field(None, description="")
    rom_start: Optional[int] = Field(None, description="")
    rom_start: Optional[int] = Field(None, description="")
    rom_end: Optional[int] = Field(None, description="")
    type: str = Field(..., description="")
    name: str = Field(..., description="")
    vram: Optional[int] = Field(None, description="")
    vram_start: Optional[int] = Field(None, description="")
    vram_symbol: Optional[str] = Field(None, description="")
    vram_class: Optional[VramClass] = Field(None, description="")
    follows_vram: Optional[str] = Field(None, description="")
    align: Optional[int] = Field(None, description="")
    subalign: Optional[int] = Field(None, description="")
    section_order: Optional[list[str]] = Field(None, description="")
    subsegments: Optional[list[Segment]] = Field(None, description="")
    bss_size: Optional[int] = Field(None, description="")
    args: Optional[list[str]] = Field(None, description="")


class ListSegment(RootModel[tuple[int, str, str] | tuple[int, str] | tuple[int]]): ...


Segment = DictSegment | ListSegment


class Config(BaseModel):
    name: str
    sha1: str
    options: SplatOpts
    segments: list[Segment]


def main():
    import json

    with open("../../.vscode/schema/splat_config.schema.json", "w") as fh:
        fh.write(json.dumps(Config.model_json_schema(), indent=2))


if __name__ == "__main__":
    main()
