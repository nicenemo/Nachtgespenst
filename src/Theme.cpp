#include <M5Unified.h>
#include "Theme.h"

ThemeMode ThemeManager::_currentMode = ThemeMode::DARK;

void ThemeManager::setTheme(ThemeMode mode) { _currentMode = mode; }

Theme ThemeManager::getTheme() {
    if (_currentMode == ThemeMode::LIGHT) {
        return { WHITE, BLUE, LIGHTGREY, 0xE71C, BLACK, DARKGREY };
    }
    return { BLACK, BLUE, DARKGREY, 0x2948, WHITE, LIGHTGREY };
}

// The colours of the keys come from the console palette (docs/wiki/design-system.md): amber, peach, lavender and coral, with black text.
// Plain keys are peach; a chord with Ctrl is amber, with Alt lavender, with Win coral; apps are lavender, media keys coral.
uint16_t ThemeManager::getKeyColor(const Binding& b, bool getBackground) {
    Theme t = getTheme();
    if (b.usage.id == 0 && b.modifier == 0) return getBackground ? t.defaultKeyBg : DARKGREY;   // empty tile: a dim block
    if (!getBackground) return BLACK;
    const uint16_t AMBER = M5.Lcd.color565(255, 153, 0), PEACH = M5.Lcd.color565(255, 204, 153), LAVENDER = M5.Lcd.color565(153, 153, 255), CORAL = M5.Lcd.color565(204, 102, 102);
    uint16_t base;
    switch (b.usage.kind) {
        case HID::UsageKind::CA: base = LAVENDER; break;   // applications
        case HID::UsageKind::CP: base = CORAL; break;      // media
        default:                 base = PEACH; break;      // plain keys and logic
    }
    if (b.modifier & 0x11) base = AMBER;                   // Ctrl
    if (b.modifier & 0x44) base = LAVENDER;                // Alt
    if (b.modifier & 0x88) base = CORAL;                   // Win
    return base;
}
