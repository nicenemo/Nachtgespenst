#include <M5Unified.h>
#include "Theme.h"

ThemeMode ThemeManager::_currentMode = ThemeMode::DARK;

void ThemeManager::setTheme(ThemeMode mode) { _currentMode = mode; }

Theme ThemeManager::getTheme() {
    if (_currentMode == ThemeMode::LIGHT) {
        return { WHITE, BLUE, LIGHTGREY, 0xE71C, BLACK, DARKGREY };
    }
    return { BLACK, BLUE, DARKGREY, 0x2104, WHITE, LIGHTGREY };
}

uint16_t ThemeManager::getKeyColor(const Binding& b, bool getBackground) {
    Theme t = getTheme();
    
    // Default for 'none' keys
    if (b.usage.id == 0 && b.modifier == 0) {
        return getBackground ? t.defaultKeyBg : DARKGREY;
    }

    if (getBackground) {
        uint16_t base;
        switch (b.usage.kind) {
            case HID::UsageKind::CA:  base = M5.Lcd.color565(30, 30, 150); break; // App
            case HID::UsageKind::CP:  base = M5.Lcd.color565(150, 30, 30); break; // Media
            case HID::UsageKind::LC:  base = M5.Lcd.color565(30, 150, 30); break; // Logic
            default:                  base = t.defaultKeyBg; break;
        }

        // Modifier Combinations: Alter background brightness/tint
        if (b.modifier != 0) {
            if (b.modifier & 0x11) base = M5.Lcd.color565(100, 100, 0);   // Ctrl -> Olive
            if (b.modifier & 0x44) base = M5.Lcd.color565(100, 0, 100);   // Alt -> Purple
            if (b.modifier & 0x88) base = M5.Lcd.color565(0, 100, 100);   // Gui -> Teal
        }
        return base;
    } else {
        // Foreground Text Color
        if (b.modifier & 0x22) return YELLOW; // Shifted keys always show yellow text
        return t.defaultKeyFg;
    }
}
