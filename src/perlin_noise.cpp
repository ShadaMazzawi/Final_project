#include "perlin_noise.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

PerlinNoise::PerlinNoise(uint32_t seed) {
    int permutation[256];
    std::iota(permutation, permutation + 256, 0);

    std::mt19937 rng(seed);
    std::shuffle(permutation, permutation + 256, rng);

    for (int i = 0; i < 512; ++i) {
        perm_[i] = permutation[i & 255];
    }
}

float PerlinNoise::fade(float t) {
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

float PerlinNoise::lerp(float a, float b, float t) {
    return a + t * (b - a);
}

float PerlinNoise::gradient(int hash, float x, float y) {
    switch (hash & 7) {
        case 0: return  x + y;
        case 1: return  x - y;
        case 2: return -x + y;
        case 3: return -x - y;
        case 4: return  x;
        case 5: return -x;
        case 6: return  y;
        default: return -y;
    }
}

float PerlinNoise::noise(float x, float y) const {
    float xf = x - std::floor(x);
    float yf = y - std::floor(y);
    int xi = static_cast<int>(std::floor(x)) & 255;
    int yi = static_cast<int>(std::floor(y)) & 255;

    float u = fade(xf);
    float v = fade(yf);

    int aa = perm_[perm_[xi] + yi];
    int ab = perm_[perm_[xi] + yi + 1];
    int ba = perm_[perm_[xi + 1] + yi];
    int bb = perm_[perm_[xi + 1] + yi + 1];

    float x1 = lerp(gradient(aa, xf, yf), gradient(ba, xf - 1.0f, yf), u);
    float x2 = lerp(gradient(ab, xf, yf - 1.0f), gradient(bb, xf - 1.0f, yf - 1.0f), u);

    return lerp(x1, x2, v);
}

float PerlinNoise::fractal(float x, float y, int octaves, float persistence, float lacunarity) const {
    float total = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float amplitude_sum = 0.0f;

    for (int i = 0; i < octaves; ++i) {
        total += noise(x * frequency, y * frequency) * amplitude;
        amplitude_sum += amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }

    return amplitude_sum > 0.0f ? total / amplitude_sum : 0.0f;
}