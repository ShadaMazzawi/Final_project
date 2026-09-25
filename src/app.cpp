#include "app.h"

#include "MiniFB.h"
extern "C" {
#include "microui.h"
}
#include "perlin_noise.h"
#include "texture_presets.h"
#include "ui_bridge.h"
#include "ui_renderer.h"

#include <algorithm>
#include <cstdio>
#include <memory>
#include <random>
#include <vector>

namespace {

constexpr int kWindowWidth = 900;
constexpr int kWindowHeight = 600;
constexpr int kCanvasSize = 580;
constexpr int kCanvasX = 10;
constexpr int kCanvasY = 10;
constexpr int kPanelX = kCanvasX + kCanvasSize + 10;
constexpr uint32_t kBackground = 0xFF202020u;

InputBridge* g_active_bridge = nullptr;

void on_char_input(struct mfb_window*, unsigned int codepoint) {
    if (g_active_bridge) g_active_bridge->on_char(codepoint);
}

void blit(std::vector<uint32_t>& dst, int dst_w, const std::vector<uint32_t>& src, int src_w, int src_h,
          int x0, int y0) {
    for (int y = 0; y < src_h; ++y) {
        std::copy(src.begin() + static_cast<size_t>(y) * src_w,
                   src.begin() + static_cast<size_t>(y) * src_w + src_w,
                   dst.begin() + static_cast<size_t>(y0 + y) * dst_w + x0);
    }
}

// Builds the control panel for this frame; returns true if any parameter changed.
bool build_ui(mu_Context* ctx, NoiseParams& params) {
    bool changed = false;
    int panel_w = kWindowWidth - kPanelX;
    mu_begin_window_ex(ctx, "Controls", mu_rect(kPanelX, 0, panel_w, kWindowHeight),
                        MU_OPT_NOCLOSE | MU_OPT_NORESIZE | MU_OPT_NOTITLE);

    int full_row[] = {-1};
    mu_layout_row(ctx, 1, full_row, 0);
    mu_label(ctx, "PROCEDURAL TEXTURE");

    char current[32];
    std::snprintf(current, sizeof(current), "Preset: %s", preset_name(params.preset));
    mu_label(ctx, current);
    mu_label(ctx, "");

    static const Preset kPresets[] = {Preset::Clouds, Preset::Marble, Preset::Fire, Preset::Terrain};
    mu_Color base_button = ctx->style->colors[MU_COLOR_BUTTON];
    mu_Color base_hover = ctx->style->colors[MU_COLOR_BUTTONHOVER];
    for (Preset p : kPresets) {
        bool active = (p == params.preset);
        if (active) {
            ctx->style->colors[MU_COLOR_BUTTON] = mu_color(66, 133, 199, 255);
            ctx->style->colors[MU_COLOR_BUTTONHOVER] = mu_color(84, 151, 217, 255);
        }
        if (mu_button(ctx, preset_name(p)) && params.preset != p) {
            params.preset = p;
            changed = true;
        }
        if (active) {
            ctx->style->colors[MU_COLOR_BUTTON] = base_button;
            ctx->style->colors[MU_COLOR_BUTTONHOVER] = base_hover;
        }
    }

    mu_label(ctx, "");

    mu_label(ctx, "Scale");
    if (mu_slider_ex(ctx, &params.scale, 0.5f, 12.0f, 0, "%.2f", MU_OPT_ALIGNCENTER) & MU_RES_CHANGE) {
        changed = true;
    }

    mu_label(ctx, "Octaves");
    float octaves_f = static_cast<float>(params.octaves);
    if (mu_slider_ex(ctx, &octaves_f, 1.0f, 8.0f, 1.0f, "%.0f", MU_OPT_ALIGNCENTER) & MU_RES_CHANGE) {
        params.octaves = static_cast<int>(octaves_f);
        changed = true;
    }

    mu_label(ctx, "Persistence");
    if (mu_slider_ex(ctx, &params.persistence, 0.1f, 0.9f, 0, "%.2f", MU_OPT_ALIGNCENTER) & MU_RES_CHANGE) {
        changed = true;
    }

    mu_label(ctx, "Lacunarity");
    if (mu_slider_ex(ctx, &params.lacunarity, 1.0f, 3.5f, 0, "%.2f", MU_OPT_ALIGNCENTER) & MU_RES_CHANGE) {
        changed = true;
    }

    mu_label(ctx, "");
    char seed_label[32];
    std::snprintf(seed_label, sizeof(seed_label), "Seed: %u", params.seed);
    mu_label(ctx, seed_label);
    if (mu_button(ctx, "Randomize Seed")) {
        static std::random_device rd;
        params.seed = rd();
        changed = true;
    }

    mu_end_window(ctx);
    return changed;
}

}  // namespace

int run_interactive_app() {
    struct mfb_window* window = mfb_open("Procedural Texture Generator", kWindowWidth, kWindowHeight);
    if (!window) {
        std::fprintf(stderr, "Failed to open a window (no display available?)\n");
        return 1;
    }

    std::vector<uint32_t> framebuffer(static_cast<size_t>(kWindowWidth) * kWindowHeight, kBackground);
    std::vector<uint32_t> texture(static_cast<size_t>(kCanvasSize) * kCanvasSize, 0);

    auto ctx = std::make_unique<mu_Context>();
    mu_init(ctx.get());
    ctx->text_width = mu_text_width;
    ctx->text_height = mu_text_height;

    UiRenderer renderer(kWindowWidth, kWindowHeight);

    InputBridge input;
    g_active_bridge = &input;
    mfb_set_char_input_callback(window, on_char_input);

    NoiseParams params;
    bool regenerate = true;

    do {
        input.feed(ctx.get(), window);

        mu_begin(ctx.get());
        if (build_ui(ctx.get(), params)) regenerate = true;
        mu_end(ctx.get());

        if (regenerate) {
            PerlinNoise noise(params.seed);
            generate_texture(texture, kCanvasSize, kCanvasSize, noise, params);
            regenerate = false;
        }

        std::fill(framebuffer.begin(), framebuffer.end(), kBackground);
        blit(framebuffer, kWindowWidth, texture, kCanvasSize, kCanvasSize, kCanvasX, kCanvasY);
        renderer.render(ctx.get(), framebuffer.data());

        mfb_update_state state = mfb_update(window, framebuffer.data());
        if (state != MFB_STATE_OK) break;
    } while (mfb_wait_sync(window));

    g_active_bridge = nullptr;
    return 0;
}