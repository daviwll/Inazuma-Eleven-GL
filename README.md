# RonaldinhoSoccer

Small OpenGL/GLFW C++ project built with CMake.

## Build (Windows + MinGW)

From the project root:

```bash
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
```

Run:

```bash
./build/RonaldinhoSoccer.exe
```

## Build (Linux/macOS)

```bash
cmake -S . -B build
cmake --build build
```

Run:

```bash
./build/RonaldinhoSoccer
```

## Notes

- GLFW is fetched automatically by CMake (no manual install required).
- OpenGL must be available on your system.
