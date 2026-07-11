# Contributing / Maintenance

This repository is a mirror of the `math/minuit2` directory of
[ROOT](https://github.com/root-project/root), extracted using ROOT's own
standalone machinery (`StandAlone.cmake` / `copy_standalone.cmake`, which are
part of upstream). The only repo-local files are `.gitignore` and `.github/`;
everything else is an exact copy of the ROOT contents. Do not edit mirrored
files here — fix them upstream in ROOT instead.

## Updating to a new ROOT version

With a ROOT checkout in `../root` at the target tag (e.g. `v6-40-02`):

1. Run the upstream copy step, which copies the needed files from
   `math/mathcore` and `core/foundation` (`inc/Fit`, `inc/Math`, `inc/ROOT`,
   `src/math`, `RVersion.hxx`, `LICENSE`, `LGPL2_1.txt`) into the ROOT source
   tree:

   ```bash
   cmake -S ../root/math/minuit2 -B /tmp/minuit2-extract -Dminuit2_standalone=ON
   ```

2. Mirror the populated tree into this repository:

   ```bash
   rsync -a --delete --exclude=.git --exclude=.gitignore --exclude=.github \
       ../root/math/minuit2/ ./
   ```

3. Verify isolation by building and testing from this repository, outside the
   ROOT tree (some ROOT versions have broken this in the past):

   ```bash
   cmake -S . -B build
   cmake --build build -j
   ctest --test-dir build --output-on-failure
   ```

   All tests should pass, including `ExampleCMakeBuild`, which does a full
   downstream `find_package(Minuit2)` consumer build.

4. Restore the ROOT checkout (removes exactly the files copied in step 1):

   ```bash
   git -C ../root clean -fd math/minuit2
   ```

## Gotchas

- `ExampleCMakeBuild` uses `find_package`, which consults the CMake user
  package registry (`~/.cmake/packages/Minuit2`). Stale entries pointing at
  old build trees can make it pick up ancient headers; delete them if the
  test fails with confusing errors.
- Since ROOT ~6.40, the version is read from the copied `RVersion.hxx` (the
  old `version_number` file is gone), and the internal LA/BLAS sources are
  consolidated into `MnMatrix`.
