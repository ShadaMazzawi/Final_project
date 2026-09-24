#pragma once
extern "C" {
#include "microui.h"
}
#include <cstdint>

// Draws a microui context's command list onto a raw 0xAARRGGBB pixel buffer,
// using microui's own bundled font/icon atlas (demo/atlas.inl).
class UiRenderer {
public:
    UiRenderer(int width, int height);
    void render(mu_Context* ctx, uint32_t* pixels);

private:
    void draw_rect(mu_Rect rect, mu_Color color);
    void draw_text(const char* text, mu_Vec2 pos, mu_Color color);
    void draw_icon(int id, mu_Rect rect, mu_Color color);
    void blend_atlas_glyph(mu_Rect atlas_rect, int dst_x, int dst_y, mu_Color color);
    void set_pixel(int x, int y, uint32_t argb);

    uint32_t* pixels_ = nullptr;
    int width_ = 0;
    int height_ = 0;
    mu_Rect clip_{};
};

// microui text-measurement callbacks, backed by the same atlas.
int mu_text_width(mu_Font font, const char* text, int len);
int mu_text_height(mu_Font font);