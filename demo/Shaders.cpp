#include "Shaders.h"
#include "Voronoi.h"
#include <cmath>
#include <core/GraphicsConstants.h>
#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <glm/mat2x2.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <utils/ShaderUtils.h>

using namespace libled;

// ============================================================================
// Sine Ripple Shader
// ============================================================================

Color SineShader(float u, float v, float time) {
  // Convert to centered coordinates
  glm::vec2 uv(u * 2.0f - 1.0f, v * 2.0f - 1.0f);

  // Aspect ratio correction
  float aspect = (float)DISPLAY_WIDTH / (float)DISPLAY_HEIGHT;
  if (aspect > 1.0f) {
    uv.x *= aspect;
  } else {
    uv.y /= aspect;
  }

  // Animated time
  float t = -time * 3.0f + 5000.0f + std::sin(time / 3.0f) * 5.0f;

  // Distance from center - reduced scaling to extend further
  float dist = glm::length(uv) * 0.35f;
  float maxDist = 1.2f;

  // Quadratic distance for smoother wave effect
  float expDist = dist * dist;
  float strength = (std::sin(expDist * 50.0f) + 1.0f) / 2.0f;
  float height = (std::sin(t * strength) + 1.0f) / 2.0f;
  float alpha =
      1.0f - expDist / (maxDist * maxDist) + (1.0f - height) * -0.014f;

  // Color with height-based intensity
  glm::vec3 color =
      glm::vec3(0.9f, 0.9f, 0.9f) * height - (1.0f - alpha) * 0.652f;

  // Clip outside max distance
  if (dist > maxDist) {
    color = glm::vec3(0.1f, 0.1f, 0.1f);
    alpha = 0.0f;
  }

  // Blend with black background using alpha
  color = Mix(glm::vec3(0.0f, 0.0f, 0.0f), color, Clamp(alpha, 0.0f, 1.0f));

  return Color((byte)Clamp(color.r * 255.0f, 0.0f, 255.0f),
               (byte)Clamp(color.g * 255.0f, 0.0f, 255.0f),
               (byte)Clamp(color.b * 255.0f, 0.0f, 255.0f));
}

// ============================================================================
// Electromagnetic Field Shader
// ============================================================================

// Complex logarithm
static glm::vec2 clog(glm::vec2 z) {
  return glm::vec2(std::log(glm::length(z)), std::atan2(z.y, z.x));
}

Color MagnetShader(float u, float v, float time) {
  // Constants
  const float PI = 3.1415926538f;
  const int e_solutions = 5;
  const int v_solutions = 5;
  const float particle_radius = 3.0f;
  const float fade_radius = 2.0f;
  const float line_thickness = 1.5f;

  // Convert to pixel coordinates
  glm::vec2 size(DISPLAY_WIDTH, DISPLAY_HEIGHT);
  glm::vec2 fragCoord = glm::vec2(u, v) * size;

  // Define particles and their charges
  int charges[] = {2, 2, -2};
  glm::vec2 particles[] = {
      size * glm::vec2(0.5f + std::sin(time * 0.5f) * 0.2f, 0.5f),
      size * glm::vec2(0.3f, 0.5f), size * glm::vec2(0.7f, 0.5f)};

  // Calculate complex potential field
  glm::vec2 field(0.0f, 0.0f);
  for (int i = 0; i < 3; i++) {
    glm::vec2 delta = fragCoord - particles[i];
    field += (float)charges[i] * clog(delta);
  }

  // Draw field lines using cosine contours
  float e = std::cos(field.y * (float)e_solutions);
  float e_deriv =
      std::abs(std::sin(field.y * (float)e_solutions) * (float)e_solutions);
  float e_value =
      line_thickness - std::abs(e / glm::max(e_deriv * 0.01f, 0.001f));

  // Equipotential lines with animation
  float preview_animation = std::fmod(time, 2.0f * PI);
  float vval = std::cos(field.x * (float)v_solutions + preview_animation);
  float v_deriv =
      std::abs(std::sin(field.x * (float)v_solutions + preview_animation) *
               (float)v_solutions);
  float v_value =
      line_thickness - std::abs(vval / glm::max(v_deriv * 0.01f, 0.001f));

  // Apply colors
  glm::vec3 e_color =
      glm::max(e_value, 0.0f) * glm::vec3(0.031f, 0.482f, 0.737f);
  glm::vec3 v_color =
      glm::max(v_value, 0.0f) * glm::vec3(0.031f, 0.596f, 0.490f);
  glm::vec3 color = glm::max(e_color, v_color);

  // Draw particles on top
  for (int i = 0; i < 3; i++) {
    glm::vec2 delta = fragCoord - particles[i];
    glm::vec3 particle_color = (charges[i] < 0) ? glm::vec3(1.0f, 0.0f, 0.0f)
                                                : glm::vec3(0.0f, 0.0f, 1.0f);

    float dist = glm::length(delta);
    float t = -(dist - particle_radius) / fade_radius;
    t = SmoothStep(0.0f, 1.0f, t);
    color = Mix(color, particle_color, t);
  }

  return Color((byte)Clamp(color.r * 255.0f, 0.0f, 255.0f),
               (byte)Clamp(color.g * 255.0f, 0.0f, 255.0f),
               (byte)Clamp(color.b * 255.0f, 0.0f, 255.0f));
}

// ============================================================================
// Joy Division Waves Shader
// ============================================================================

// Remap value
static float remap(float v, float l1, float h1, float l2, float h2) {
  return l2 + (v - l1) * (h2 - l2) / (h1 - l1);
}

// Hash function
static float hashPleasures(float p) {
  p = glm::fract(7.8233139f * p);
  p = ((2384.2345f * p - 1324.3438f) * p + 3884.2243f) * p - 4921.2354f;
  return remap(glm::fract(p), 0.1f, 1.0f, 2.0f, 3.0f);
}

// Fractal brownian motion for wave distortion
static float fbmPleasures(float v, float h, float t) {
  float f = std::sin(v + t) * 0.5f - 0.5f;
  f += std::sin(v * h * 1.145f + t * h * 3.345f) * 0.5f - 0.5f;
  f += std::sin(v * h * 1.776f + t * h * 2.964f) * 0.5f - 0.5f;
  f += std::sin(v * h * 2.454f - t * h * 0.478f) * 0.5f - 0.5f;
  f *= 2.0f;
  f *= SmoothStep(5.0f, -5.0f, std::abs(v));
  return f * 0.5f;
}

// Draw a sin curve with masking
static glm::vec3 sineCurve(glm::vec2 p, float w, float h, float t) {
  float s = p.y + fbmPleasures(p.x, h, t);

  float borderWidth = 15.0f / (float)DISPLAY_HEIGHT;
  float s1 = SmoothStep(0.0f, borderWidth, s - w);
  float s2 = SmoothStep(-borderWidth, 0.0f, s + w - 1.0f);

  float upMask = s1;
  float border = s1 - s2;
  float downMask = s2;

  return glm::vec3(upMask, border, downMask);
}

Color PleasuresShader(float u, float v, float time) {
  glm::vec2 R(DISPLAY_WIDTH, DISPLAY_HEIGHT);
  glm::vec2 fragCoord(u * R.x, v * R.y);

  // Much smaller scaling for fewer, much larger waves
  glm::vec2 p = 2.0f * (fragCoord - 0.5f * R) / R.y;

  float T = time * 0.4f;

  // Only 5 lines total
  glm::vec3 DOMAIN(0.8f, -0.8f, 0.4f);

  float s = 0.0f;
  for (float i = DOMAIN.x; i >= DOMAIN.y; i -= DOMAIN.z) {
    // Very thick lines
    glm::vec3 sn = sineCurve(p - glm::vec2(0.0f, i), 1.2f, hashPleasures(i), T);
    s = sn.y + glm::min(s, sn.z);
  }

  // Border clipping
  float col = glm::max(1.0f - s, std::abs(p.x) > 1.5f ? 1.0f : 0.0f);

  return Color((byte)(col * 255.0f), (byte)(col * 255.0f),
               (byte)(col * 255.0f));
}

// ============================================================================
// Fire Shader
// ============================================================================

Color FireShader(float u, float v, float time) {
  float t = time * 2.0f;
  glm::vec2 uv(u, v);
  float n1 = FractionalBrownianMotion(uv * 5.0f - glm::vec2(0, t));
  float n2 =
      FractionalBrownianMotion(glm::vec2(u * 10.0f + n1, v * 10.0f - t * 2.0f));

  float c = n2 * 1.5f;
  glm::vec3 col = glm::vec3(c, c * c, c * c * c * c) * 255.0f;
  col.r = std::min(255.0f, col.r * 2.0f);
  col.g = std::min(255.0f, col.g + col.r / 4.0f);

  return Color((byte)col.r, (byte)col.g, (byte)col.b);
}

// ============================================================================
// Voronoi Shader
// ============================================================================

Color VoronoiShader(float u, float v, float time) {
  // Convert UV back to pixel coordinates for the Voronoi utils
  int x = static_cast<int>(u * DISPLAY_WIDTH);
  int y = static_cast<int>(v * DISPLAY_HEIGHT);

  // Use the moved Voronoi utility
  byte val = Voronoi::PixelShader(x, y, time);

  // Return grayscale color
  return Color(val, val, val);
}

// ============================================================================
// Grid Shader
// ============================================================================

static float Grid(glm::vec2 uv, float battery, float time) {
  // Thicker lines for lower resolution (128x32)
  // Was 0.01f, increased to 0.025f
  glm::vec2 size = glm::vec2(uv.y, uv.y * uv.y * 0.2f) * 0.025f;

  // Scale UVs to reduce grid density (make cells larger)
  uv *= 0.2f;

  // Reduced speed from 4.0f to 0.5f to match the zoomed-in scale
  // Negated time to reverse direction (lines move towards center/horizon)
  uv += glm::vec2(0.0f, -time * 0.5f * (battery + 0.05f));
  uv = glm::abs(glm::fract(uv) - 0.5f);
  glm::vec2 lines = glm::smoothstep(size, glm::vec2(0.0f), uv);
  lines += glm::smoothstep(size * 5.0f, glm::vec2(0.0f), uv) * 0.4f * battery;
  return glm::clamp(lines.x + lines.y, 0.0f, 3.0f);
}

Color GridShader(float u, float v, float time) {
  glm::vec2 resolution(DISPLAY_WIDTH, DISPLAY_HEIGHT);
  glm::vec2 fragCoord(u * DISPLAY_WIDTH, v * DISPLAY_HEIGHT);

  glm::vec2 uv = (2.0f * fragCoord - resolution) / resolution.y;

  // Fade to center logic (Y-axis): Darken center, brighten edges
  // Center of screen (y=0) is dark, edges are bright
  // Adjusted to 0.2->0.8 to create a wider dark band in the center
  float fade = glm::smoothstep(0.2f, 0.8f, std::abs(uv.y));

  float battery = 1.0f;

  // Adjusted fog to be symmetric around 0.0 (horizon)
  float fog = glm::smoothstep(0.1f, -0.02f, std::abs(uv.y));
  glm::vec3 col = glm::vec3(0.0f, 0.1f, 0.2f);

  // Render grid if we are far enough from the center horizon
  if (std::abs(uv.y) > 0.05f) {
    // Use abs(uv.y) for symmetric perspective projection on top and bottom
    float projectedY = 3.0f / (std::abs(uv.y) + 0.05f);

    glm::vec2 gridUV = uv;
    gridUV.y = projectedY;
    gridUV.x *= gridUV.y * 1.0f;

    float gridVal = Grid(gridUV, battery, time);
    col = glm::mix(col, glm::vec3(1.0f, 0.5f, 1.0f), gridVal);
  }

  col += fog * fog * fog;
  col = glm::mix(glm::vec3(col.r, col.r, col.r) * 0.5f, col, battery * 0.7f);

  // Apply the fade
  col *= fade;

  return Color((byte)glm::clamp(col.r * 255.0f, 0.0f, 255.0f),
               (byte)glm::clamp(col.g * 255.0f, 0.0f, 255.0f),
               (byte)glm::clamp(col.b * 255.0f, 0.0f, 255.0f));
}
