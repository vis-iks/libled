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

### Building
The library is built using CMake. You can build it as a standalone project, which will also compile the included demo application.

```bash
mkdir build
cd build
cmake ..
make
```

### Running the Demo
The `apps/demo` project serves as a comprehensive example of how to use LibLed.

To run the demo, you must ensure that a `configuration.toml` configuration file is present in the execution directory.


```bash
./build/apps/demo
```

### Configuration
LibLed uses a configuration file (typically `configuration.toml`) to set up display parameters (resolution, chain length, brightness) and audio settings.
