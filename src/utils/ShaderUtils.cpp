#include "utils/ShaderUtils.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat2x2.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <glm/common.hpp>
#include <cmath>

namespace libled {

// ============================================================================
// Noise Functions
// ============================================================================

glm::vec2 NoiseHash(glm::vec2 p)
{
    p = glm::vec2(glm::dot(p, glm::vec2(127.1f, 311.7f)),
        glm::dot(p, glm::vec2(269.5f, 183.3f)));
    return -1.0f + 2.0f * glm::fract(glm::sin(p) * 43758.5453123f);
}

float Noise(glm::vec2 p)
{
    const float K1 = 0.366025404f;
    const float K2 = 0.211324865f;
    glm::vec2 i = glm::floor(p + (p.x + p.y) * K1);
    glm::vec2 a = p - i + (i.x + i.y) * K2;
    glm::vec2 o = glm::step(glm::vec2(a.y, a.x), a);
    glm::vec2 b = a - o + K2;
    glm::vec2 c = a - 1.0f + 2.0f * K2;
    glm::vec3 h = glm::max(0.5f - glm::vec3(glm::dot(a, a), glm::dot(b, b), glm::dot(c, c)), 0.0f);
    glm::vec3 n = h * h * h * h * glm::vec3(glm::dot(a, NoiseHash(i + 0.0f)), glm::dot(b, NoiseHash(i + o)), glm::dot(c, NoiseHash(i + 1.0f)));
    return glm::dot(n, glm::vec3(70.0f));
}

float FractionalBrownianMotion(glm::vec2 st, int octaves)
{
    float value = 0.0f;
    float amplitude = 0.5f;
    for (int i = 0; i < octaves; ++i) {
        value += amplitude * Noise(st);
        st *= 2.0f;
        amplitude *= 0.5f;
    }
    return value * 0.5f + 0.5f;
}

// ============================================================================
// Matrix Operations
// ============================================================================

glm::mat2 Rotate2D(float angle)
{
    float c = std::cos(angle);
    float s = std::sin(angle);
    return glm::mat2(c, s, -s, c);
}

// ============================================================================
// Shader Utility Functions
// ============================================================================

float Mix(float a, float b, float t)
{
    return a + (b - a) * t;
}

glm::vec2 Mix(const glm::vec2& a, const glm::vec2& b, float t)
{
    return glm::mix(a, b, t);
}

glm::vec3 Mix(const glm::vec3& a, const glm::vec3& b, float t)
{
    return glm::mix(a, b, t);
}

float SmoothStep(float edge0, float edge1, float x)
{
    float t = Clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

float Clamp(float x, float minVal, float maxVal)
{
    return glm::clamp(x, minVal, maxVal);
}

glm::vec3 Clamp(const glm::vec3& v, float minVal, float maxVal)
{
    return glm::clamp(v, minVal, maxVal);
}

} // namespace libled
