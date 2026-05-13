#pragma once
#include <Arduino.h>
#include "Bindings.h"
#include "LedColors.h"

/**
 * GridLayer holds the Title, the inline Binding array, 
 * dimensions, and the color reference from LedColors.h.
 */
struct GridLayer {
    const char* title;
    const Binding* tiles;
    uint8_t rows;
    uint8_t cols;
    CRGB color;
};
