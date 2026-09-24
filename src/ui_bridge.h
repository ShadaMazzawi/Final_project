#pragma once
#include "MiniFB.h"
extern "C" {
#include "microui.h"
}
#include <cstdint>
#include <cstring>

// Forwards MiniFB's per-frame window/input state into a microui context.
// One instance per window; call on_char() from MiniFB's char-input callback
// and feed() once per frame before mu_begin().
class InputBridge {
public:
    void on_char(unsigned int codepoint) {
        if (codepoint < 0x80 && text_len_ < static_cast<int>(sizeof(text_buf_)) - 1) {
            text_buf_[text_len_++] = static_cast<char>(codepoint);
            text_buf_[text_len_] = '\0';
        }
    }

    void feed(mu_Context* ctx, mfb_window* window) {
        if (text_len_ > 0) {
            mu_input_text(ctx, text_buf_);
            text_len_ = 0;
            text_buf_[0] = '\0';
        }

        int mx = mfb_get_mouse_x(window);
        int my = mfb_get_mouse_y(window);
        mu_input_mousemove(ctx, mx, my);

        const uint8_t* mouse = mfb_get_mouse_button_buffer(window);
        feed_mouse_button(ctx, mouse, MFB_MOUSE_LEFT, MU_MOUSE_LEFT, mx, my);
        feed_mouse_button(ctx, mouse, MFB_MOUSE_RIGHT, MU_MOUSE_RIGHT, mx, my);
        feed_mouse_button(ctx, mouse, MFB_MOUSE_MIDDLE, MU_MOUSE_MIDDLE, mx, my);

        float scroll = mfb_get_mouse_scroll_y(window);
        if (scroll != 0.0f) {
            mu_input_scroll(ctx, 0, static_cast<int>(scroll * -10.0f));
        }

        const uint8_t* keys = mfb_get_key_buffer(window);
        feed_key(ctx, keys, MFB_KB_KEY_LEFT_SHIFT, MU_KEY_SHIFT);
        feed_key(ctx, keys, MFB_KB_KEY_RIGHT_SHIFT, MU_KEY_SHIFT);
        feed_key(ctx, keys, MFB_KB_KEY_LEFT_CONTROL, MU_KEY_CTRL);
        feed_key(ctx, keys, MFB_KB_KEY_RIGHT_CONTROL, MU_KEY_CTRL);
        feed_key(ctx, keys, MFB_KB_KEY_LEFT_ALT, MU_KEY_ALT);
        feed_key(ctx, keys, MFB_KB_KEY_RIGHT_ALT, MU_KEY_ALT);
        feed_key(ctx, keys, MFB_KB_KEY_ENTER, MU_KEY_RETURN);
        feed_key(ctx, keys, MFB_KB_KEY_KP_ENTER, MU_KEY_RETURN);
        feed_key(ctx, keys, MFB_KB_KEY_BACKSPACE, MU_KEY_BACKSPACE);
    }

private:
    void feed_mouse_button(mu_Context* ctx, const uint8_t* buttons, int mfb_btn, int mu_btn, int mx, int my) {
        bool down = buttons[mfb_btn] != 0;
        if (down && !prev_mouse_[mfb_btn]) mu_input_mousedown(ctx, mx, my, mu_btn);
        if (!down && prev_mouse_[mfb_btn]) mu_input_mouseup(ctx, mx, my, mu_btn);
        prev_mouse_[mfb_btn] = down;
    }

    void feed_key(mu_Context* ctx, const uint8_t* keys, int mfb_key, int mu_key) {
        bool down = keys[mfb_key] != 0;
        if (down && !prev_keys_[mfb_key]) mu_input_keydown(ctx, mu_key);
        if (!down && prev_keys_[mfb_key]) mu_input_keyup(ctx, mu_key);
        prev_keys_[mfb_key] = down;
    }

    char text_buf_[32] = {};
    int text_len_ = 0;
    bool prev_mouse_[8] = {};
    bool prev_keys_[MFB_KB_KEY_LAST + 1] = {};
};