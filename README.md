# Rainbrot9

A fractal viewer and experimental project using SDL2, Dear ImGui and arbitrary precision arithmetic provided by GMP/MPFR.  The source uses the `external/imgui` submodule for GUI rendering.

## Dependencies

- **SDL2** development libraries.  On Debian/Ubuntu install `libsdl2-dev`; on macOS `brew install sdl2`; on MSYS2 use `pacman -S mingw-w64-i686-SDL2`.
- **GMP** and **MPFR** development libraries.
- **Dear ImGui** checked out as a submodule under `external/imgui`.

Clone with submodules and build using the provided `Makefile`:

```bash
git clone --recursive <repo-url>
cd rainbrot9
make
```

The executable `rainbrot9` will be produced in the repository root.  Run `make clean` to remove build artifacts.

