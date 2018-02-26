#!/usr/bin/env python
from __future__ import print_function, division

from plumbum import local, cli, colors, FG
from plumbum.cmd import git
import re

ver_re = re.compile(r'6.\d\d.\d\d')
root_ver_re = re.compile(r'v6-\d\d-\d\d')

clone = git['clone']

# All paths are relative to this directory
master_dir = local.path(__file__).dirname

def process(a, b, root):
    "Copy a file in if changed, report"
    for f in master_dir // a:
        root_file = root / b / f.name
        assert root_file.exists()
        assert f.exists()
        if f.read() != root_file.read():
            colors.warn.print("Changed:", f.name)
            root_file.copy(f)
        else:
            colors.info.print("No changes:", f.name)

class Update(cli.Application):

    def main(self, root_version):
        with local.tempdir() as tmp:
            # Download ROOT with the requested tag
            root = tmp / 'root'
            with colors.blue:
                clone['--branch='+root_version,
                      '--depth=5',
                      'http://root.cern.ch/git/root.git',
                      root] & FG

            # Copy any changed files over to the current package
            process('src/Math/*.cxx', 'math/mathcore/src', root)
            process('src/Minuit2/*.cxx', 'math/minuit2/src', root)
            process('inc/fit/*.h', 'math/mathcore/inc/Fit', root)
            process('inc/Math/*.h', 'math/mathcore/inc/Math', root)
            process('inc/Minuit2/*.h', 'math/minuit2/inc/Minuit2', root)

        # Set the version number inside the readme
        readme = master_dir / 'README.md'
        text = readme.read()
        text = root_ver_re.sub(root_version, text)
        readme.write(text)

        # Set the version number inside CMake
        cmake = master_dir / 'CMakeLists.txt'
        text = cmake.read()
        text = ver_re.sub(root_version.replace('-','.').replace('v',''), text)
        cmake.write(text)


if __name__ == '__main__':
    Update.run()
