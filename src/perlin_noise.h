#pragma once
#include <cstdint>

// Classic (Ken Perlin, 2002) gradient noise over a 2D domain, plus fractal
// Brownian motion built from repeated octaves of it.
class PerlinNoise {
public:
    explicit PerlinNoise(uint32_t seed);

    // Single octave of gradient noise, in roughly [-1, 1].
    float noise(float x, float y) const;

    // Sums `octaves` layers of noise() at increasing frequency and decreasing
    // amplitude, normalized back to roughly [-1, 1].
    float fractal(float x, float y, int octaves, float persistence, float lacunarity) const;

private:
    static float fade(float t);
    static float lerp(float a, float b, float t);
    static float gradient(int hash, float x, float y);

    int perm_[512];
};