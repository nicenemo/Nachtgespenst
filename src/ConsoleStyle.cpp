#include <Arduino.h>
#include "ConsoleStyle.h"

extern "C" const uint8_t ng_geist_bold_ttf[];
extern "C" const size_t ng_geist_bold_ttf_len;
extern "C" const uint8_t ng_geist_mono_ttf[];
extern "C" const size_t ng_geist_mono_ttf_len;

namespace Console {
namespace {
constexpr int SANS_PX[4] = {12, 16, 20, 28};
constexpr int MONO_PX[2] = {12, 14};
lv_font_t* g_sans[4];
lv_font_t* g_mono[2];

const lv_font_t* montserrat(int px) {
    return px <= 12 ? &lv_font_montserrat_12 : px <= 14 ? &lv_font_montserrat_14 : px <= 16 ? &lv_font_montserrat_16 : px <= 20 ? &lv_font_montserrat_20 : &lv_font_montserrat_28;
}
}

void begin() {
    for (int i = 0; i < 4; i++) {
        g_sans[i] = lv_tiny_ttf_create_data(ng_geist_bold_ttf, ng_geist_bold_ttf_len, SANS_PX[i]);
        if (g_sans[i]) g_sans[i]->fallback = montserrat(SANS_PX[i]);
    }
    for (int i = 0; i < 2; i++) {
        g_mono[i] = lv_tiny_ttf_create_data(ng_geist_mono_ttf, ng_geist_mono_ttf_len, MONO_PX[i]);
        if (g_mono[i]) g_mono[i]->fallback = montserrat(MONO_PX[i]);
    }
}

const lv_font_t* sans(int px) {
    int k = px <= 12 ? 0 : px <= 16 ? 1 : px <= 20 ? 2 : 3;
    return g_sans[k] ? g_sans[k] : montserrat(SANS_PX[k]);
}

const lv_font_t* mono(int px) {
    int k = px <= 12 ? 0 : 1;
    return g_mono[k] ? g_mono[k] : montserrat(MONO_PX[k]);
}

void block(lv_obj_t* o, uint32_t colour) {
    lv_obj_set_style_bg_color(o, lv_color_hex(colour), 0);
    lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(o, RADIUS, 0);
    lv_obj_set_style_border_width(o, 0, 0);
    lv_obj_set_style_shadow_width(o, 0, 0);
    lv_obj_set_style_text_color(o, lv_color_hex(INK), 0);
    lv_obj_set_style_bg_color(o, lv_color_hex(AMBER), LV_STATE_PRESSED);          // pressed: the block turns amber (touch and the buttons A, B, C alike)
    lv_obj_set_style_text_color(o, lv_color_hex(INK), LV_STATE_PRESSED);
    lv_obj_set_style_outline_width(o, 2, LV_STATE_FOCUS_KEY);                     // focus: a peach ring
    lv_obj_set_style_outline_color(o, lv_color_hex(PEACH), LV_STATE_FOCUS_KEY);
}

void pill(lv_obj_t* o, uint32_t colour) {
    block(o, colour);
    lv_obj_set_style_radius(o, LV_RADIUS_CIRCLE, 0);
}
}
