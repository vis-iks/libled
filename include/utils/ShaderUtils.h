#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat2x2.hpp>

namespace libled {

// ============================================================================
// Noise Functions
// ============================================================================

/**
 * Hash function for 2D vectors
 * Generates pseudo-random values from 2D input
 */
glm::vec2 NoiseHash(glm::vec2 p);

/**
 * 2D Simplex Noise
 * Returns noise value in range approximately [-1, 1]
 */
float Noise(glm::vec2 p);

/**
 * Fractional Brownian Motion (FBM)
 * Layers multiple octaves of noise for more natural-looking results
 * 
 * @param st Input coordinates
 * @param octaves Number of noise layers (default: 5)
 * @return Noise value in range [0, 1]
 */
float FractionalBrownianMotion(glm::vec2 st, int octaves = 5);

// ============================================================================
// Matrix Operations
// ============================================================================

/**
 * Create a 2D rotation matrix
 * @param angle Rotation angle in radians
 */
glm::mat2 Rotate2D(float angle);

// ============================================================================
// Shader Utility Functions
// ============================================================================

/**
 * Linear interpolation between two floats
 */
float Mix(float a, float b, float t);

/**
 * Linear interpolation between two vec2
 */
glm::vec2 Mix(const glm::vec2& a, const glm::vec2& b, float t);

/**
 * Linear interpolation between two vec3
 */
glm::vec3 Mix(const glm::vec3& a, const glm::vec3& b, float t);

/**
 * Smooth Hermite interpolation
 * Returns 0 when x <= edge0, 1 when x >= edge1, smooth between
 */
float SmoothStep(float edge0, float edge1, float x);

/**
 * Clamp value between min and max
 */
float Clamp(float x, float minVal, float maxVal);

/**
 * Clamp vec3 components between min and max
 */
glm::vec3 Clamp(const glm::vec3& v, float minVal, float maxVal);

} // namespace libled
