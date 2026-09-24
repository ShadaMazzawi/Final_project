#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "app.h"
#include "perlin_noise.h"
#include "texture_presets.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace {

std::vector<uint8_t> to_rgb_bytes(const std::vector<uint32_t>& pixels) {
    std::vector<uint8_t> out(pixels.size() * 3);
    for (size_t i = 0; i < pixels.size(); ++i) {
        out[i * 3 + 0] = static_cast<uint8_t>((pixels[i] >> 16) & 0xFF);
        out[i * 3 + 1] = static_cast<uint8_t>((pixels[i] >> 8) & 0xFF);
        out[i * 3 + 2] = static_cast<uint8_t>(pixels[i] & 0xFF);
    }
    return out;
}

void export_preset(const std::string& dir, const char* filename, const NoiseParams& params, int size) {
    PerlinNoise noise(params.seed);
    std::vector<uint32_t> pixels;
    generate_texture(pixels, size, size, noise, params);

    std::vector<uint8_t> rgb = to_rgb_bytes(pixels);
    std::string path = dir + "/" + filename;
    stbi_write_png(path.c_str(), size, size, 3, rgb.data(), size * 3);
    std::printf("wrote %s\n", path.c_str());
}

int run_export(const std::string& dir) {
    const int size = 512;

    NoiseParams clouds;
    clouds.preset = Preset::Clouds;
    clouds.scale = 4.0f;
    clouds.octaves = 5;
    clouds.persistence = 0.5f;
    clouds.lacunarity = 2.0f;
    clouds.seed = 7;
    export_preset(dir, "clouds_scale4_octaves5.png", clouds, size);

    NoiseParams marble;
    marble.preset = Preset::Marble;
    marble.scale = 3.0f;
    marble.octaves = 4;
    marble.persistence = 0.55f;
    marble.lacunarity = 2.2f;
    marble.seed = 42;
    export_preset(dir, "marble_veins_default.png", marble, size);

    NoiseParams fire;
    fire.preset = Preset::Fire;
    fire.scale = 5.0f;
    fire.octaves = 6;
    fire.persistence = 0.6f;
    fire.lacunarity = 2.0f;
    fire.seed = 99;
    export_preset(dir, "fire_high_octaves.png", fire, size);

    NoiseParams terrain;
    terrain.preset = Preset::Terrain;
    terrain.scale = 4.0f;
    terrain.octaves = 6;
    terrain.persistence = 0.5f;
    terrain.lacunarity = 2.0f;
    terrain.seed = 5;
    export_preset(dir, "terrain_default_seed5.png", terrain, size);

    NoiseParams terrain_alt_seed = terrain;
    terrain_alt_seed.seed = 123;
    export_preset(dir, "terrain_seed123.png", terrain_alt_seed, size);

    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    if (argc >= 3 && std::strcmp(argv[1], "--export") == 0) {
        return run_export(argv[2]);
    }
    return run_interactive_app();
}