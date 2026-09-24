#include "ui_renderer.h"
#include "atlas_data.h"

#include <algorithm>
#include <cstring>

namespace {

struct Atlas {
    const unsigned char* texture = mu_atlas_texture();
    const mu_Rect* rects = mu_atlas_rects();
    int width = mu_atlas_width();
    int height = mu_atlas_height();
    int font_base = mu_atlas_font_base();
};

const Atlas& atlas() {
    static Atlas instance;
    return instance;
}

uint32_t pack_argb(mu_Color c) {
    return (static_cast<uint32_t>(c.a) << 24) | (static_cast<uint32_t>(c.r) << 16) |
           (static_cast<uint32_t>(c.g) << 8) | static_cast<uint32_t>(c.b);
}

uint32_t blend(uint32_t dst, uint32_t src_rgb, uint8_t alpha) {
    if (alpha == 0) return dst;
    if (alpha == 255) return src_rgb | 0xFF000000u;
    uint32_t sr = (src_rgb >> 16) & 0xFF, sg = (src_rgb >> 8) & 0xFF, sb = src_rgb & 0xFF;
    uint32_t dr = (dst >> 16) & 0xFF, dg = (dst >> 8) & 0xFF, db = dst & 0xFF;
    uint32_t r = (sr * alpha + dr * (255 - alpha)) / 255;
    uint32_t g = (sg * alpha + dg * (255 - alpha)) / 255;
    uint32_t b = (sb * alpha + db * (255 - alpha)) / 255;
    return 0xFF000000u | (r << 16) | (g << 8) | b;
}

}  // namespace

UiRenderer::UiRenderer(int width, int height) : width_(width), height_(height) {
    clip_ = {0, 0, width, height};
}

void UiRenderer::set_pixel(int x, int y, uint32_t argb) {
    if (x < clip_.x || x >= clip_.x + clip_.w) return;
    if (y < clip_.y || y >= clip_.y + clip_.h) return;
    if (x < 0 || x >= width_ || y < 0 || y >= height_) return;
    pixels_[static_cast<size_t>(y) * width_ + x] = argb;
}

void UiRenderer::draw_rect(mu_Rect rect, mu_Color color) {
    uint32_t c = pack_argb(color);
    int x2 = rect.x + rect.w, y2 = rect.y + rect.h;
    for (int y = rect.y; y < y2; ++y) {
        for (int x = rect.x; x < x2; ++x) {
            set_pixel(x, y, c);
        }
    }
}

void UiRenderer::blend_atlas_glyph(mu_Rect src, int dst_x, int dst_y, mu_Color color) {
    const Atlas& a = atlas();
    uint32_t src_rgb = pack_argb(color);
    for (int y = 0; y < src.h; ++y) {
        for (int x = 0; x < src.w; ++x) {
            int ax = src.x + x, ay = src.y + y;
            if (ax < 0 || ax >= a.width || ay < 0 || ay >= a.height) continue;
            uint8_t alpha_src = a.texture[ay * a.width + ax];
            if (alpha_src == 0) continue;
            uint8_t alpha = static_cast<uint8_t>((alpha_src * color.a) / 255);
            int px = dst_x + x, py = dst_y + y;
            if (px < clip_.x || px >= clip_.x + clip_.w) continue;
            if (py < clip_.y || py >= clip_.y + clip_.h) continue;
            if (px < 0 || px >= width_ || py < 0 || py >= height_) continue;
            size_t idx = static_cast<size_t>(py) * width_ + px;
            pixels_[idx] = blend(pixels_[idx], src_rgb, alpha);
        }
    }
}

void UiRenderer::draw_text(const char* text, mu_Vec2 pos, mu_Color color) {
    const Atlas& a = atlas();
    int x = pos.x;
    for (const unsigned char* p = reinterpret_cast<const unsigned char*>(text); *p; ++p) {
        int chr = std::min<int>(*p, 127);
        mu_Rect src = a.rects[a.font_base + chr];
        blend_atlas_glyph(src, x, pos.y, color);
        x += src.w;
    }
}

void UiRenderer::draw_icon(int id, mu_Rect rect, mu_Color color) {
    mu_Rect src = atlas().rects[id];
    int x = rect.x + (rect.w - src.w) / 2;
    int y = rect.y + (rect.h - src.h) / 2;
    blend_atlas_glyph(src, x, y, color);
}

void UiRenderer::render(mu_Context* ctx, uint32_t* pixels) {
    pixels_ = pixels;
    mu_Command* cmd = nullptr;
    while (mu_next_command(ctx, &cmd)) {
        switch (cmd->type) {
            case MU_COMMAND_RECT: draw_rect(cmd->rect.rect, cmd->rect.color); break;
            case MU_COMMAND_TEXT: draw_text(cmd->text.str, cmd->text.pos, cmd->text.color); break;
            case MU_COMMAND_ICON: draw_icon(cmd->icon.id, cmd->icon.rect, cmd->icon.color); break;
            case MU_COMMAND_CLIP: clip_ = cmd->clip.rect; break;
        }
    }
}

int mu_text_width(mu_Font, const char* text, int len) {
    if (len < 0) len = static_cast<int>(std::strlen(text));
    const Atlas& a = atlas();
    int width = 0;
    for (int i = 0; i < len; ++i) {
        int chr = std::min<int>(static_cast<unsigned char>(text[i]), 127);
        width += a.rects[a.font_base + chr].w;
    }
    return width;
}

int mu_text_height(mu_Font) {
    return 18;
}