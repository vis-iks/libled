# LibLed

LibLed is a lightweight C++ library designed for creating rich graphical and audio applications, specifically tailored for LED matrix displays (via `rpi-rgb-led-matrix`) and desktop development (via X11).

## Features

### Graphics
The library provides an abstraction layer for graphics rendering, allowing the same code to run on a desktop window or a physical LED matrix.

*   **Platforms**:
    *   **RGB Matrix**: Direct support for Raspberry Pi LED matrices using the `rpi-rgb-led-matrix` library.
    *   **X11**: Desktop simulation window for easy development and debugging on Linux.
*   **Primitives**: Support for drawing pixels, lines, rectangles, and clearing the canvas.
*   **Images**: Load and render images (DDS format supported).
*   **Fonts**: Bitmap font support for text rendering.
*   **Canvas**: Advanced canvas manipulation including blending, masking, and pixel access.

### Audio
Integrated audio system wrapping FMOD.

*   **Sound & Music**: Play sound effects and music tracks.
*   **Formats**: Supports MP3, WAV, and other FMOD-supported formats.
*   **Volume Control**: Global and per-channel volume management.

### Effects
A collection of ready-to-use visual effects for dynamic displays.

*   **Text Effects**: Scrolling text, typewriter effect, scaling text.
*   **Shaders**: Pixel shader-like effects (Plasma, Fire, Voronoi) that can be applied to the canvas.
*   **Particles**: Particle system for explosions, rain, and other motion effects.
*   **Transitions**: Screen melt, dissolve, and other screen transition effects.

### Utilities
*   **Configuration**: TOML-based configuration loading.
*   **Filesystem**: Helpers for file path management and resource loading.
*   **Math**: Vector and matrix math utilities (GLM integration).
*   **Tweens**: Tweening library integration for smooth animations.

## Usage

### Prerequisites

- **X11 Development Headers**: Required for desktop simulation.
- **FMOD Library**: The library includes FMOD in `external/fmodstudioapi`.

### Building
The library and demo are built using CMake. 

```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

### Running the Demo
The demo is built in the `build/demo` directory. It requires a `configuration.toml` file (automatically copied to the build directory) and needs the FMOD shared library to be in the library search path.

A helper script or a one-liner can be used:

```bash
./build/demo/Demo
```

### Configuration
LibLed uses a configuration file (typically `configuration.toml`) to set up display parameters (resolution, chain length, brightness) and audio settings. The demo looks for it in its own directory.
