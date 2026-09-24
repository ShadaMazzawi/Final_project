#include "texture_presets.h"

#include <algorithm>
#include <cmath>
#include <cstddef>

namespace {

struct RGB {
    uint8_t r, g, b;
};

RGB lerp_rgb(RGB a, RGB b, float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return RGB{
        static_cast<uint8_t>(a.r + (b.r - a.r) * t),
        static_cast<uint8_t>(a.g + (b.g - a.g) * t),
        static_cast<uint8_t>(a.b + (b.b - a.b) * t),
    };
}

uint32_t pack_rgb(RGB c) {
    return 0xFF000000u | (static_cast<uint32_t>(c.r) << 16) |
           (static_cast<uint32_t>(c.g) << 8) | static_cast<uint32_t>(c.b);
}

// Fractal noise output is roughly [-1, 1] in theory, but in practice the
// amplitude-normalized fBm sum rarely exceeds about +-0.5, so a gain is
// applied here to actually use the full [0, 1] shading range.
float normalize(float n) {
    constexpr float kContrastGain = 2.0f;
    return std::clamp(n * kContrastGain * 0.5f + 0.5f, 0.0f, 1.0f);
}

RGB shade_clouds(float n) {
    constexpr RGB sky{74, 144, 217};
    constexpr RGB cloud{255, 255, 255};
    float t = normalize(n);
    t = t * t * (3.0f - 2.0f * t);  // smoothstep: keeps mid-tones mostly sky or cloud
    return lerp_rgb(sky, cloud, t);
}

RGB shade_marble(float n, float x, float y) {
    constexpr RGB vein{40, 40, 46};
    constexpr RGB base{232, 226, 216};
    float stripes = std::sin((x + y) * 6.0f + n * 12.0f);
    return lerp_rgb(vein, base, normalize(stripes));
}

RGB shade_fire(float n) {
    struct Stop {
        float pos;
        RGB color;
    };
    static constexpr Stop stops[] = {
        {0.00f, RGB{10, 0, 0}},
        {0.35f, RGB{120, 10, 0}},
        {0.60f, RGB{220, 90, 0}},
        {0.82f, RGB{255, 190, 40}},
        {1.00f, RGB{255, 250, 210}},
    };
    float t = std::pow(normalize(n), 1.4f);  // bias dark so bright flame cores stay sparse
    for (size_t i = 1; i < std::size(stops); ++i) {
        if (t <= stops[i].pos) {
            float local = (t - stops[i - 1].pos) / (stops[i].pos - stops[i - 1].pos);
            return lerp_rgb(stops[i - 1].color, stops[i].color, local);
        }
    }
    return stops[std::size(stops) - 1].color;
}

RGB shade_terrain(float n) {
    struct Band {
        float pos;
        RGB color;
    };
    static constexpr Band bands[] = {
        {0.00f, RGB{15, 40, 90}},    // deep water
        {0.38f, RGB{40, 95, 175}},   // shallow water
        {0.44f, RGB{215, 200, 140}}, // sand
        {0.50f, RGB{70, 140, 60}},   // grass
        {0.72f, RGB{110, 100, 95}},  // rock
        {0.88f, RGB{250, 250, 250}}, // snow
        {1.00f, RGB{255, 255, 255}},
    };
    float t = normalize(n);
    for (size_t i = 1; i < std::size(bands); ++i) {
        if (t <= bands[i].pos) {
            float local = (t - bands[i - 1].pos) / (bands[i].pos - bands[i - 1].pos);
            return lerp_rgb(bands[i - 1].color, bands[i].color, local);
        }
    }
    return bands[std::size(bands) - 1].color;
}

}  // namespace

const char* preset_name(Preset preset) {
    switch (preset) {
        case Preset::Clouds:  return "Clouds";
        case Preset::Marble:  return "Marble";
        case Preset::Fire:    return "Fire";
        case Preset::Terrain: return "Terrain";
    }
    return "Unknown";
}

void generate_texture(std::vector<uint32_t>& pixels, int width, int height,
                       const PerlinNoise& noise, const NoiseParams& params) {
    pixels.resize(static_cast<size_t>(width) * height);

    for (int y = 0; y < height; ++y) {
        float ny = (static_cast<float>(y) / height) * params.scale;
        for (int x = 0; x < width; ++x) {
            float nx = (static_cast<float>(x) / width) * params.scale;
            float n = noise.fractal(nx, ny, params.octaves, params.persistence, params.lacunarity);

            RGB color;
            switch (params.preset) {
                case Preset::Clouds:  color = shade_clouds(n); break;
                case Preset::Marble:  color = shade_marble(n, nx, ny); break;
                case Preset::Fire:    color = shade_fire(n); break;
                case Preset::Terrain: color = shade_terrain(n); break;
            }
            pixels[static_cast<size_t>(y) * width + x] = pack_rgb(color);
        }
    }
}