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

# Project structure

- `main.cpp` - entry point and GUI logic
- `beam.hpp`/`beam.cpp` - beam parameters and sampling
- `plate.hpp`/`plate.cpp` - plate projection data
- `model.hpp`/`model.cpp` - fractal model registry
- `quaternion.hpp` - quaternion math utilities
- `tests/` - Catch2 unit tests
- `external/imgui` - Dear ImGui submodule

# Testing

Build and run the unit tests with:

```bash
make test
./test_runner
```
