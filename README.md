# Inazuma Eleven GL

Arcade-style 2D football game inspired by Inazuma Eleven, built in C++ with OpenGL and GLFW.

## Overview

This project includes:
- 11v11 gameplay (user team vs AI team)
- Ball possession, passing, tackling, and scoring
- Animated stadium crowd and scoreboard
- Hissatsu-style charged special shot effects
- Particle effects, motion blur, and ball spin (Magnus curve)
- Background music and sound effects

For full technical details (architecture, module responsibilities, runtime flow, and specifications), see `PROJECT_DOCUMENTATION.md`.

## Controls

- `W`, `A`, `S`, `D`: Move the currently controlled player (nearest eligible teammate)
- `SPACE` (hold): Charge shot power
- `SPACE` (release): Shoot toward mouse cursor
- Mouse: Aim shot direction

## Requirements (Linux)

Install toolchain and runtime dependencies:

```bash
sudo apt update
sudo apt install build-essential libglfw3-dev libgl1-mesa-dev
```

## Build and Run

Build:

```bash
make
```

Run:

```bash
./InazumaElevenGL
```

Clean build artifacts:

```bash
make clean
```

## Tech Stack

- `C++17`
- `OpenGL` (compatibility-style immediate mode)
- `GLFW`
- `GLAD`
- `stb_image`
- `miniaudio`

## Project Structure

```text
include/                 # Public interfaces and shared data structures
src/main.cpp             # Entry point
src/game.cpp             # Main initialization/update/render loop
src/game_logic.cpp       # Core match and team update logic
src/player.cpp           # Player AI and rendering
src/ball.cpp             # Ball physics, animation, and ball VFX
src/field.cpp            # Pitch rendering
src/stadium.cpp          # Stadium, crowd animation, scoreboard
src/input.cpp            # Input mapping
src/audio.cpp            # Audio system wrapper (miniaudio)
src/particle.cpp         # Particle simulation/rendering
src/utils.cpp            # Texture and asset-path helpers
src/glad.c               # GLAD loader implementation (generated)
src/stb_image_impl.cpp   # stb_image implementation unit
assets/                  # Textures and sounds
Makefile                 # Build rules
```

## Assets

Expected assets are under `assets/`, including:
- Team/player sprites (blue/red + running frames)
- Ball sprites (normal + super shot)
- Crowd textures (`fans_*`)
- Sound files:
  - `assets/sound/background-sound.mp3`
  - `assets/sound/kick.mp3`
  - `assets/sound/referee-start.mp3`

The game uses fallback lookup paths at runtime to locate `assets/`.

## Notes

- Rendering is intentionally simple and educational (OpenGL immediate mode).
- The game loop is single-threaded and deterministic by frame order.
- AI behavior is heuristic-based (distance, role, and probabilistic decisions).

