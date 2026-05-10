#pragma once
#include "Glyphs.h"
#include "Colors.h"

// Helper macro to clean up the matrix layout
#define K_BIND(GLYPH, COLOR, PAYLOAD) { GLYPH, COLOR, PAYLOAD }

const Binding DECK[9][9] = {
    // LAYER 0: SYSTEM & SECURITY
    {
        K_BIND(K_PC,    K_COLOR_ACTION,   "konsole\n"),              K_BIND(K_LOCK,  K_COLOR_BKA,      "loginctl lock-session\n"), K_BIND(K_GEAR,  K_COLOR_NEUTRAL,  "kcalc\n"),
        K_BIND(K_FOLD,  K_COLOR_ACTION,   "dolphin\n"),              K_BIND(K_FIRE,  K_COLOR_WARNING,  "librewolf\n"),             K_BIND(K_TRASH, K_COLOR_BKA,      "rm -rf ~/.cache/*\n"),
        K_BIND(K_BUG,   K_COLOR_CRITICAL, "dmesg -w\n"),             K_BIND(K_CLOUD, K_COLOR_ACTION,   "ssh root@server\n"),       K_BIND(K_SKULL, K_COLOR_CRITICAL, "killall -9 ")
    },

    // LAYER 1: DEVELOPMENT
    {
        K_BIND(K_SNAKE,  K_COLOR_SUCCESS,  "python3\n"),             K_BIND(K_COFFEE, K_COLOR_WARNING,  "java -version\n"),         K_BIND(K_CRAB,   K_COLOR_CRITICAL, "cargo run\n"),
        K_BIND(K_ROCKET, K_COLOR_ACTION,   "npm start\n"),           K_BIND(K_HAMMER, K_COLOR_NEUTRAL,  "make\n"),                  K_BIND(K_WRENCH, K_COLOR_NEUTRAL,  "cmake\n"),
        K_BIND(K_LIGHT,  K_COLOR_SUCCESS,  "nvim\n"),                K_BIND(K_LINK,   K_COLOR_ACTION,   "ln -s "),                  K_BIND(K_DB,     K_COLOR_ACTION,   "sqlite3 ")
    },

    // LAYER 2: GIT OPERATIONS
    {
        K_BIND(K_LEAF,   K_COLOR_SUCCESS,  "git status\n"),          K_BIND(K_PLUS,   K_COLOR_ACTION,   "git add .\n"),             K_BIND(K_MINUS,  K_COLOR_CRITICAL, "git reset --hard\n"),
        K_BIND(K_UP,     K_COLOR_ACTION,   "git push\n"),            K_BIND(K_DOWN,   K_COLOR_ACTION,   "git pull\n"),              K_BIND(K_LEFT,   K_COLOR_NEUTRAL,  "git checkout -\n"),
        K_BIND(K_RIGHT,  K_COLOR_NEUTRAL,  "git merge "),            K_BIND(K_CHECK,  K_COLOR_SUCCESS,  "git commit\n"),            K_BIND(K_CROSS,  K_COLOR_CRITICAL, "git branch -D ")
    },

    // LAYER 3: CONTAINERS & K8S
    {
        K_BIND(K_WHALE,  K_COLOR_ACTION,   "docker ps\n"),           K_BIND(K_BOX,    K_COLOR_NEUTRAL,  "docker-compose up\n"),     K_BIND(K_SHIP,   K_COLOR_ACTION,   "kubectl get pods\n"),
        K_BIND(K_ANCHOR, K_COLOR_BKA,      "docker stop "),          K_BIND(K_WAVE,   K_COLOR_ACTION,   "docker network ls\n"),     K_BIND(K_FISH,   K_COLOR_SUCCESS,  "docker logs "),
        K_BIND(K_NET,    K_COLOR_NEUTRAL,  "ufw status\n"),          K_BIND(K_HOOK,   K_COLOR_WARNING,  "git hook "),               K_BIND(K_COMPASS,K_COLOR_ACTION,   "explore ")
    },

    // LAYER 4: MEDIA CONTROL
    {
        K_BIND(K_VOL_U,  K_COLOR_ACTION,   ""),                      K_BIND(K_VOL_D,  K_COLOR_ACTION,   ""),                        K_BIND(K_MUTE,   K_COLOR_CRITICAL, ""),
        K_BIND(K_PLAY,   K_COLOR_SUCCESS,  ""),                      K_BIND(K_PAUSE,  K_COLOR_WARNING,  ""),                        K_BIND(K_NEXT,   K_COLOR_ACTION,   ""),
        K_BIND(K_PREV,   K_COLOR_ACTION,   ""),                      K_BIND(K_SUN,    K_COLOR_WHITE,    ""),                        K_BIND(K_MOON,   K_COLOR_NEUTRAL,  "")
    },

    // LAYER 5: WEB & NAVIGATION
    {
        K_BIND(K_WEB,    K_COLOR_ACTION,   ""),                      K_BIND(K_TAB,    K_COLOR_NEUTRAL,  ""),                        K_BIND(K_NEW,    K_COLOR_SUCCESS,  ""),
        K_BIND(K_CLOSE,  K_COLOR_CRITICAL, ""),                      K_BIND(K_RELOAD, K_COLOR_WARNING,  ""),                        K_BIND(K_DEV,    K_COLOR_ACTION,   ""),
        K_BIND(K_SEARCH, K_COLOR_NEUTRAL,  ""),                      K_BIND(K_STAR,   K_COLOR_WARNING,  ""),                        K_BIND(K_HOME,   K_COLOR_ACTION,   "")
    },

    // LAYER 6: PRODUCTIVITY
    {
        K_BIND(K_DOC,    K_COLOR_WHITE,    ""),                      K_BIND(K_SHEET,  K_COLOR_SUCCESS,  ""),                        K_BIND(K_SLIDE,  K_COLOR_WARNING,  ""),
        K_BIND(K_MAIL,   K_COLOR_ACTION,   ""),                      K_BIND(K_CAL,    K_COLOR_CRITICAL, ""),                        K_BIND(K_USER,   K_COLOR_NEUTRAL,  ""),
        K_BIND(K_SAVE,   K_COLOR_SUCCESS,  ""),                      K_BIND(K_PRINT,  K_COLOR_NEUTRAL,  ""),                        K_BIND(K_PDF,    K_COLOR_CRITICAL, "")
    },

    // LAYER 7: WINDOW MANAGEMENT
    {
        K_BIND(K_WIN_L,  K_COLOR_ACTION,   ""),                      K_BIND(K_WIN_R,  K_COLOR_ACTION,   ""),                        K_BIND(K_WIN_U,  K_COLOR_ACTION,   ""),
        K_BIND(K_WIN_D,  K_COLOR_ACTION,   ""),                      K_BIND(K_WIN_C,  K_COLOR_NEUTRAL,  ""),                        K_BIND(K_WIN_M,  K_COLOR_NEUTRAL,  ""),
        K_BIND(K_DESK_L, K_COLOR_BKA,      ""),                      K_BIND(K_DESK_R, K_COLOR_BKA,      ""),                        K_BIND(K_DESK_O, K_COLOR_BKA,      "")
    },

    // LAYER 8: HARDWARE & POWER
    {
        K_BIND(K_PWR_S,  K_COLOR_NEUTRAL,  ""),                      K_BIND(K_PWR_H,  K_COLOR_ACTION,   ""),                        K_BIND(K_PWR_L,  K_COLOR_CRITICAL, ""),
        K_BIND(K_PWR_R,  K_COLOR_CRITICAL, ""),                      K_BIND(K_PWR_O,  K_COLOR_CRITICAL, ""),                        K_BIND(K_DISP_O, K_COLOR_BKA,      ""),
        K_BIND(K_FAN_U,  K_COLOR_ACTION,   ""),                      K_BIND(K_FAN_D,  K_COLOR_ACTION,   ""),                        K_BIND(K_BATT,   K_COLOR_SUCCESS,  "")
    }
};
