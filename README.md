install deps
```bash
sudo apt install make binutils-mips-linux-gnu python3 python3-pip gcc-mips-linux-gnu ninja-build
```

force-configure the project
```bash
python3 -m venv .venv
source .venv/bin/activate
python3 -m pip install -r requirements.txt
python3 configure.py -c config/{region}/ff1.{region}.yaml
```

clean compile artifacts 
```bash
$ ninja -t clean
```

compile 
```bash
$ ninja
```
``

EU Build
```bash
make eu-configure
make eu-extract-data
make eu-build
```

clean compile artifacts
```
$ ninja -t clean
```

compile
```
$ ninja
```

1. update config/{lang}/ff1.{lang}.yaml
   a. replace asm with c for the TU you want to splat
   b. for each section, duplicate the last line and update the name (e.g., duplicate { start: 0x24abc0, type: rodata,  vram: 0x349bc0, name: rodata_18 } and update the name to rodata_19 (not really necessary to have it numbered but I can easily count how many we did up to now this way)
   c. update the now second to last line with the path of the TU in the name field (e.g., rodata_18 to os/eeiop/adpcm/ea_event)
   d. add a leading dot (.) to the type (e.g., rodata -> .rodata) to make splat fetch its contents from the compiled TU)
   e. change `start` and `vram` addresses of the last line to point to the first symbol of the next TU (I ususally use ghidra for this)
2. reconfigure the project: run `make eu-configure` to reconfigure the eu one, `make us-configure` for the us one
3. extract data vars: run `make eu-extract-data` (or `make us-extract-data`). this will extract big data variables into header files in config/{lang}/include/data
4. build and link: run `make eu-build` (or `make us-build`)
