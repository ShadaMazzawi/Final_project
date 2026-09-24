#pragma once
// Accessors for microui's bundled default font/icon atlas (demo/atlas.inl).
// That file uses C99 designated array initializers GCC's C++ front end
// rejects, so it's compiled once as plain C (atlas_data.c) and exposed here.
#include "microui.h"

#ifdef __cplusplus
extern "C" {
#endif

const unsigned char* mu_atlas_texture(void);
const mu_Rect* mu_atlas_rects(void);
int mu_atlas_width(void);
int mu_atlas_height(void);
int mu_atlas_font_base(void);

#ifdef __cplusplus
}
#endif