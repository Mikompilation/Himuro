import re
import argparse

from pathlib import Path


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--language", required=True, choices=["us", "eu"])

    args = parser.parse_args()

    if args.language == "us":
        map_path = Path("config/us/build/SLUS_203.88.map")
    else:
        map_path = Path("config/eu/build/SLES_508.21.map")

    with open(map_path, mode="r") as fh:
        for n, line in enumerate(fh):
            line = line.rstrip("\n")
            if match := re.match(r"^\s*0x([0-9a-fA-F]+)\s+[^ ]+?([0-9a-fA-F]{6,8})\s*$", line):
                addr = match.group(1)
                label = match.group(2)
                if addr.upper() != label.upper():
                    print(f"{map_path}:{n+1} {line}")
                    return

    print("no mismatches found")


if __name__ == "__main__":
    main()
