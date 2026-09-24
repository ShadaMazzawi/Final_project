#pragma once
#include <cstdint>
#include <vector>

#include "perlin_noise.h"

enum class Preset {
    Clouds,
    Marble,
    Fire,
    Terrain,
};

constexpr int kPresetCount = 4;
const char* preset_name(Preset preset);

struct NoiseParams {
    Preset preset = Preset::Clouds;
    float scale = 4.0f;
    int octaves = 5;
    float persistence = 0.5f;
    float lacunarity = 2.0f;
    uint32_t seed = 1337;
};

// Fills `pixels` (row-major, 0xAARRGGBB) with a width*height texture shaped by
// `params` and sampled from `noise`.
void generate_texture(std::vector<uint32_t>& pixels, int width, int height,
                       const PerlinNoise& noise, const NoiseParams& params);