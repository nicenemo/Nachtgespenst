#pragma once

// --- Layer 0: System ---
#define K_PC      "💻"
#define K_LOCK    "🔒"
#define K_GEAR    "⚙"
#define K_FOLD    "📁"
#define K_FIRE    "🔥"
#define K_TRASH   "🗑"
#define K_BUG     "🐞"
#define K_CLOUD   "☁"
#define K_SKULL   "💀"

// --- Layer 1: Dev ---
#define K_SNAKE   "🐍"
#define K_COFFEE  "☕"
#define K_CRAB    "🦀"
#define K_ROCKET  "🚀"
#define K_HAMMER  "🔨"
#define K_WRENCH  "🔧"
#define K_LIGHT   "💡"
#define K_LINK    "🔗"
#define K_DB      "🗄"

// --- Layer 2: Git ---
#define K_LEAF    "🌿"
#define K_PLUS    "➕"
#define K_MINUS   "➖"
#define K_UP      "⬆"
#define K_DOWN    "⬇"
#define K_LEFT    "⬅"
#define K_RIGHT   "➡"
#define K_CHECK   "✅"
#define K_CROSS   "❌"

// --- Layer 3: Containers ---
#define K_WHALE   "🐳"
#define K_BOX     "📦"
#define K_SHIP    "🚢"
#define K_ANCHOR  "⚓"
#define K_WAVE    "🌊"
#define K_FISH    "🐟"
#define K_NET     "🕸"
#define K_HOOK    "🪝"
#define K_COMPASS "🧭"

// --- Layer 4: Media ---
#define K_VOL_U   "🔊"
#define K_VOL_D   "🔉"
#define K_MUTE    "🔇"
#define K_PLAY    "▶"
#define K_PAUSE   "⏸"
#define K_NEXT    "⏭"
#define K_PREV    "⏮"
#define K_SUN     "☀"
#define K_MOON    "🌙"

// --- Layer 5: Web ---
#define K_WEB     "🌐"
#define K_TAB     "🗂"
#define K_NEW     "✨"
#define K_CLOSE   "⛔"
#define K_RELOAD  "🔄"
#define K_DEV     "👨‍💻"
#define K_SEARCH  "🔍"
#define K_STAR    "⭐"
#define K_HOME    "🏠"

// --- Layer 6: Office ---
#define K_DOC     "📄"
#define K_SHEET   "📊"
#define K_SLIDE   "📝"
#define K_MAIL    "📧"
#define K_CAL     "📅"
#define K_USER    "👤"
#define K_SAVE    "💾"
#define K_PRINT   "🖨"
#define K_PDF     "📑"

// --- Layer 7: Windows ---
#define K_WIN_L   "↼"
#define K_WIN_R   "⇀"
#define K_WIN_U   "↿"
#define K_WIN_D   "↾"
#define K_WIN_C   "▣"
#define K_WIN_M   "▢"
#define K_DESK_L  "⏪"
#define K_DESK_R  "⏩"
#define K_DESK_O  "⏫"

// --- Layer 8: Power ---
#define K_PWR_S   "💤"
#define K_PWR_H   "❄"
#define K_PWR_L   "🔑"
#define K_PWR_R   "🔃"
#define K_PWR_O   "🛑"
#define K_DISP_O  "🖥"
#define K_FAN_U   "🌬"
#define K_FAN_D   "💨"
#define K_BATT    "🔋"

struct Binding { 
    const char* glyph; 
    uint16_t color; 
    const char* payload; 
};
