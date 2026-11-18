#pragma once
#include <GridLayer.h>

// ==========================================
// FULL INLINE LAYER DEFINITIONS (26 GRIDS)
// ==========================================

const GridLayer LAYERS[] = {
    // 1. Maven Tools (3x3)
    {
        "Maven Tools",
        (const Binding[]){
            lcs(kp_B),  lcs(kp_Y),  lcs(kp_U), 
            none,       none,       none,    
            lcs(kp_I),  lcs(kp_Q),  lcs(kp_E)
        },
        3, 3, LAYER_COLORS[0]
    },

    // 2. Office & Productivity (4x3)
    {
        "Office/Prod",
        (const Binding[]){
            lhyp(kp_W), lhyp(kp_X), lhyp(kp_P),
            lhyp(kp_O), lhyp(kp_T), lhyp(kp_N),
            lhyp(kp_B), lhyp(kp_E), lhyp(kp_Y),
            lhyp(kp_L), none,       none
        },
        4, 3, LAYER_COLORS[1]
    },

    // 3. Edit & Navigation (5x3)
    {
        "Edit/Nav",
        (const Binding[]){
            kp_INSERT,     kp_HOME,       kp_PAGEUP,
            kp_DELETE_FORWARD,     kp_END,        kp_PAGEDOWN,
            lcs(kp_C),     kp_UPARROW,   lcs(kp_V),
            kp_LEFTARROW, kp_DOWNARROW, kp_RIGHTARROW,
            lcs(kp_Z),     lcs(kp_X),     lcs(kp_A)
        },
        5, 3, LAYER_COLORS[2]
    },

    // 4. System & Edit (4x3)
    {
        "System",
        (const Binding[]){
            kp_ESCAPE,       kp_APPLICATION,  kp_PRINTSCREEN, 
            lct(kp_Z),       lct(kp_X),       lct(kp_C),
            lct(kp_V),       lct(kp_F),       kp_SCROLL_LOCK,
            kp_PAUSE,        gp_SYS_POWER_DOWN, kp_KP_NUM_LOCK_AND_CLEAR
        },
        4, 3, LAYER_COLORS[3]
    },

    // 5. F1-F10 Standard (4x3)
    {
        "F1-F10",
        (const Binding[]){
            kp_F7,     kp_F8,     kp_F9,
            kp_F4,     kp_F5,     kp_F6,
            kp_F1,     kp_F2,     kp_F3,
            kp_ESCAPE, kp_F10,    kp_RETURN_ENTER
        },
        4, 3, LAYER_COLORS[4]
    },

    // 6. F1-F10 MEH (4x3)
    {
        "F1-F10 MEH",
        (const Binding[]){
            lmeh(kp_F7), lmeh(kp_F8), lmeh(kp_F9),
            lmeh(kp_F4), lmeh(kp_F5), lmeh(kp_F6),
            lmeh(kp_F1), lmeh(kp_F2), lmeh(kp_F3),
            kp_ESCAPE,   lmeh(kp_F10),kp_RETURN_ENTER
        },
        4, 3, LAYER_COLORS[5]
    },

    // 7. F1-F10 HYPER (4x3)
    {
        "F1-F10 HYP",
        (const Binding[]){
            lhyp(kp_F7), lhyp(kp_F8), lhyp(kp_F9),
            lhyp(kp_F4), lhyp(kp_F5), lhyp(kp_F6),
            lhyp(kp_F1), lhyp(kp_F2), lhyp(kp_F3),
            kp_ESCAPE,   lhyp(kp_F10),kp_RETURN_ENTER
        },
        4, 3, LAYER_COLORS[6]
    },

    // 8. F11-F20 Standard (4x3)
    {
        "F11-F20",
        (const Binding[]){
            kp_F17,    kp_F18,    kp_F19,
            kp_F14,    kp_F15,    kp_F16,
            kp_F11,    kp_F12,    kp_F13,
            kp_ESCAPE, kp_F20,    kp_RETURN_ENTER
        },
        4, 3, LAYER_COLORS[7]
    },

    // 9. F11-F20 MEH (4x3)
    {
        "F11-F20 MEH",
        (const Binding[]){
            lmeh(kp_F17), lmeh(kp_F18), lmeh(kp_F19),
            lmeh(kp_F14), lmeh(kp_F15), lmeh(kp_F16),
            lmeh(kp_F11), lmeh(kp_F12), lmeh(kp_F13),
            kp_ESCAPE,    lmeh(kp_F20),kp_RETURN_ENTER
        },
        4, 3, LAYER_COLORS[8]
    },

    // 10. F11-F20 HYP (4x3)
    {
        "F11-F20 HYP",
        (const Binding[]){
            lhyp(kp_F17), lhyp(kp_F18), lhyp(kp_F19),
            lhyp(kp_F14), lhyp(kp_F15), lhyp(kp_F16),
            lhyp(kp_F11), lhyp(kp_F12), lhyp(kp_F13),
            kp_ESCAPE,    lhyp(kp_F20),kp_RETURN_ENTER
        },
        4, 3, LAYER_COLORS[9]
    },

    // 11. F21-F24 Standard (2x2)
    {
        "F21-F24",
        (const Binding[]){
            kp_F23, kp_F24, 
            kp_F21, kp_F22 
        },
        2, 2, LAYER_COLORS[10]
    },

    // 12. F21-F24 MEH (2x2)
    {
        "F21-F24 MEH",
        (const Binding[]){
            lmeh(kp_F23), lmeh(kp_F24), 
            lmeh(kp_F21), lmeh(kp_F22) 
        },
        2, 2, LAYER_COLORS[11]
    },

    // 13. F21-F24 HYP (2x2)
    {
        "F21-F24 HYP",
        (const Binding[]){
            lhyp(kp_F23), lhyp(kp_F24), 
            lhyp(kp_F21), lhyp(kp_F22) 
        },
        2, 2, LAYER_COLORS[12]
    },

    // 14. Symbol Pad (6x4)
    {
        "Symbol Pad",
        (const Binding[]){
            kp_ID_7_AND_AMPERSAND,        kp_ID_8_AND_STAR,        kp_ID_9_AND_LEFT_BRACKET,        lsf(kp_ID_1_AND_BANG),
            kp_ID_4_AND_DOLLAR,        kp_ID_5_AND_PERCENT,        kp_ID_6_AND_CARET,        lsf(kp_ID_2_AND_AT),
            kp_ID_1_AND_BANG,        kp_ID_2_AND_AT,        kp_ID_3_AND_HASH,        lsf(kp_ID_3_AND_HASH),
            kp_ID_0_AND_RIGHT_BRACKET,        kp_PERIOD_AND_GREATERTHAN,      kp_COMMA_AND_LESSTHAN,    lsf(kp_ID_4_AND_DOLLAR),
            lsf(kp_ID_9_AND_LEFT_BRACKET),   lsf(kp_ID_0_AND_RIGHT_BRACKET),   kp_DASH_AND_UNDERSCORE,    lsf(kp_EQUALS_AND_PLUS),
            kp_ESCAPE,   kp_TAB,      kp_DELETE, kp_RETURN_ENTER
        },
        6, 4, LAYER_COLORS[13]
    },

    // 15. Numpad Full (6x4)
    {
        "Numpad Full",
        (const Binding[]){
            kp_KP_NUM_LOCK_AND_CLEAR, kp_KP_FORWARDSLASH, kp_KP_STAR, kp_KP_DASH,
            kp_KP_7_AND_HOME,     kp_KP_8_AND_UP_ARROW,     kp_KP_9_AND_PAGEUP,        kp_KP_PLUS,
            kp_KP_4_AND_LEFT_ARROW,     kp_KP_5,     kp_KP_6_AND_RIGHT_ARROW,        kp_KP_PERIOD_AND_DELETE,
            kp_KP_1_AND_END,     kp_KP_2_AND_DOWN_ARROW,     kp_KP_3_AND_PAGEDN,        kp_KP_ENTER,
            kp_KP_0_AND_INSERT,     none,        none,           kp_KP_COMMA,
            kp_ESCAPE,   kp_TAB,      kp_DELETE,   kp_RETURN_ENTER
        },
        6, 4, LAYER_COLORS[14]
    },

    // 16. International (3x3)
    {
        "International",
        (const Binding[]){
            kp_INTERNATIONAL7, kp_INTERNATIONAL8, kp_INTERNATIONAL9,
            kp_INTERNATIONAL4, kp_INTERNATIONAL5, kp_INTERNATIONAL6,
            kp_INTERNATIONAL1, kp_INTERNATIONAL2, kp_INTERNATIONAL3
        },
        3, 3, LAYER_COLORS[15]
    },

    // 17. Language & Specials (4x3)
    {
        "Language",
        (const Binding[]){
            kp_LANG1,      kp_LANG2,      kp_LANG3,
            kp_LANG4,      kp_LANG5,      kp_EXECUTE,
            kp_APPLICATION, none,           kp_CAPS_LOCK,
            kp_KP_NUM_LOCK_AND_CLEAR,    kp_SCROLL_LOCK, kp_RETURN_ENTER
        },
        4, 3, LAYER_COLORS[16]
    },

    // 18. Hard Locks (1x3)
    {
        "Hard Locks",
        (const Binding[]){
            kp_CAPS_LOCK, kp_KP_NUM_LOCK_AND_CLEAR, kp_SCROLL_LOCK
        },
        1, 3, LAYER_COLORS[17]
    },

    // 19. Media Control (4x3)
    {
        "Media",
        (const Binding[]){
            cp_SCAN_PREVIOUS_TRACK, cp_PLAY_PAUSE, cp_SCAN_NEXT_TRACK,
            cp_VOLUME_DECREMENT,    cp_MUTE,       cp_VOLUME_INCREMENT,
            cp_STOP,                cp_EMAIL_READER, cp_LOCAL_MACHINE_BROWSER,
            gp_SYS_POWER_DOWN,      gp_SYS_SLEEP,  gp_SYS_WAKE_UP
        },
        4, 3, LAYER_COLORS[18]
    },

    // 20. MS Teams (4x3)
    {
        "MS Teams",
        (const Binding[]){
            lcs(kp_M),  lcs(kp_O),  lcs(kp_D),
            none,       kp_UPARROW, none,
            kp_LEFTARROW, kp_DOWNARROW, kp_RIGHTARROW,
            kp_ESCAPE,  kp_RETURN_ENTER,   kp_DELETE
        },
        4, 3, LAYER_COLORS[19]
    },

    // 21. MS Outlook (4x3)
    {
        "MS Outlook",
        (const Binding[]){
            lcs(kp_N),  lcs(kp_R),  lcs(kp_F),
            none,       kp_UPARROW, none,
            kp_LEFTARROW, kp_DOWNARROW, kp_RIGHTARROW,
            kp_DELETE_FORWARD,  kp_RETURN_ENTER,   kp_DELETE
        },
        4, 3, LAYER_COLORS[20]
    },

    // 22. PowerPoint (4x3)
    {
        "PowerPoint",
        (const Binding[]){
            kp_F5,       lsf(kp_F5), kp_ESCAPE,
            none,        kp_UPARROW, kp_PAGEUP,
            kp_LEFTARROW, kp_DOWNARROW, kp_RIGHTARROW,
            kp_HOME,     kp_PAGEDOWN, kp_END
        },
        4, 3, LAYER_COLORS[21]
    },

    // 23. Browser Control (4x3)
    {
        "Browser Ctrl",
        (const Binding[]){
            kp_F5,       lcs(kp_R),  lcs(kp_T),
            kp_F7,       kp_UPARROW, lcs(kp_W), 
            kp_LEFTARROW, kp_DOWNARROW, kp_RIGHTARROW,
            kp_HOME,     kp_F11,     kp_END
        },
        4, 3, LAYER_COLORS[22]
    },

    // 24. Debug IDE (4x3)
    {
        "Debug IDE",
        (const Binding[]){
            kp_F5,       kp_F9,      lcs(kp_F5),
            kp_F10,      kp_F11,     lsf(kp_F11),
            none,        kp_UPARROW, none,
            kp_LEFTARROW, kp_DOWNARROW, kp_RIGHTARROW
        },
        4, 3, LAYER_COLORS[23]
    },

    // 25. App Launch (4x3)
    {
        "App Launch",
        (const Binding[]){
            cp_INTERNET_BROWSER, cp_LOCAL_MACHINE_BROWSER, cp_EMAIL_READER,
            cp_CALENDAR_SCHEDULE,    cp_PROCESS_TASK_MANAGER,  cp_CALCULATOR,
            none,              none,                none,
            kp_ESCAPE,         kp_RETURN_ENTER,            kp_DELETE
        },
        4, 3, LAYER_COLORS[24]
    },

    // 26. Edit Keys (3x3)
    {
        "Edit Keys",
        (const Binding[]){
            lct(kp_F),   lct(kp_Z),  none,
            lct(kp_X),   lct(kp_C),  lct(kp_V),
            kp_ESCAPE,   kp_RETURN_ENTER,   kp_DELETE
        },
        3, 3, LAYER_COLORS[25]
    }
};

const uint8_t LAYER_COUNT = sizeof(LAYERS) / sizeof(GridLayer);
