#pragma once
#include <Arduino.h>
#include "Bindings.h"
#include "DisplayColors.h"

enum class ThemeMode { DARK, LIGHT };

struct Theme {
    uint16_t background;
    uint16_t header;
    uint16_t gridLines;
    uint16_t defaultKeyBg;
    uint16_t defaultKeyFg;
    uint16_t overlayText;
};

class ThemeManager {
public:
    static void setTheme(ThemeMode mode);
    static Theme getTheme();
    static uint16_t getKeyColor(const Binding& b, bool getBackground);
    
private:
    static ThemeMode _currentMode;
};
