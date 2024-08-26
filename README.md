# Himuro
## What is Himuro?
Himuro is a project that aims to have a matching source decompilation of the PS2 game Fatal Frame 1; Project Zero; <ruby>零<rt>ぜろ</rt></ruby>. Once we have a matching decompiled rom we want to port the game to modern hardware.

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
sudo apt install make binutils-mips-linux-gnu gcc-mips-linux-gnu python3-venv
```

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

### Compile
#### EU
```bash
make eu-build
```

#### US
```bash
make us-build
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

