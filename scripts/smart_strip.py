#!/usr/bin/env python3
#
# Copyright (C) 2025  Tetex7
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

import argparse
import subprocess
import sys
from pathlib import Path
import tempfile
import random
import string

# --- argument parsing ---
parser = argparse.ArgumentParser(
    description="Strip non-prefixed C symbols from an archive.\nNot suitable for obfuscation It does not technically strip the symbol but renames them"
)
parser.add_argument("archive", type=Path, help="Path to the .a file")
parser.add_argument(
    "-p", "--prefix", default="sn_",
    help="Keep symbols starting with this prefix (default: sn_)"
)

parser.add_argument(
    "--silent", 
    action="store_true",
    help="Silences output (boolean flag)"
)

parser.add_argument(
    "--nm",
    type=Path,
    help="Path to toolchain nm Binary",
    required=True
)

parser.add_argument(
    "--objcopy",
    type=Path,
    help="Path to toolchain objcopy Binary",
    required=True
)

args = parser.parse_args()

def log_output(text:str):
    if not args.silent:
        print(text)

def random_c_symbol(length=5):
    if length < 1:
        raise ValueError("Length must be at least 1")
    
    letters = string.ascii_letters
    digits = string.digits
    
    first_char = random.choice(letters)
    if length == 1:
        return first_char
    
    remaining_chars = ''.join(random.choices(letters + digits, k=length - 1))
    return first_char + remaining_chars

def get_defined_symbols(nm_path: str, archive: str) -> str:
    """Return all defined symbol names from an archive."""
    result:subprocess.CompletedProcess[str] = subprocess.run(
        [nm_path, "--defined-only", "--format=posix", archive],
        text=True, capture_output=True, check=True
    )
    symbols = set()
    for line in result.stdout.splitlines():
        parts = line.strip().split()
        if len(parts) >= 3:
            name = parts[0]
            # Skip compiler internals and metadata
            if not name.startswith((".", "__")):
                symbols.add(name)
    return symbols

def main():
    if not args.objcopy.exists():
        sys.exit(f"Toolchain objcopy Binary not found: {args.objcopy}")
    if not args.nm.exists():
        sys.exit(f"Toolchain nm Binary not found: {args.nm}")
    if not args.archive.exists():
        sys.exit(f"Archive not found: {args.archive}")
    

    all_syms = sorted(get_defined_symbols(str(args.nm), str(args.archive)))
    to_strip = [s for s in all_syms if not s.startswith(args.prefix)]

    if len(to_strip) == 0:
        sys.exit(f"No symbols to strip in {args.archive}")

    log_output(f"Found {len(all_syms)} symbols, stripping {len(to_strip)} that aren't Prefixed with {args.prefix}")

    for sym in to_strip:
        redef_sym = f"{args.prefix}_internal_pri_{sym}_{random_c_symbol()}__"
        log_output(f"Stripping Symbol {sym} to {redef_sym} from {args.archive}")
        out = subprocess.run(
            [str(args.objcopy), f"--redefine-sym", f"{sym}={redef_sym}", str(args.archive), str(args.archive)],
            check=False,  # some may fail (e.g. static locals), that’s fine
            #text=True, capture_output=True
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL
        )
        #print(out.stderr)

    log_output("Symbol cleanup complete.")

if __name__ == "__main__":
    main()