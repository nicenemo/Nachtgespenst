#pragma once
#include <stdint.h>

namespace HID::Lighting {
    // De Usage Page ID voor Lighting and Illumination
    constexpr uint8_t PAGE_ID = 0x59;

    // ==========================================
    // LAMP ARRAY ATTRIBUTES (Host -> Device)
    // De host vraagt hiermee wat de mogelijkheden zijn
    // ==========================================
    constexpr uint8_t LAMP_ARRAY_ATTRIBUTES        = 0x02; // Aantal lampen, fysieke afmetingen
    constexpr uint8_t LAMP_ATTRIBUTES_REQUEST      = 0x03; // Host vraagt info over specifieke LED
    constexpr uint8_t LAMP_ATTRIBUTES_RESPONSE     = 0x04; // Device stuurt positie (XYZ) en kleurtype

    // ==========================================
    // LIGHTING CONTROL (Updates)
    // Hoe de kleuren daadwerkelijk worden verzonden
    // ==========================================
    constexpr uint8_t LAMP_MULTI_UPDATE            = 0x05; // Update meerdere LED's tegelijk (snel)
    constexpr uint8_t LAMP_RANGE_UPDATE            = 0x06; // Update een reeks LED's (bijv. 1 t/m 10)
    constexpr uint8_t LAMP_ARRAY_CONTROL           = 0x07; // Master control (bijv. autonoom vs host-gestuurd)

    // ==========================================
    // LAMP TYPES (Usage 0x20 range)
    // Geeft aan de host door wat voor soort verlichting het is
    // ==========================================
    enum LampType : uint8_t {
        KEYBOARD           = 0x01,
        MOUSE              = 0x02,
        AMBIENT            = 0x03, // Achtergrondverlichting
        RFU                = 0x04, // Reserved
        GRAPHICS_CARD      = 0x05,
        CHASSIS            = 0x06,
        MONITOR            = 0x07,
        UNDERGLOW          = 0x08,
        PAD                = 0x09, // Keypads / Stream Decks
        FURNITURE          = 0x0A
    };

    // ==========================================
    // COLOR ATTRIBUTES
    // Definieert hoe de kleuren worden opgebouwd
    // ==========================================
    namespace Color {
        constexpr uint8_t RED_LEVEL   = 0x21;
        constexpr uint8_t GREEN_LEVEL = 0x22;
        constexpr uint8_t BLUE_LEVEL  = 0x23;
        constexpr uint8_t INTENSITY   = 0x24; // Globale helderheid per LED
        
        // Voor "Theater" / Pro-gear kan dit ook White of Amber zijn
        constexpr uint8_t WHITE_LEVEL = 0x25;
    }

    // ==========================================
    // PROGRAMMABLE EFFECTS (Autonome effecten)
    // Als de host de controle teruggeeft aan het device
    // ==========================================
    namespace Effects {
        constexpr uint8_t EFFECT_START    = 0x50;
        constexpr uint8_t EFFECT_STOP     = 0x51;
        
        enum Preset : uint8_t {
            STATIC        = 0x01,
            BREATHING     = 0x02,
            WAVE          = 0x03,
            SPECTRUM      = 0x04,
            RAIN          = 0x05
        };
    }

    // ==========================================
    // BITMASKERS VOOR LAMP_ARRAY_CONTROL
    // ==========================================
    enum ControlMask : uint8_t {
        AUTONOMOUS_MODE = 0x01 // 1: Device bepaalt effect, 0: Host bepaalt kleur
    };
}
