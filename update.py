#!/usr/bin/env python
from __future__ import print_function, division

from plumbum import local, cli, colors, FG
from plumbum.cmd import git
import re

ver_re = re.compile(r'6.\d\d.\d\d')

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

def update_version(root_version, filename):
    "Update the version number everywhere in file"
    textfile = master_dir / filename
    text = textfile.read()
    text = ver_re.sub(root_version.replace('-','.').replace('v',''), text)
    textfile.write(text)

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

        # Set the version number in files
        update_version(root_version, 'README.md')
        update_version(root_version, 'CMakeLists.txt')
        update_version(root_version, 'setup.py')


if __name__ == '__main__':
    Update.run()
