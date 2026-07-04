# Himuro / ひむろ
## What is Himuro?
Himuro is a project that aims to have a matching source decompilation of the PS2 game Fatal Frame 1; Project Zero; <ruby>零<rt>ぜろ</rt></ruby>. Once we have a matching decompiled rom we want to port the game to modern hardware.

## Progress
| Name | Region | Game ID     | Progress (powered by [decomp.dev](https://decomp.dev/))
|----------------|--------|-------------|--------------------------------
| Fatal Frame    | US     | SLUS_203.88 | [![](https://decomp.dev/Mikompilation/Himuro/SLUS_203.88.svg?mode=shield&category=us&label=⠀  Code   ⠀&measure=matched_code_percent)](https://decomp.dev/Mikompilation/Himuro/SLUS_203.88)⠀ [![](https://decomp.dev/Mikompilation/Himuro/SLUS_203.88.svg?mode=shield&category=us&label=Functions&measure=matched_functions)](https://decomp.dev/Mikompilation/Himuro/SLUS_203.88)
| Project Zero   | EU     | SLES_508.21 |[![](https://decomp.dev/Mikompilation/Himuro/SLES_508.21.svg?mode=shield&category=eu&label=⠀  Code   ⠀&measure=matched_code_percent)](https://decomp.dev/Mikompilation/Himuro/SLES_508.21)⠀ [![](https://decomp.dev/Mikompilation/Himuro/SLES_508.21.svg?mode=shield&category=eu&label=Functions&measure=matched_functions)](https://decomp.dev/Mikompilation/Himuro/SLES_508.21)
| Zero           | JP     | SLPS_250.74 |[![](https://decomp.dev/Mikompilation/Himuro/SLPS_250.74.svg?mode=shield&category=jp&label=⠀  Code   ⠀&measure=matched_code_percent)](https://decomp.dev/Mikompilation/Himuro/SLPS_250.74)⠀ [![](https://decomp.dev/Mikompilation/Himuro/SLPS_250.74.svg?mode=shield&category=jp&label=Functions&measure=matched_functions)](https://decomp.dev/Mikompilation/Himuro/SLPS_250.74)

## Disclaimer
No game assets are hosted on this repository. You must provide them yourself, copying them from a legal copy.


## Getting Started & Building
Linux is required in order to build this project. It requires a compiler that is only available for Linux. You'll also need at least `python 3.8` to run any command. Your linux distribution should come with `python3` already installed. If not, please follow your distribution's instructions to install it. Python is a mandatory dependency, as many core tools are written in python. On Ubuntu, if necessary, python3 can be installed with the following command:
```bash
sudo apt install python3-full
```

### Add i386 architecture
The original GCC compiler is a 32-bit executable, so on a 64-bit system the `i386` architecture must be added in order for the system to run it. On Ubuntu you can use the following commands:
```bash
sudo dpkg --add-architecture i386
sudo apt update
sudo apt install libc6:i386 libstdc++6:i386
```

### Install dependencies
```bash
sudo apt install make python3-venv
```

### Install decompals binutils

This project requires a custom PS2-specific version of binutils to assemble the extracted MIPS assembly. The system-provided `binutils-mips-linux-gnu` package may not work correctly with PS2 assembly, so manual installation is required.

Download the [binutils-mips-ps2-decompals-linux-x86-64.tar.gz (v0.10)](https://github.com/decompals/binutils-mips-ps2-decompals/releases/download/v0.10/binutils-mips-ps2-decompals-linux-x86-64.tar.gz) package.

Extract the archive to a location of your choice, for example:

```bash
sudo mkdir -p /opt/binutils-mips-ps2-decompals
sudo tar -xzf binutils-mips-ps2-decompals-linux-x86-64.tar.gz -C /opt/binutils-mips-ps2-decompals
```

Then add the bin directory to your `PATH` environment variable:

```bash
export PATH="/opt/binutils-mips-ps2-decompals/:$PATH"
```

To make this change persistent, add the line above to your shell configuration file (for example `~/.bashrc`, `~/.zshrc`, or equivalent), then restart your terminal or reload your shell.

Verify the installation by running:

```bash
mips-ps2-decompals-as --version
```

If the installation was successful, the command should print the assembler version information.


### Setup a Virtual Environment for Python
Python's virtual environments are the preferred way to use this software, as you may not be allowed to install packages globally. 
```bash
python3 -m venv .venv
source .venv/bin/activate
python3 -m pip install -r requirements.txt
```

### Copy assets from game DVD/ISO
The main executable is needed in order to perform the decompilation.
#### EU
Copy `SLES_508.21` from DVD/ISO into `config/eu/`. File **must** have the following sha1 hash:
```
cb36d4365e70be9e9dc5bb66e2aa083811a39f12
```

#### US
Copy `SLUS_203.88` from DVD/ISO into `config/us/`. File **must** have the following sha1 hash:
```
5ac4626e0d76218e977b01f4fae5948de9a1755e
```

#### JP
Copy `SLPS_250.74` from DVD/ISO into `config/jp/`. File **must** have the following sha1 hash:
```
7aa0b147649b015a14f86c8f89cc7c1b89430958
```

### Extract data
NOTE: this operation needs to be performed only once.
#### EU
```bash
make eu-extract-data
```

#### US
```bash
make us-extract-data
```

#### JP
```bash
make jp-extract-data
```

### Configure the project
NOTE: running these commands on an already configured project will reconfigure it.
#### EU
```bash
make eu-configure
```

#### US
```bash
make us-configure
```

#### JP
```bash
make jp-configure
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

#### JP
```bash
make jp-build
```

### Clean Artifacts
#### EU
```bash
make eu-clean
```

#### US
```bash
make us-clean
```

#### JP
```bash
make jp-clean
```

### Command Help
To have a list all available commands, run `make` without targets:
```bash
make
```

## Decompiling a TU
1. Add the decompiled code to the TU's `c` file in `src/`
2. Update `config/{region}/ff1.{region}.yaml`:
   1. Replace `asm` with `c` for the TU you are decompiling
   2. Add a leading dot (`.`) to the type (e.g., `rodata` -> `.rodata`) of each subsection that belongs to the TU
3. Reconfigure the project
4. Build the project

