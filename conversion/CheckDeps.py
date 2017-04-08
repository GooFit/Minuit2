#!/usr/bin/env python3

from pathlib import Path
import re

m2 = list(Path('../src/').glob('*/*.cxx'))

pattern = re.compile(r'#include\s+[\<"]([\w\./]*)[\>"]')

files = set()
for f in m2:
    with open(f) as fi:
        inc = set(pattern.findall(fi.read()))
        files |= inc

for f in sorted(files):
    print(f)

