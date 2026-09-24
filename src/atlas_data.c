#include "atlas_data.h"
#include "atlas.inl"

const unsigned char* mu_atlas_texture(void) { return atlas_texture; }
const mu_Rect* mu_atlas_rects(void) { return atlas; }
int mu_atlas_width(void) { return ATLAS_WIDTH; }
int mu_atlas_height(void) { return ATLAS_HEIGHT; }
int mu_atlas_font_base(void) { return ATLAS_FONT; }