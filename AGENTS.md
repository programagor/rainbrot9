This repository uses a simple Makefile and SDL2-based dependencies.

# Building

1. Ensure submodules are synced and initialized:
   ```bash
   git submodule sync
   git submodule update --init --recursive
   ```
2. Install build dependencies (Debian/Ubuntu packages):
   ```bash
   sudo apt-get update
   sudo apt-get install -y libsdl2-dev libmpfr-dev libgmp-dev libgl1-mesa-dev
   ```
3. Build the project:
   ```bash
   make
   ```
   The resulting executable `rainbrot9` will be placed in the repository root.

Run `make clean` to remove build artifacts.
