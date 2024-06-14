# Himuro
## What is Himuro?
Himuro is a project that aims to have a matching source decompilation of the game Fatal Frame 1; Project Zero; <ruby>零<rt>ぜろ</rt></ruby>. Once we have a matching decompiled rom we want to port the game to modern hardware.

## Getting Started & Building
Linux is required in order to build this project. It requires a compiler that is only available for Linux.

### Install dependencies
```bash
sudo apt install make binutils-mips-linux-gnu python3-full gcc-mips-linux-gnu ninja-build
```

### Setup a Virtual Environment for Python
```bash
python3 -m venv .venv
source .venv/bin/activate
python3 -m pip install -r requirements.txt
```

### Configure the project
#### EU
```bash
python3 configure.py -c config/eu/ff1.eu.yaml
make eu-configure
make eu-extract-data
```

#### US
```bash
python3 configure.py -c config/us/ff1.us.yaml
make us-configure
make us-extract-data
```

### Compile
#### EU
```bash
make eu-build
```

#### US
```bash
make us-build
```

#### Clean Artifacts 
```bash
ninja -t clean
```

#### Compile All 
```bash
ninja
```

## Adding a TU
1. Update config/{region}/ff1.{region}.yaml 
   1. Replace `asm` with `c` for the TU you want to splat
   2. For each section, duplicate the last line and update the name (e.g., duplicate `{ start: 0x24abc0, type: rodata,  vram: 0x349bc0, name: rodata_18 }`
      and update the name to `rodata_19`)
   3. Update the now second to last line with the path of the TU in the name field (e.g., `rodata_18` to `os/eeiop/adpcm/ea_event`)
   4. Add a leading dot (`.`) to the type (e.g., `rodata` -> `.rodata`) to make splat fetch its contents from the compiled TU
   5. Change `start` and `vram` addresses of the last line to point to the first symbol of the next TU
      1. `start` is the `Byte Source Offset` address
      2. `vram` is the address indicated in Ghidra's UI
2. Reconfigure the project
3. Build the project

