#pragma once

#include <core/Color.h>

/**
 * Demo shader collection
 * These are specific visual effects for the demo, not core library
 * functionality
 */

// Sine ripple shader - circular wave patterns
Color SineShader(float u, float v, float time);

// Joy Division waves shader - iconic wave ridge patterns
Color PleasuresShader(float u, float v, float time);

// Fire shader - animated fire effect using fractional brownian motion
Color FireShader(float u, float v, float time);

// Voronoi shader - cellular noise patterns
Color VoronoiShader(float u, float v, float time);

// Grid shader - retro vaporwave styles
Color GridShader(float u, float v, float time);
