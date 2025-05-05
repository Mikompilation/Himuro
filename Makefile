# Disable printing "Entering/Leaving directory ..."
MAKEFLAGS += --no-print-directory

NUMPROC ?= $(shell nproc)

.PHONY: help \
		.us-build-only .us-check-files-on-error us-configure us-build us-extract-data us-make-asm us-map-mismatch us-report clean \
		.eu-build-only .eu-check-files-on-error eu-configure eu-build eu-extract-data eu-make-asm eu-map-mismatch eu-report clean

.DEFAULT_GOAL := help

# python script that parses makefiles and pretty-prints help messages
define PY_HELP_SCRIPT
import os
import re
import sys

CYAN = "\033[36m"
CRST = "\033[0m"

makefiles = sys.argv[1:]
target_max_len = 0
target_help_list = []

for file in makefiles:
	with open(file, mode="r") as fh:
		for line in fh:
			if match := re.match(r"^([a-zA-Z0-9_-]+):\s*##\s*(.*)$$", line):
				target, help_str = match.groups()
				target_max_len = max(target_max_len, len(target))
				target_help_list.append((target, help_str))

			if match := re.match(r"^##\s*(.*)$$", line):
				comment = match.group(1)
				target_help_list.append((None, comment))

all_targets = [target for target, help_or_comment in target_help_list if target]

print(f"{CYAN}usage:{CRST} make <{CYAN}{f'{CRST}|{CYAN}'.join(all_targets)}{CRST}>")
print()

for target, help_or_comment in target_help_list:
	if target:
		print(f"  {CYAN}{target:{target_max_len + 2}s}{CRST} {help_or_comment}")
	else:
		print(help_or_comment)

print()
endef

export PY_HELP_SCRIPT
PYHELP := python3 -c "$$PY_HELP_SCRIPT"

help: ## Show this help
	@$(PYHELP) $(MAKEFILE_LIST)
##
## US Commands:
us-configure: ## Configure US project (needs SLUS_203.88)
	@python3 configure.py config/us/ff1.us.yaml -c

.us-build-only:
	@cd config/us; \
	ninja -t clean; \
	ninja -j$(NUMPROC)

.us-check-files-on-error:
	ls -l config/us/build/SLUS_203.88 config/us/SLUS_203.88

us-build: ## Build US project
	@$(MAKE) .us-build-only; status=$$?; [ $$status -eq 0 ] || { $(MAKE) .us-check-files-on-error; }; exit $$status

us-extract-data:  ## Extract variables from .data in US config directory
	@python3 tools/python/parse_data.py us

us-make-asm:  ## Create expected asm folder in US config directory
	@python3 configure.py config/us/ff1.us.yaml --make-asm

us-map-mismatch:  ## Check for mismatches in US mapfile
	@python3 tools/python/map_mismatch.py --language us

us-report:  ## Create progress report in US config directory
	@(stat config/us/expected/obj/ >/dev/null 2>&1 || (echo "Target objects do not exist, please run \`make us-make-asm\`"; false));
	@(stat config/us/build/src/ >/dev/null 2>&1 || (echo "Base objects do not exist, please run \`make us-build\`"; false));
	@tools/objdiff/objdiff-cli report generate -p config/us/ -o config/us/report.json -f json
	@python3 tools/python/fix_report.py config/us/report.json
	@python3 -c "import json;from pathlib import Path;report=json.loads(Path('config/us/report.json').read_text());print(f\"Progress: {report['measures']['fuzzy_match_percent']:.2f}%\")"

us-clean:  ## Clean artifact in US config directory
	@cd config/us; \
	ninja -t clean;

##
## EU Commands:
eu-configure: ## Configure EU project (needs SLES_508.21)
	@python3 configure.py config/eu/ff1.eu.yaml -c

.eu-build-only:
	@cd config/eu; \
	ninja -t clean; \
	ninja -j$(NUMPROC)

.eu-check-files-on-error:
	ls -l config/eu/build/SLES_508.21 config/eu/SLES_508.21

eu-build: ## Build EU project
	@$(MAKE) .eu-build-only; status=$$?; [ $$status -eq 0 ] || { $(MAKE) .eu-check-files-on-error; }; exit $$status

eu-extract-data:  ## Extract initialized variables from .data in EU config directory
	@python3 tools/python/parse_data.py eu

eu-make-asm:  ## Create expected asm folder in EU config directory
	@python3 configure.py config/eu/ff1.eu.yaml --make-asm

eu-map-mismatch:  ## Check for mismatches in EU mapfile
	@python3 tools/python/map_mismatch.py --language eu

eu-report:  ## Create progress report in EU config directory
	@(stat config/eu/expected/obj/ >/dev/null 2>&1 || (echo "Target objects do not exist, please run \`make eu-make-asm\`"; false));
	@(stat config/eu/build/src/ >/dev/null 2>&1 || (echo "Base objects do not exist, please run \`make eu-build\`"; false));
	@tools/objdiff/objdiff-cli report generate -p config/eu/ -o config/eu/report.json -f json
	@python3 tools/python/fix_report.py config/eu/report.json
	@python3 -c "import json;from pathlib import Path;report=json.loads(Path('config/eu/report.json').read_text());print(f\"Progress: {report['measures']['fuzzy_match_percent']:.2f}%\")"

eu-clean:  ## Clean artifact in US config directory
	@cd config/eu; \
	ninja -t clean;

# Include custom makefile for user-defined commands
-include Makefile.custom
