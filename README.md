# Inazuma Eleven GL

Small C++/OpenGL arcade soccer prototype inspired by Inazuma Eleven.

## Requirements

- CMake 3.10+
- C++17 compiler (g++/clang++)
- OpenGL
- GLFW3 development package

On Ubuntu/Debian, you can install dependencies with:

```bash
sudo apt install build-essential cmake libglfw3-dev libgl1-mesa-dev
```

## Build

From the project root:

```bash
cmake -S . -B build-local
cmake --build build-local
```

## Run

```bash
./build-local/RonaldinhoSoccer
```

## Controls

- `W`, `A`, `S`, `D`: move the selected player (closest to ball)
- Hold `Space`: charge kick power
- Release `Space`: shoot

## Notes

- The `build/` folder in this repository may be machine-specific. If CMake reports cache path mismatches, use a fresh build directory such as `build-local/`.
