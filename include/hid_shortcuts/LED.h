#pragma once
/**
 * Shortcuts for LED
 */
#ifndef le_NUM_LOCK
#define le_NUM_LOCK Binding(HID::Page::LED, HID::LED::OTHER::NUM_LOCK)
#endif
#ifndef le_CAPS_LOCK
#define le_CAPS_LOCK Binding(HID::Page::LED, HID::LED::OTHER::CAPS_LOCK)
#endif
#ifndef le_SCROLL_LOCK
#define le_SCROLL_LOCK Binding(HID::Page::LED, HID::LED::OTHER::SCROLL_LOCK)
#endif
#ifndef le_COMPOSE
#define le_COMPOSE Binding(HID::Page::LED, HID::LED::OTHER::COMPOSE)
#endif
#ifndef le_KANA
#define le_KANA Binding(HID::Page::LED, HID::LED::OTHER::KANA)
#endif
#ifndef le_POWER
#define le_POWER Binding(HID::Page::LED, HID::LED::OTHER::POWER)
#endif
#ifndef le_SHIFT
#define le_SHIFT Binding(HID::Page::LED, HID::LED::OTHER::SHIFT)
#endif
#ifndef le_DO_NOT_DISTURB
#define le_DO_NOT_DISTURB Binding(HID::Page::LED, HID::LED::OTHER::DO_NOT_DISTURB)
#endif
#ifndef le_MUTE
#define le_MUTE Binding(HID::Page::LED, HID::LED::OTHER::MUTE)
#endif
#ifndef le_TONE_ENABLE
#define le_TONE_ENABLE Binding(HID::Page::LED, HID::LED::OTHER::TONE_ENABLE)
#endif
#ifndef le_HIGH_CUT_FILTER
#define le_HIGH_CUT_FILTER Binding(HID::Page::LED, HID::LED::OTHER::HIGH_CUT_FILTER)
#endif
#ifndef le_LOW_CUT_FILTER
#define le_LOW_CUT_FILTER Binding(HID::Page::LED, HID::LED::OTHER::LOW_CUT_FILTER)
#endif
#ifndef le_EQUALIZER_ENABLE
#define le_EQUALIZER_ENABLE Binding(HID::Page::LED, HID::LED::OTHER::EQUALIZER_ENABLE)
#endif
#ifndef le_SOUND_FIELD_ON
#define le_SOUND_FIELD_ON Binding(HID::Page::LED, HID::LED::OTHER::SOUND_FIELD_ON)
#endif
#ifndef le_SURROUND_ON
#define le_SURROUND_ON Binding(HID::Page::LED, HID::LED::OTHER::SURROUND_ON)
#endif
#ifndef le_REPEAT
#define le_REPEAT Binding(HID::Page::LED, HID::LED::OTHER::REPEAT)
#endif
#ifndef le_STEREO
#define le_STEREO Binding(HID::Page::LED, HID::LED::OTHER::STEREO)
#endif
#ifndef le_SAMPLING_RATE_DETECT
#define le_SAMPLING_RATE_DETECT Binding(HID::Page::LED, HID::LED::OTHER::SAMPLING_RATE_DETECT)
#endif
#ifndef le_SPINNING
#define le_SPINNING Binding(HID::Page::LED, HID::LED::OTHER::SPINNING)
#endif
#ifndef le_CAV
#define le_CAV Binding(HID::Page::LED, HID::LED::OTHER::CAV)
#endif
#ifndef le_CLV
#define le_CLV Binding(HID::Page::LED, HID::LED::OTHER::CLV)
#endif
#ifndef le_RECORDING_FORMAT_DETECT
#define le_RECORDING_FORMAT_DETECT Binding(HID::Page::LED, HID::LED::OTHER::RECORDING_FORMAT_DETECT)
#endif
#ifndef le_OFF_HOOK
#define le_OFF_HOOK Binding(HID::Page::LED, HID::LED::OTHER::OFF_HOOK)
#endif
#ifndef le_RING
#define le_RING Binding(HID::Page::LED, HID::LED::OTHER::RING)
#endif
#ifndef le_MESSAGE_WAITING
#define le_MESSAGE_WAITING Binding(HID::Page::LED, HID::LED::OTHER::MESSAGE_WAITING)
#endif
#ifndef le_DATA_MODE
#define le_DATA_MODE Binding(HID::Page::LED, HID::LED::OTHER::DATA_MODE)
#endif
#ifndef le_BATTERY_OPERATION
#define le_BATTERY_OPERATION Binding(HID::Page::LED, HID::LED::OTHER::BATTERY_OPERATION)
#endif
#ifndef le_BATTERY_OK
#define le_BATTERY_OK Binding(HID::Page::LED, HID::LED::OTHER::BATTERY_OK)
#endif
#ifndef le_BATTERY_LOW
#define le_BATTERY_LOW Binding(HID::Page::LED, HID::LED::OTHER::BATTERY_LOW)
#endif
#ifndef le_SPEAKER
#define le_SPEAKER Binding(HID::Page::LED, HID::LED::OTHER::SPEAKER)
#endif
#ifndef le_HEADSET
#define le_HEADSET Binding(HID::Page::LED, HID::LED::OTHER::HEADSET)
#endif
#ifndef le_HOLD
#define le_HOLD Binding(HID::Page::LED, HID::LED::OTHER::HOLD)
#endif
#ifndef le_MICROPHONE
#define le_MICROPHONE Binding(HID::Page::LED, HID::LED::OTHER::MICROPHONE)
#endif
#ifndef le_COVERAGE
#define le_COVERAGE Binding(HID::Page::LED, HID::LED::OTHER::COVERAGE)
#endif
#ifndef le_NIGHT_MODE
#define le_NIGHT_MODE Binding(HID::Page::LED, HID::LED::OTHER::NIGHT_MODE)
#endif
#ifndef le_SEND_CALLS
#define le_SEND_CALLS Binding(HID::Page::LED, HID::LED::OTHER::SEND_CALLS)
#endif
#ifndef le_CALL_PICKUP
#define le_CALL_PICKUP Binding(HID::Page::LED, HID::LED::OTHER::CALL_PICKUP)
#endif
#ifndef le_CONFERENCE
#define le_CONFERENCE Binding(HID::Page::LED, HID::LED::OTHER::CONFERENCE)
#endif
#ifndef le_STAND_BY
#define le_STAND_BY Binding(HID::Page::LED, HID::LED::OTHER::STAND_BY)
#endif
#ifndef le_CAMERA_ON
#define le_CAMERA_ON Binding(HID::Page::LED, HID::LED::OTHER::CAMERA_ON)
#endif
#ifndef le_CAMERA_OFF
#define le_CAMERA_OFF Binding(HID::Page::LED, HID::LED::OTHER::CAMERA_OFF)
#endif
#ifndef le_ON_LINE
#define le_ON_LINE Binding(HID::Page::LED, HID::LED::OTHER::ON_LINE)
#endif
#ifndef le_OFF_LINE
#define le_OFF_LINE Binding(HID::Page::LED, HID::LED::OTHER::OFF_LINE)
#endif
#ifndef le_BUSY
#define le_BUSY Binding(HID::Page::LED, HID::LED::OTHER::BUSY)
#endif
#ifndef le_READY
#define le_READY Binding(HID::Page::LED, HID::LED::OTHER::READY)
#endif
#ifndef le_PAPER_OUT
#define le_PAPER_OUT Binding(HID::Page::LED, HID::LED::OTHER::PAPER_OUT)
#endif
#ifndef le_PAPER_JAM
#define le_PAPER_JAM Binding(HID::Page::LED, HID::LED::OTHER::PAPER_JAM)
#endif
#ifndef le_REMOTE
#define le_REMOTE Binding(HID::Page::LED, HID::LED::OTHER::REMOTE)
#endif
#ifndef le_FORWARD
#define le_FORWARD Binding(HID::Page::LED, HID::LED::OTHER::FORWARD)
#endif
#ifndef le_REVERSE
#define le_REVERSE Binding(HID::Page::LED, HID::LED::OTHER::REVERSE)
#endif
#ifndef le_STOP
#define le_STOP Binding(HID::Page::LED, HID::LED::OTHER::STOP)
#endif
#ifndef le_REWIND
#define le_REWIND Binding(HID::Page::LED, HID::LED::OTHER::REWIND)
#endif
#ifndef le_FAST_FORWARD
#define le_FAST_FORWARD Binding(HID::Page::LED, HID::LED::OTHER::FAST_FORWARD)
#endif
#ifndef le_PLAY
#define le_PLAY Binding(HID::Page::LED, HID::LED::OTHER::PLAY)
#endif
#ifndef le_PAUSE
#define le_PAUSE Binding(HID::Page::LED, HID::LED::OTHER::PAUSE)
#endif
#ifndef le_RECORD
#define le_RECORD Binding(HID::Page::LED, HID::LED::OTHER::RECORD)
#endif
#ifndef le_ERROR
#define le_ERROR Binding(HID::Page::LED, HID::LED::OTHER::ERROR)
#endif
#ifndef le_USAGE_SELECTED_INDICATOR
#define le_USAGE_SELECTED_INDICATOR Binding(HID::Page::LED, HID::LED::OTHER::USAGE_SELECTED_INDICATOR)
#endif
#ifndef le_USAGE_IN_USE_INDICATOR
#define le_USAGE_IN_USE_INDICATOR Binding(HID::Page::LED, HID::LED::OTHER::USAGE_IN_USE_INDICATOR)
#endif
#ifndef le_USAGE_MULTI_MODE_INDICATOR
#define le_USAGE_MULTI_MODE_INDICATOR Binding(HID::Page::LED, HID::LED::OTHER::USAGE_MULTI_MODE_INDICATOR)
#endif
#ifndef le_INDICATOR_ON
#define le_INDICATOR_ON Binding(HID::Page::LED, HID::LED::OTHER::INDICATOR_ON)
#endif
#ifndef le_INDICATOR_FLASH
#define le_INDICATOR_FLASH Binding(HID::Page::LED, HID::LED::OTHER::INDICATOR_FLASH)
#endif
#ifndef le_INDICATOR_SLOW_BLINK
#define le_INDICATOR_SLOW_BLINK Binding(HID::Page::LED, HID::LED::OTHER::INDICATOR_SLOW_BLINK)
#endif
#ifndef le_INDICATOR_FAST_BLINK
#define le_INDICATOR_FAST_BLINK Binding(HID::Page::LED, HID::LED::OTHER::INDICATOR_FAST_BLINK)
#endif
#ifndef le_INDICATOR_OFF
#define le_INDICATOR_OFF Binding(HID::Page::LED, HID::LED::OTHER::INDICATOR_OFF)
#endif
#ifndef le_FLASH_ON_TIME
#define le_FLASH_ON_TIME Binding(HID::Page::LED, HID::LED::OTHER::FLASH_ON_TIME)
#endif
#ifndef le_SLOW_BLINK_ON_TIME
#define le_SLOW_BLINK_ON_TIME Binding(HID::Page::LED, HID::LED::OTHER::SLOW_BLINK_ON_TIME)
#endif
#ifndef le_SLOW_BLINK_OFF_TIME
#define le_SLOW_BLINK_OFF_TIME Binding(HID::Page::LED, HID::LED::OTHER::SLOW_BLINK_OFF_TIME)
#endif
#ifndef le_FAST_BLINK_ON_TIME
#define le_FAST_BLINK_ON_TIME Binding(HID::Page::LED, HID::LED::OTHER::FAST_BLINK_ON_TIME)
#endif
#ifndef le_FAST_BLINK_OFF_TIME
#define le_FAST_BLINK_OFF_TIME Binding(HID::Page::LED, HID::LED::OTHER::FAST_BLINK_OFF_TIME)
#endif
#ifndef le_USAGE_INDICATOR_COLOR
#define le_USAGE_INDICATOR_COLOR Binding(HID::Page::LED, HID::LED::OTHER::USAGE_INDICATOR_COLOR)
#endif
#ifndef le_INDICATOR_RED
#define le_INDICATOR_RED Binding(HID::Page::LED, HID::LED::OTHER::INDICATOR_RED)
#endif
#ifndef le_INDICATOR_GREEN
#define le_INDICATOR_GREEN Binding(HID::Page::LED, HID::LED::OTHER::INDICATOR_GREEN)
#endif
#ifndef le_INDICATOR_AMBER
#define le_INDICATOR_AMBER Binding(HID::Page::LED, HID::LED::OTHER::INDICATOR_AMBER)
#endif
#ifndef le_GENERIC_INDICATOR
#define le_GENERIC_INDICATOR Binding(HID::Page::LED, HID::LED::OTHER::GENERIC_INDICATOR)
#endif
#ifndef le_SYS_SUSPEND
#define le_SYS_SUSPEND Binding(HID::Page::LED, HID::LED::OTHER::SYS_SUSPEND)
#endif
#ifndef le_EXTERNAL_POWER_CONNECTED
#define le_EXTERNAL_POWER_CONNECTED Binding(HID::Page::LED, HID::LED::OTHER::EXTERNAL_POWER_CONNECTED)
#endif
#ifndef le_INDICATOR_BLUE
#define le_INDICATOR_BLUE Binding(HID::Page::LED, HID::LED::OTHER::INDICATOR_BLUE)
#endif
#ifndef le_INDICATOR_ORANGE
#define le_INDICATOR_ORANGE Binding(HID::Page::LED, HID::LED::OTHER::INDICATOR_ORANGE)
#endif
#ifndef le_GOOD_STATUS
#define le_GOOD_STATUS Binding(HID::Page::LED, HID::LED::OTHER::GOOD_STATUS)
#endif
#ifndef le_WARNING_STATUS
#define le_WARNING_STATUS Binding(HID::Page::LED, HID::LED::OTHER::WARNING_STATUS)
#endif
#ifndef le_RGB_LED
#define le_RGB_LED Binding(HID::Page::LED, HID::LED::OTHER::RGB_LED)
#endif
#ifndef le_RED_LED_CHANNEL
#define le_RED_LED_CHANNEL Binding(HID::Page::LED, HID::LED::OTHER::RED_LED_CHANNEL)
#endif
#ifndef le_BLUE_LED_CHANNEL
#define le_BLUE_LED_CHANNEL Binding(HID::Page::LED, HID::LED::OTHER::BLUE_LED_CHANNEL)
#endif
#ifndef le_GREEN_LED_CHANNEL
#define le_GREEN_LED_CHANNEL Binding(HID::Page::LED, HID::LED::OTHER::GREEN_LED_CHANNEL)
#endif
#ifndef le_LED_INTENSITY
#define le_LED_INTENSITY Binding(HID::Page::LED, HID::LED::OTHER::LED_INTENSITY)
#endif
#ifndef le_SYS_MICROPHONE_MUTE
#define le_SYS_MICROPHONE_MUTE Binding(HID::Page::LED, HID::LED::OTHER::SYS_MICROPHONE_MUTE)
#endif
#ifndef le_PLAYER_INDICATOR
#define le_PLAYER_INDICATOR Binding(HID::Page::LED, HID::LED::OTHER::PLAYER_INDICATOR)
#endif
#ifndef le_PLAYER_1
#define le_PLAYER_1 Binding(HID::Page::LED, HID::LED::OTHER::PLAYER_1)
#endif
#ifndef le_PLAYER_2
#define le_PLAYER_2 Binding(HID::Page::LED, HID::LED::OTHER::PLAYER_2)
#endif
#ifndef le_PLAYER_3
#define le_PLAYER_3 Binding(HID::Page::LED, HID::LED::OTHER::PLAYER_3)
#endif
#ifndef le_PLAYER_4
#define le_PLAYER_4 Binding(HID::Page::LED, HID::LED::OTHER::PLAYER_4)
#endif
#ifndef le_PLAYER_5
#define le_PLAYER_5 Binding(HID::Page::LED, HID::LED::OTHER::PLAYER_5)
#endif
#ifndef le_PLAYER_6
#define le_PLAYER_6 Binding(HID::Page::LED, HID::LED::OTHER::PLAYER_6)
#endif
#ifndef le_PLAYER_7
#define le_PLAYER_7 Binding(HID::Page::LED, HID::LED::OTHER::PLAYER_7)
#endif
#ifndef le_PLAYER_8
#define le_PLAYER_8 Binding(HID::Page::LED, HID::LED::OTHER::PLAYER_8)
#endif
