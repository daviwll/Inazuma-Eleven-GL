# Inazuma Eleven GL - Project Documentation

## 1) Project Overview

Inazuma Eleven GL is a 2D arcade-style football game inspired by *Inazuma Eleven*, implemented in C++ with OpenGL immediate-mode rendering and GLFW window/input handling.

The game loop supports:
- 11v11 players (user team + AI team)
- Ball possession, kicking, passing, tackling, and scoring
- Visual effects (Hissatsu charge, particles, motion blur, crowd animation)
- Audio playback (looped background + one-shot SFX)

Primary entrypoint: `src/main.cpp` -> `runGame()` in `src/game.cpp`.

## 2) Technologies Used

- `C++17` language standard (`Makefile`)
- `OpenGL` (compatibility profile style rendering through GLAD symbols)
- `GLFW` for window creation, context management, keyboard/mouse input
- `GLAD` (`src/glad.c`) as OpenGL function loader
- `stb_image` (`src/stb_image_impl.cpp`) for PNG decoding
- `miniaudio` (`src/audio.cpp`) for music/SFX playback
- GNU toolchain on Linux (`g++`, `gcc`, `make`)

Build flags and linkage are defined in `Makefile`:
- `-std=c++17 -Iinclude`
- linked libs: `-lglfw -lGL -ldl`

## 3) High-Level Architecture

The project is organized by gameplay/rendering domains:

- **Bootstrap + orchestration**
  - `src/main.cpp`: process entrypoint
  - `src/game.cpp`: creates all systems, runs update/render loop, performs shutdown

- **Core gameplay logic**
  - `src/game_logic.cpp`: reset, ball state progression, team behavior updates
  - `src/player.cpp`: per-player AI movement and rendering
  - `src/ball.cpp`: ball animation, spin physics, and ball-specific VFX

- **World rendering/UI**
  - `src/field.cpp`: field lines/geometry
  - `src/stadium.cpp`: stands, crowd animation, scoreboard

- **Input/Audio/Utility systems**
  - `src/input.cpp`: keyboard/mouse -> normalized game input
  - `src/audio.cpp`: background music + one-shot sounds
  - `src/particle.cpp`: generic particle simulation/render
  - `src/utils.cpp`: texture loading, textured quad helper, asset path discovery

- **Third-party implementations**
  - `src/glad.c`
  - `src/stb_image_impl.cpp`

Public interfaces are declared in `include/*.hpp`.

## 4) Module Responsibilities

### 4.1 Game Bootstrap (`src/game.cpp`, `include/game.hpp`)

`runGame()` performs:
1. GLFW + OpenGL context setup
2. Initialization of game objects (`Field`, `Stadium`, `Ball`, `ParticleSystem`, teams, audio)
3. Asset loading (textures/sounds from candidate base directories)
4. Main loop:
   - delta time computation
   - input polling
   - gameplay updates
   - rendering pass
   - buffer swap/event poll
5. Graceful shutdown (`stadium.shutdown()`, `audio.shutdown()`, GLFW cleanup)

It also defines `onKick` callback logic used by gameplay updates to trigger:
- kick sound playback
- particle emissions depending on shot type

### 4.2 Gameplay Rules (`src/game_logic.cpp`, `include/game_logic.hpp`)

- `resetGame(...)`
  - places ball in center
  - resets players to formation start positions
  - clears temporary statuses (stun, kick charge)
  - sets kickoff ownership and kickoff timer

- `updateBall(...)`
  - handles possession-follow mode or free-ball movement
  - applies friction
  - clamps to field bounds
  - resolves wall bounces
  - detects goals and updates score

- `updateTeam(...)`
  - user team: nearest-player control + charge-and-release kick behavior
  - AI team: chasing, marking, passing, dribbling, shooting decisions
  - possession transitions and tackle capture logic

### 4.3 Player System (`src/player.cpp`, `include/player.hpp`)

`Player` stores:
- kinematics (`x`, `y`, `speed`, facing)
- tactical identity (`side`, `role`)
- state (`stunTimer`, `kickPower`, animation timers)
- visual assets (static and running-frame textures)

Core methods:
- `moveTowards(...)`: normalized steering + bounds clamp
- `update(...)`: role-aware AI behavior
- `render()`: run-frame or directional static sprite + shadow fallback
- `renderPowerBar()`: charging UI above the player

### 4.4 Ball System (`src/ball.cpp`, `include/ball.hpp`)

`Ball` stores:
- position/velocity/friction
- pointer to current owner (or `nullptr` if free)
- texture animation state
- special shot state (`isSuperShot`, `chargingPower`)
- spin values (`spinX/Y/Z`) and Magnus force scale
- motion blur trail buffers

Core methods:
- `loadTextures()`: loads normal and super-shot textures
- `update(deltaTime)`: animation frame progression, spin decay, Magnus curve force
- `render()`: textured sprite with rotation
- `renderMotionBlur()`: high-speed trail
- `renderHissatsuEffect(power)`: ring/orb/speedline effect while charging

### 4.5 Stadium + Scoreboard (`src/stadium.cpp`, `include/stadium.hpp`)

- Lazy crowd texture loading and cleanup
- Animated crowd bands (idle motion + celebration jump)
- Celebration trigger by scoring side
- Simple custom scoreboard rendering with digit segments

### 4.6 Field Rendering (`src/field.cpp`, `include/field.hpp`)

Draws complete pitch geometry:
- striped grass
- perimeter lines
- midfield line + center circle
- penalty boxes
- goals

### 4.7 Input Handling (`src/input.cpp`, `include/input.hpp`)

`processInput(...)` reads:
- WASD movement axes
- SPACE hold/release state (`spacePressed` / `spaceWasPressed`)
- mouse cursor mapped to normalized world coordinates

Control gating is applied via `allowControls` (used during kickoff countdown).

### 4.8 Audio (`src/audio.cpp`, `include/audio.hpp`)

`AudioPlayer` wraps miniaudio:
- `init()`: engine allocation/init
- `initLoopingTrack(path, volume)`: starts looping music
- `playOneShot(path, volume)`: short SFX playback
- `shutdown()`: resource release

### 4.9 Particles (`src/particle.cpp`, `include/particle.hpp`)

`ParticleSystem` provides:
- `emit(...)`: radial spawn with velocity spread
- `update(deltaTime)`: gravity, friction, fade-out, dead-particle removal
- `render()`: blended point sprites
- `clear()`: immediate reset (used on goal celebration)

### 4.10 Shared Utilities (`src/utils.cpp`, `include/utils.hpp`)

- `loadTextureFromPng(...)`: texture creation from PNG
- `renderTexturedQuad(...)`: reusable quad helper
- `candidateBaseDirs()`: dynamic asset search paths

## 5) Runtime Flow (How Parts Relate)

### 5.1 Startup Flow

1. `main()` calls `runGame()`.
2. `runGame()` initializes rendering/audio systems.
3. Textures and sounds are loaded via `candidateBaseDirs()` fallback strategy.
4. Teams are created with role-based formations and animation frames.
5. `resetGame(...)` sets kickoff state.

### 5.2 Per-Frame Update Flow

1. `processInput(...)` fills `InputState`.
2. `updateBall(...)` advances ball/goal logic.
3. `ball.update(...)` advances spin/animation/trail.
4. `particles.update(...)` advances FX particles.
5. `updateTeam(...)` runs user control and AI decisions.
6. Rendering order:
   - stadium + scoreboard
   - field
   - ball + ball effects + particles
   - players + charge bars

### 5.3 Event Coupling

- Kick action from `updateTeam(...)` invokes `onKick(...)` callback in `runGame()`.
- Callback triggers audio + particle emissions.
- Goal result from `updateBall(...)` triggers crowd celebration and kickoff reset handling.

## 6) Gameplay Specifications

### 6.1 Teams and Roles

- 22 total players, 11 per side:
  - 1 goalkeeper
  - defenders, midfielders, attackers
- User controls nearest eligible player on one team.
- Opponent behavior is AI-driven.

### 6.2 Ball and Match Rules

- Ball can be owned (follows player facing) or free (velocity + friction).
- Possession changes by proximity and tackle conditions.
- Goals are detected when crossing side boundaries within goal vertical range.
- After goal:
  - score increments
  - crowd celebration triggers
  - kickoff countdown restarts

### 6.3 Kicking and Special Shots

- Hold SPACE to charge kick power (`0.0` to `1.0`).
- Release SPACE to shoot toward mouse direction.
- High-power close-range kicks can toggle `isSuperShot`.
- Spin values are injected on kick, then decayed over time.
- Magnus force creates curved trajectories.

### 6.4 Visual and Audio Feedback

- Running sprite animation for moving players
- Elliptical player shadows
- Ball rotation based on speed
- Motion blur trail for fast ball movement
- Hissatsu charging rings and speed lines
- Goal/kick/special-shot particles
- Background music + kick + referee whistle SFX

## 7) Rendering and Coordinate System

- Uses OpenGL immediate mode (`glBegin`/`glEnd`) with normalized world-space coordinates.
- Field extents and gameplay boundaries are defined in `include/constants.hpp`.
- Most gameplay uses a coordinate range roughly within `[-1, 1]` on both axes.

Key constants (`include/constants.hpp`):
- `FIELD_HALF_WIDTH`, `FIELD_HALF_HEIGHT`
- `FIELD_BOUNDARY_X`, `FIELD_BOUNDARY_Y`
- `GOAL_HALF_WIDTH`, `GOAL_DEPTH`
- `PENALTY_AREA_WIDTH`, `PENALTY_AREA_HEIGHT`

## 8) Asset and Resource Specifications

Expected runtime assets (under `assets/`):
- Player textures (blue/red, directional + running frames)
- Ball textures (normal frames + super-shot)
- Crowd textures (`fans_*`)
- Audio files:
  - `background-sound.mp3`
  - `kick.mp3`
  - `referee-start.mp3`

Asset lookup uses fallback base directories from `candidateBaseDirs()`.

## 9) Build and Run Specifications

### 9.1 Requirements (Linux)

- C++ compiler with C++17 support
- OpenGL development packages
- GLFW development packages
- GNU Make

### 9.2 Commands

```bash
make
./InazumaElevenGL
make clean
```

## 10) File Relationship Map (Quick Reference)

- `src/main.cpp` -> `src/game.cpp`
- `src/game.cpp` -> orchestrates:
  - `src/input.cpp`
  - `src/game_logic.cpp`
  - `src/ball.cpp`
  - `src/player.cpp`
  - `src/particle.cpp`
  - `src/stadium.cpp`
  - `src/field.cpp`
  - `src/audio.cpp`
  - `src/utils.cpp`

Headers in `include/` provide interfaces and shared structs/constants used across those modules.

## 11) Current Technical Characteristics

- Single-threaded game loop
- Deterministic frame-order update pipeline
- Immediate-mode OpenGL rendering (simple and educational, not modern GPU pipeline focused)
- Pointer-based possession reference (`Ball::owner` points to `Player` in team vectors)
- Lightweight AI heuristics (distance-based chase, probabilistic pass/shot decisions)

## 12) Development Attribution

- The project was developed by Antonio Guilherme and Davi Santos.
- AI-assisted development was used for parts of the sprite workflow and in-game physics implementation.
