#! /usr/bin/env python3

# pyright: reportUnknownMemberType=false, reportMissingTypeStubs=false

from __future__ import annotations

import os
import re
import sys
import json
import yaml
import copy
import splat
import shutil
import argparse
import tempfile
import subprocess
import ninja_syntax

from typing import Any, Union, Protocol, Literal, cast
from pathlib import Path
from contextlib import contextmanager

from splat.scripts import split
from splat.util.conf import load as splat_load_yaml
from splat.segtypes.linker_entry import LinkerEntry

# from tools.python.fix_gp import fix_gp
from tools.python.fix_assets import fix_assets
from tools.python.fix_linkerscript import fix_linkerscript


@contextmanager
def suppress_stdout_stderr():
    null_fds = [os.open(os.devnull, os.O_RDWR) for _ in range(2)]
    save_fds = [os.dup(1), os.dup(2)]
    os.dup2(null_fds[0], 1)
    os.dup2(null_fds[1], 2)
    try:
        yield
    finally:
        os.dup2(save_fds[0], 1)
        os.dup2(save_fds[1], 2)
        for fd in null_fds + save_fds:
            os.close(fd)


ROOT = Path(__file__).parent.resolve()
TOOLS_DIR = ROOT / "tools"

COMPILER = "ee-gcc2.9-991111-01"
COMPILER_FLAGS = "-DMATCHING_DECOMP -O2 -g2 -gstabs"  # -malign-functions=3

LANGUAGES: dict[str, str] = {
    "SLUS_203.88": "us",
    "SLES_508.21": "eu",
}

# list of object file names (e.g., "tim2_new.c.o"), that have asm that needs
# to be patched. the respective c files (e.g. "tim2_new.c") will be compiled
# in two steps: c -> asm, then asm -> obj. in between, the intermediate
# asm will be passed to `tools/python/fix_asm_matching.py` for patching.
ASM_PATCH_LIST: list[str] = [
    "effect_oth.c.o",
]

EUC_JP_LIST = [
    "title.c.o",
]


def get_compiler_command(command: str):
    compiler_dir = Path("tools") / "cc" / COMPILER
    ee_dir = compiler_dir / "lib" / "gcc-lib" / "ee"
    ee_compiler_dirname = next(os.walk(ee_dir))[1][0]

    commands = {
        "ee-gcc": compiler_dir / "bin" / "ee-gcc",
        "cc1": compiler_dir / "lib" / "gcc-lib" / "ee" / ee_compiler_dirname / "cc1",
        "cc1plus": compiler_dir / "lib" / "gcc-lib" / "ee" / ee_compiler_dirname / "cc1plus",
        "cpp": compiler_dir / "lib" / "gcc-lib" / "ee" / ee_compiler_dirname / "cpp",
        "as": compiler_dir / "ee" / "bin" / "as",
    }

    return commands[command]


def make_compiler_cmd(config_dir: Path, src_path: Path, language: str):
    rel_root = Path(os.path.relpath(ROOT, config_dir))
    tools_dir = rel_root / "tools"

    game_cc_dir = f"{tools_dir}/cc/{COMPILER}/bin"
    lib_cc_dir = f"{tools_dir}/cc/{COMPILER}/bin"

    language_define = ""
    if language == "eu":
        language_define = "-DBUILD_EU_VERSION"

    compiler_includes = f"-I{src_path.parent / 'src'} -I{src_path.parent / 'include'} -Iinclude -isystem include/sdk/ee -isystem include/gcc"
    assembler_includes = f"-Wa,-I{src_path.parent / 'include'} -Wa,-I{src_path.parent}"
    common_includes = f"{compiler_includes} {assembler_includes} {language_define}"
    game_compile_cmd = f"{game_cc_dir}/ee-gcc -c {common_includes} {COMPILER_FLAGS}"
    lib_compile_cmd = f"{lib_cc_dir}/ee-gcc -c -isystem include/{COMPILER} {common_includes} {COMPILER_FLAGS}"

    return game_compile_cmd, lib_compile_cmd, common_includes


WIBO_VER = "0.6.4"


def exec_shell(command: list[str]) -> str:
    ret = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    return ret.stdout


def clean(config_dir: Path, config: dict[str, Any]):
    """
    Removes all temporary files and folders.
    """
    basename = Path(config["options"]["basename"])

    asm_path = Path(config["options"]["asm_path"])
    asset_path = Path(config["options"]["asset_path"])
    build_path = Path(config["options"]["build_path"])
    cc_str_path = Path("cc-src")

    relative_asm_path = (config_dir / asm_path).resolve().relative_to(ROOT)
    relative_asset_path = (config_dir / asset_path).resolve().relative_to(ROOT)
    relative_build_path = (config_dir / build_path).resolve().relative_to(ROOT)
    relative_cc_str_path = (config_dir / cc_str_path).resolve().relative_to(ROOT)

    for file in (
        ".splache",
        "build.ninja",
        ".ninja_log",
        "permuter_settings.toml",
        "undefined_syms_auto.txt",
        f"{basename}.ld",
    ):
        (config_dir / file).unlink(missing_ok=True)

    shutil.rmtree(relative_asm_path, ignore_errors=True)
    shutil.rmtree(relative_asset_path, ignore_errors=True)
    shutil.rmtree(relative_build_path, ignore_errors=True)
    shutil.rmtree(relative_cc_str_path, ignore_errors=True)


def write_permuter_settings(config_dir: Path, src_path: Path, language: str):
    game_compile_cmd, _, _ = make_compiler_cmd(config_dir, src_path, language)
    with open(config_dir / "permuter_settings.toml", "w") as f:
        f.write(
            f"""compiler_command = "{game_compile_cmd} -D__GNUC__"
assembler_command = "mips-linux-gnu-as -march=r5900 -mabi=eabi -Iinclude"
compiler_type = "gcc"

[preserve_macros]

[decompme.compilers]
"tools/build/cc/gcc/gcc" = "{COMPILER}"
"""
        )


def build_stuff(
    config_dir: Path,
    config: dict[str, Any],
    linker_entries: list[LinkerEntry],
    language: str,
):
    basename = Path(config["options"]["basename"]).name
    ld_path = f"{basename}.ld"
    elf_path = f"build/{basename}"
    map_path = f"build/{basename}.map"
    pre_elf_path = f"build/{basename}.elf"

    src_path = Path(config["options"]["src_path"])
    game_compile_cmd, lib_compile_cmd, common_includes = make_compiler_cmd(config_dir, src_path, language)

    built_objects: set[Path] = set()

    def build(
        object_paths: Union[Path, list[Path]],
        src_paths: list[Path],
        task: str,
        variables: dict[str, str] = {},
        implicit_outputs: list[str] = [],
    ):
        if not isinstance(object_paths, list):
            object_paths = [object_paths]

        # FIX: escape any $ sign with $$ as ninja_syntax
        #      doesn't seem to do any escaping
        object_paths = [Path(str(entry).replace("$", "$$")) for entry in object_paths]
        src_paths = [Path(str(entry).replace("$", "$$")) for entry in src_paths]

        object_strs = [str(obj) for obj in object_paths]

        for object_path in object_paths:
            if object_path.suffix == ".o":
                built_objects.add(object_path)

            ninja.build(
                outputs=object_strs,
                rule=task,
                inputs=[str(s) for s in src_paths],
                variables=variables,
                implicit_outputs=implicit_outputs,
            )

    ninja = ninja_syntax.Writer(open(str(ROOT / config_dir / "build.ninja"), "w"), width=9999)

    # Rules
    cross = "mips-linux-gnu-"

    ld_args = "--no-warn-rwx-segments -EL -T undefined_syms.txt -T undefined_syms_auto.txt -T undefined_funcs_auto.txt -Map $mapfile -T $in -o $out"

    cpp = Path("..", "..", get_compiler_command("cpp"))

    ninja.rule(
        "as",
        description="as $in",
        # NOTE: Japanese strings are EUC-JP encoded!!
        #       We need to convert them from (-f) UTF-8 to (-t) EUC-JP while compiling,
        #       otherwise Japanese strings will be compiled wrong!
        command=f"{cpp} {common_includes} $in -o  - | iconv -f=UTF-8 -t=EUC-JP $in | {cross}as -no-pad-sections -EL -march=5900 -mabi=eabi -I{src_path.parent / 'include'} -o $out",
    )

    ninja.rule(
        "cc-s",
        description="compile c source to object through assembly",
        command=(
            f"s_in=$$(echo $in.S | sed 's,^[^/]*/[^/]*/,cc-src/,') && "  # .............. 1) remove ../../ from path + prefix with cc-src/ + suffix with .S and store it into s_in var: ../../src/file.c -> s_in=cc-src/src/file.c.S
            f'mkdir -p $$(dirname "$$s_in") && '  # ..................................... 2) create directory from s_in var: s_in=cc-src/src/path/to/file.c.S -> mkdir -p cc-src/src/path/to/
            f"{game_compile_cmd.replace(' -c ', ' -S ')} $in -o $$s_in && "  # .......... 3) replace -c with -S in gcc command to generate intermediate assembly file instead of object
            f"python3 {ROOT}/tools/python/fix_asm_matching.py {language} $$s_in && "  # . 4) fix assembly file using known patterns with tools/python/fix_asm_matching.py
            f"{game_compile_cmd} $$s_in -o $out && "  # ................................. 5) compile assembly file into object
            f"{cross}strip $out -N dummy-symbol-name"  # ................................ 6) strip 'dummy-symbol-name' from object
        ),
    )

    ninja.rule(
        "cc",
        description="cc $in",
        command=f"{game_compile_cmd} $in -o $out && {cross}strip $out -N dummy-symbol-name",
    )

    ninja.rule(
        "cc-eucjp",
        description="convert source to EUC-JP encoding and compile same as 'cc'",
        command=(
            f"eucjp_in=$$(echo $in.eucjp.c | sed 's,^[^/]*/[^/]*/,cc-src/,') && "  # . 1) remove ../../ from path + prefix with cc-src/ + suffix with .eucjp.c and store it into eucjp_in var: ../../src/file.c -> eucjp_in=cc-src/src/file.c.eucjp.c
            f'mkdir -p $$(dirname "$$eucjp_in") && '  # .............................. 2) create directory from eucjp_in var: s_in=cc-src/src/path/to/file.c.eucjp.c -> mkdir -p cc-src/src/path/to/
            f"iconv -o $$eucjp_in -f=UTF-8 -t=EUC-JP $in && "  # ..................... 3) convert source file to EUC-JP (save converted source to cc-src/src/file.c.eucjp.c)
            f'{game_compile_cmd} -I$$(dirname "$in") $$eucjp_in -o $out && '  # ...... 4) compile converted source file into object (also add original source directory as include path to allow relative imports)
            f"{cross}strip $out -N dummy-symbol-name"  # ............................. 5) strip 'dummy-symbol-name' from object
        ),
    )

    ninja.rule(
        "libcc",
        description="cc $in",
        command=f"{lib_compile_cmd} $in -o $out && {cross}strip $out -N dummy-symbol-name",
    )

    ninja.rule(
        "ld",
        description="link $out",
        command=f"{cross}ld {ld_args}",
    )

    ninja.rule(
        "sha1sum",
        description="sha1sum $in",
        command="sha1sum -c $in && touch $out",
    )

    ninja.rule(
        "elf",
        description="elf $out",
        command=f"{cross}objcopy $in $out -O binary",
    )

    for entry in linker_entries:
        seg = entry.segment

        if seg.type[0] == ".":
            continue

        if entry.object_path is None:
            continue

        if isinstance(
            seg,
            (
                splat.segtypes.common.asm.CommonSegAsm,
                splat.segtypes.common.data.CommonSegData,
            ),
        ):
            build(entry.object_path, entry.src_paths, "as")

        elif isinstance(seg, splat.segtypes.common.c.CommonSegC):
            entry.src_paths = [Path("../..") / Path(f"{src_path}") for src_path in entry.src_paths]
            if any(str(src_path).startswith("src/lib/") for src_path in entry.src_paths):
                build(entry.object_path, entry.src_paths, "libcc")

            elif entry.object_path.name in ASM_PATCH_LIST:
                build(entry.object_path, entry.src_paths, "cc-s")

            elif entry.object_path.name in EUC_JP_LIST:
                build(entry.object_path, entry.src_paths, "cc-eucjp")

            else:
                build(entry.object_path, entry.src_paths, "cc")

        elif isinstance(seg, splat.segtypes.common.databin.CommonSegDatabin):
            build(entry.object_path, entry.src_paths, "as")

        elif isinstance(seg, splat.segtypes.common.rodatabin.CommonSegRodatabin):
            build(entry.object_path, entry.src_paths, "as")

        elif isinstance(seg, splat.segtypes.common.textbin.CommonSegTextbin):
            build(entry.object_path, entry.src_paths, "as")

        elif isinstance(seg, splat.segtypes.common.sbss.CommonSegSbss):
            build(entry.object_path, entry.src_paths, "as")

        else:
            print(f"ERROR: Unsupported build segment type {seg.type}")
            sys.exit(1)

    ninja.build(
        pre_elf_path,
        "ld",
        ld_path,
        implicit=[str(obj) for obj in built_objects],
        variables={"mapfile": map_path},
    )

    ninja.build(
        elf_path,
        "elf",
        pre_elf_path,
    )

    ninja.build(
        elf_path + ".ok",
        "sha1sum",
        "checksum.sha1",
        implicit=[elf_path],
    )


def make_asm(config_path: Path, config: dict[str, Any]):
    """
    Extracts assembly for each function into 'expected/asm/' subfolder.

    The extraction is done following these steps:
        1. create temporary directory in project folder
        2. copy yaml to temporary directory
        3. modify yaml to have c subsegments instead of asm subsegments
        4. remove data, bss, etc., subsegments
        5. run splat on the new config yaml
        6. splat extracts assembly for each function into '<temp_dir>/asm/nonmatchings/'
        7. copy '<temp_dir>/asm/nonmatchings/' to '<project_dir>/expected/asm/'
    """
    with tempfile.TemporaryDirectory(dir=config_path, prefix="tmp_") as tmp_dir:
        tmp_path = Path(tmp_dir)

        yaml_path = tmp_path / "config.yaml"
        asm_path = tmp_path / "asm" / "nonmatchings"
        dst_path = tmp_path / ".." / "expected" / "asm"
        dst_path = dst_path.resolve().relative_to(ROOT)

        if dst_path.exists():
            print(f"expected asm dir '{dst_path}' already exists")
            return

        config = copy.deepcopy(config)
        config["options"]["target_path"] = "../" + config["options"]["target_path"]
        config["options"]["asm_path"] = "asm"
        config["options"]["src_path"] = "src"
        config["options"]["build_path"] = "build"
        config["options"]["asset_path"] = "assets"
        config["options"]["undefined_funcs_auto_path"] = "../" + config["options"]["undefined_funcs_auto_path"]
        config["options"]["undefined_syms_auto_path"] = "../" + config["options"]["undefined_syms_auto_path"]
        config["options"]["symbol_addrs_path"] = "../" + config["options"]["symbol_addrs_path"]
        config["options"]["extensions_path"] = "../" + config["options"]["extensions_path"]

        new_segments: list[Any] = []
        segments: list[Any] = config["segments"]
        for segment in segments:
            if isinstance(segment, list):
                new_segments.append(segment)
            elif isinstance(segment, dict) and segment["name"] == "main":
                new_subsegments: list[Any] = []
                subsegments = cast(list[Any], segment["subsegments"])
                for subsegment in subsegments:
                    if isinstance(subsegment, list):
                        if subsegment[1] == "asm":
                            subsegment[1] = "c"
                        new_subsegments.append(subsegment)
                    elif isinstance(subsegment, dict):
                        subsegment["type"] = subsegment["type"].strip(".")
                        if subsegment["type"] == "rodata":
                            # splat now requires rodata to be always prefixed with a dot (.)
                            # for the rodata migration to work properly
                            subsegment["type"] = ".rodata"
                        new_subsegments.append(subsegment)
                segment["subsegments"] = new_subsegments
                new_segments.append(segment)
        config["segments"] = new_segments

        def rename_locals(base_path: Path):
            for asm_file in base_path.rglob("*.s"):
                data = asm_file.read_text()
                data = re.sub(r"__local_\d+", "", data)
                asm_file.write_text(data)

        with yaml_path.open(mode="w") as yaml_file:
            yaml.dump(config, yaml_file, default_flow_style=False)

        with suppress_stdout_stderr():
            split.main([yaml_path], modes=["all"], verbose=False)

        # remove '__local_#' from asm
        rename_locals(asm_path)

        dst_path.parent.mkdir(parents=True, exist_ok=True)
        shutil.copytree(asm_path, dst_path, dirs_exist_ok=True)

        print(f"expected asm extracted to '{dst_path}'")

        # make expected objs

        for subseg in new_segments[1]["subsegments"]:
            if isinstance(subseg, list) and subseg[1] == "c":
                subseg[1] = "asm"
                subseg[2] += ".c"

        config["options"]["asm_jtbl_label_macro"] = "llabel"

        with yaml_path.open(mode="w") as yaml_file:
            yaml.dump(config, yaml_file, default_flow_style=False)

        shutil.rmtree(tmp_path / "asm")
        (tmp_path / ".splache").unlink()

        with suppress_stdout_stderr():
            split.main([yaml_path], modes=["all"], verbose=False)

        # remove '__local_#' from asm
        rename_locals(asm_path)

        dst_path = dst_path.parent / "obj"
        tmp_obj_path = tmp_path / "obj"
        tmp_asm_dir = tmp_path / "asm"

        cpp = Path("..", "..", "..", get_compiler_command("cpp"))

        for asm_file in tmp_asm_dir.rglob("*.c.s"):
            asm_file_rel = asm_file.relative_to(tmp_path)
            obj_file_rel = Path("obj") / asm_file.relative_to(tmp_asm_dir).with_suffix(".o")
            obj_file = tmp_obj_path / obj_file_rel.relative_to("obj")
            obj_file.parent.mkdir(parents=True, exist_ok=True)
            subprocess.run(
                f"{cpp} -I../../../src -I../../../include -Iinclude -isystem include/sdk/ee -isystem include/gcc -Wa,-I../../../include -Wa,-I../../..  '{asm_file_rel}' -o  - | "
                f"iconv -f=UTF-8 -t=EUC-JP '{asm_file_rel}' | "
                f"mips-linux-gnu-as -no-pad-sections -EL -march=5900 -mabi=eabi -I../../../include -o {obj_file_rel} {asm_file_rel}",
                shell=True,
                cwd=tmp_path,
            )

        shutil.copytree(tmp_obj_path, dst_path, dirs_exist_ok=True)

        print(f"expected obj built to '{dst_path}'")


def generate_objdiff_configuration(config_path: Path, config: dict[str, Any], language: str):
    """
    Generate `objdiff.json` configuration from splat YAML config.

    Parse splat YAML config to get a list of the TUs that need to
    be diffed and create appropriate `units` for objdiff to process.

    Target objects need to be extracted separately (see the
    `make <lang>-make-asm` command) in order for objdiff to find the
    target files.
    """
    segments: list[Any] = config["segments"]

    tu_to_diff: list[tuple[Literal["asm", "c"], str]] = []

    for segment in segments:
        if not (isinstance(segment, dict) and segment["name"] == "main"):
            # we are looking for the main segment
            continue

        subsegments = cast(list[Any], segment["subsegments"])

        for subsegment in subsegments:
            if isinstance(subsegment, list):
                _, subs_type, subs_name = cast(tuple[int, str, str], subsegment)

            elif isinstance(subsegment, dict):
                subs_type = cast(int, subsegment["type"])
                subs_name = cast(str, subsegment["name"])

            else:
                raise RuntimeError("invalid subsegment type")

            if subs_type in ("asm", "c"):
                if subs_name in (
                    "crt0",
                    "main/glob",
                    "graphics/motion/mim_dat",
                    "graphics/motion/acs_dat",
                    "graphics/scene/scene_dat",
                    "mc/mc",
                ):
                    # -> skip 'crt0' as it's not part of the game files
                    # -> skip 'main/glob' as it doesn't have a .text section
                    # -> skip 'graphics/motion/mim_dat' as it doesn't have a .text section
                    # -> skip 'graphics/motion/acs_dat' as it doesn't have a .text section
                    # -> skip 'graphics/scene/scene_dat' as it doesn't have a .text section
                    # -> skip 'mc/mc' as it doesn't have a .text section
                    continue

                tu_to_diff.append((subs_type, subs_name))

    units: list[dict[str, Any]] = []

    for tu_type, tu_name in tu_to_diff:
        target_path = Path("expected", "obj", tu_name).with_suffix(".c.o")

        # since we only compile fully decompiled TUs, the
        # "c" type implies that the TU is complete
        is_complete = tu_type == "c"

        if is_complete:
            # compose the build path as "build/src/path/of/tu.c.o"
            base_path = Path("build", "src", tu_name).with_suffix(".c.o")
        else:
            # leave unset if the TU is not yet decompiled
            base_path = None

        unit: dict[str, Any] = {
            "name": tu_name,
            "target_path": str(target_path),
            "base_path": str(base_path) if base_path else None,
            "metadata": {"progress_categories": [language]},
        }

        if not base_path:
            # remove base_path for not yet decompiled TUs
            # (adding first and then removing if base_path is none allows
            # for key ordering to be preserved, otherwise adding it later
            # would put it after metadata)
            del unit["base_path"]

        units.append(unit)

    category_name = {
        "us": "Fatal Frame",
        "eu": "Project Zero",
    }

    progress_categories = [
        {"id": language, "name": category_name[language]},
    ]

    objdiff_json: dict[str, Any] = {
        "$schema": "https://raw.githubusercontent.com/encounter/objdiff/main/config.schema.json",
        "custom_make": "true",
        "custom_args": [],
        "build_target": False,
        "build_base": False,
        "watch_patterns": [],
        "units": units,
        "progress_categories": progress_categories,
    }

    objdiff_path = config_path / "objdiff.json"

    with objdiff_path.open(mode="w") as fw:
        json.dump(objdiff_json, fw, indent=2)

    print(f"Wrote objdiff configuration ({len(units)} units) to {objdiff_path}")


def main():
    class ArgsProtocol(Protocol):
        YAML_FILE: Path
        clean: bool
        reset: bool
        make_asm: bool

    parser = argparse.ArgumentParser(description="Configure the project")
    parser.add_argument(
        "YAML_FILE",
        type=Path,
        help="yaml file to configure the project",
    )
    parser.add_argument(
        "-c",
        "--clean",
        help="Clean extraction and build artifacts",
        action="store_true",
    )
    parser.add_argument(
        "--make-asm",
        help="Extract assembly for each function into 'expected/asm/' subfolder",
        action="store_true",
    )
    parser.add_argument(
        "-r",
        "--reset",
        help="Reset config dir to original state",
        action="store_true",
    )
    args = cast(ArgsProtocol, parser.parse_args())

    # try:
    #     exec_shell(["wibo"])
    # except FileNotFoundError:
    #     print("ERROR: wibo does not appear to be accessible")
    #     print("To install it, please download it and put it in your PATH:")
    #     print(
    #         f"  wget https://github.com/decompals/wibo/releases/download/{WIBO_VER}/wibo && "
    #         f"chmod +x wibo && sudo mv wibo /usr/bin/"
    #     )
    #     sys.exit(1)

    cwd = Path(os.getcwd()).resolve()
    if not ROOT.samefile(cwd):
        print("ERROR: this script must be run from it's directory!")
        exit(1)

    config = splat_load_yaml(
        [args.YAML_FILE],
        modes=["all"],
        verbose=False,
        disassemble_all=False,
    )

    basename = config["options"]["basename"]
    config_dir = Path(args.YAML_FILE).parent

    if basename not in LANGUAGES:
        supported_elfs = f"{set(f'{elf} ({lang})' for elf, lang in LANGUAGES.items())}".replace("'", "")
        print(f"unsupported game ELF. Supported versions are: {supported_elfs}")
        exit(1)

    language = LANGUAGES[basename]

    if args.reset:
        clean(config_dir, config)
        return

    if args.make_asm:
        make_asm(config_dir, config)
        return

    if args.clean:
        clean(config_dir, config)

    split.main([args.YAML_FILE], modes=["all"], verbose=False)

    src_path = Path(config["options"]["src_path"])
    asm_path = Path(config["options"]["asm_path"])
    asset_path = Path(config["options"]["asset_path"])

    asm_data_path = (config_dir / asm_path).resolve().relative_to(ROOT) / "data"
    asset_rel_path = (config_dir / asset_path).resolve().relative_to(ROOT)
    assert asm_data_path.is_dir(), f"{asm_data_path} not found or not a directory"
    assert asset_rel_path.is_dir(), f"{asset_rel_path} not found or not a directory"

    # fix asset .incbin path from relative to root to relative to config dir:
    #  e.g., change:
    #     .incbin "config/us/assets/name.section.bin"
    #  to:
    #     .incbin "assets/name.section.bin"
    fix_assets(asm_data_path, asset_rel_path)

    linker_entries = split.linker_writer.entries

    build_stuff(config_dir, split.config, linker_entries, language)

    write_permuter_settings(config_dir, src_path, language)

    # # replace gp_rel assembler macro with explicit offset as the gcc used
    # # to compile the code does not support it
    # gp_value = split.config["options"]["gp_value"]
    # symbol_addrs_path = Path(split.config["options"]["symbol_addrs_path"])
    # asm_rel_path = (config_dir / asm_path).resolve().relative_to(ROOT)
    # symbol_addrs_rel_path = (config_dir / symbol_addrs_path).resolve().relative_to(ROOT)
    # assert asm_rel_path.is_dir(), f"{asm_rel_path} not found or not a directory"
    # assert symbol_addrs_rel_path.is_file(), f"{symbol_addrs_rel_path} not found"
    # fix_gp(asm_rel_path, gp_value, symbol_addrs_rel_path)

    # fix linkerscript by applying explicit alignments as
    # specified in the config yaml
    linkerscript_path = (config_dir / f"{basename}.ld").resolve().relative_to(ROOT)
    assert linkerscript_path.is_file(), f"{linkerscript_path} not found"
    fix_linkerscript(split.config, linkerscript_path)

    generate_objdiff_configuration(config_dir, split.config, language)


if __name__ == "__main__":
    main()
