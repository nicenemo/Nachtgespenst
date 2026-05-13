#pragma once
#include "HID_Types.h"

// --- Page Definitions ---
#include "hid_pages/Page91_Arcade.h"
#include "hid_pages/Page14_AuxiliaryDisplay.h"
#include "hid_pages/Page8C_BarcodeScanner.h"
#include "hid_pages/Page85_BatterySystem.h"
#include "hid_pages/Page41_BrailleDisplay.h"
#include "hid_pages/Page09_Button.h"
#include "hid_pages/Page90_CameraControl.h"
#include "hid_pages/Page0C_Consumer.h"
#include "hid_pages/Page0D_Digitizers.h"
#include "hid_pages/Page12_EyeandHeadTrackers.h"
#include "hid_pages/PageF1D0_FIDOAlliance.h"
#include "hid_pages/Page05_GameControls.h"
#include "hid_pages/Page01_Desktop.h"
#include "hid_pages/Page06_GenericDeviceControls.h"
#include "hid_pages/Page0E_Haptics.h"
#include "hid_pages/Page07_Keyboard.h"
#include "hid_pages/Page08_LEDs.h"
#include "hid_pages/Page59_LightingAndIllumination.h"
#include "hid_pages/Page8E_MagneticStripeReader.h"
#include "hid_pages/Page40_MedicalInstrument.h"
#include "hid_pages/Page80_Monitor.h"
#include "hid_pages/Page81_MonitorEnumerated.h"
#include "hid_pages/Page0A_Ordinal.h"
#include "hid_pages/Page0F_PhysicalInputDevice.h"
#include "hid_pages/Page84_Power.h"
#include "hid_pages/Page8D_Scales.h"
#include "hid_pages/Page20_Sensors.h"
#include "hid_pages/Page02_SimulationControls.h"
#include "hid_pages/Page11_SoC.h"
#include "hid_pages/Page04_SportControls.h"
#include "hid_pages/Page0B_TelephonyDevice.h"
#include "hid_pages/Page82_VESAVirtualControls.h"
#include "hid_pages/Page03_VRControls.h"

// --- Shortcut Definitions ---
#include "hid_shortcuts/Page91_Arcade.h"
#include "hid_shortcuts/Page14_AuxiliaryDisplay.h"
#include "hid_shortcuts/Page8C_BarcodeScanner.h"
#include "hid_shortcuts/Page85_BatterySystem.h"
#include "hid_shortcuts/Page41_BrailleDisplay.h"
#include "hid_shortcuts/Page09_Button.h"
#include "hid_shortcuts/Page90_CameraControl.h"
#include "hid_shortcuts/Page0C_Consumer.h"
#include "hid_shortcuts/Page0D_Digitizers.h"
#include "hid_shortcuts/Page12_EyeandHeadTrackers.h"
#include "hid_shortcuts/PageF1D0_FIDOAlliance.h"
#include "hid_shortcuts/Page05_GameControls.h"
#include "hid_shortcuts/Page01_Desktop.h"
#include "hid_shortcuts/Page06_GenericDeviceControls.h"
#include "hid_shortcuts/Page0E_Haptics.h"
#include "hid_shortcuts/Page07_Keyboard.h"
#include "hid_shortcuts/Page08_LEDs.h"
#include "hid_shortcuts/Page59_LightingAndIllumination.h"
#include "hid_shortcuts/Page8E_MagneticStripeReader.h"
#include "hid_shortcuts/Page40_MedicalInstrument.h"
#include "hid_shortcuts/Page80_Monitor.h"
#include "hid_shortcuts/Page81_MonitorEnumerated.h"
#include "hid_shortcuts/Page0A_Ordinal.h"
#include "hid_shortcuts/Page0F_PhysicalInputDevice.h"
#include "hid_shortcuts/Page84_Power.h"
#include "hid_shortcuts/Page8D_Scales.h"
#include "hid_shortcuts/Page20_Sensors.h"
#include "hid_shortcuts/Page02_SimulationControls.h"
#include "hid_shortcuts/Page11_SoC.h"
#include "hid_shortcuts/Page04_SportControls.h"
#include "hid_shortcuts/Page0B_TelephonyDevice.h"
#include "hid_shortcuts/Page82_VESAVirtualControls.h"
#include "hid_shortcuts/Page03_VRControls.h"

/**
 * Dynamic Helper Macros
 */
#define H91(c) HID::Key{HID::Page::ARCADE, (uint16_t)c}
#define H14(c) HID::Key{HID::Page::AUXILIARY_DISPLAY, (uint16_t)c}
#define H8C(c) HID::Key{HID::Page::BARCODE_SCANNER, (uint16_t)c}
#define H85(c) HID::Key{HID::Page::BATTERY_SYSTEM, (uint16_t)c}
#define H41(c) HID::Key{HID::Page::BRAILLE_DISPLAY, (uint16_t)c}
#define H09(c) HID::Key{HID::Page::BUTTON, (uint16_t)c}
#define H90(c) HID::Key{HID::Page::CAMERA_CONTROL, (uint16_t)c}
#define C(c) HID::Key{HID::Page::CONSUMER, (uint16_t)c}
#define H0D(c) HID::Key{HID::Page::DIGITIZERS, (uint16_t)c}
#define H12(c) HID::Key{HID::Page::EYE_AND_HEAD_TRACKERS, (uint16_t)c}
#define HF1D0(c) HID::Key{HID::Page::FIDO_ALLIANCE, (uint16_t)c}
#define H05(c) HID::Key{HID::Page::GAME_CONTROLS, (uint16_t)c}
#define D(c) HID::Key{HID::Page::GENERIC_DESKTOP, (uint16_t)c}
#define H06(c) HID::Key{HID::Page::GENERIC_DEVICE_CONTROLS, (uint16_t)c}
#define H0E(c) HID::Key{HID::Page::HAPTICS, (uint16_t)c}
#define K(c) HID::Key{HID::Page::KEYBOARD_KEYPAD, (uint16_t)c}
#define H08(c) HID::Key{HID::Page::LED, (uint16_t)c}
#define H59(c) HID::Key{HID::Page::LIGHTING_AND_ILLUMINATION, (uint16_t)c}
#define H8E(c) HID::Key{HID::Page::MAGNETIC_STRIPE_READER, (uint16_t)c}
#define H40(c) HID::Key{HID::Page::MEDICAL_INSTRUMENT, (uint16_t)c}
#define H80(c) HID::Key{HID::Page::MONITOR, (uint16_t)c}
#define H81(c) HID::Key{HID::Page::MONITOR_ENUMERATED, (uint16_t)c}
#define H0A(c) HID::Key{HID::Page::ORDINAL, (uint16_t)c}
#define H0F(c) HID::Key{HID::Page::PHYSICAL_INPUT_DEVICE, (uint16_t)c}
#define H84(c) HID::Key{HID::Page::POWER, (uint16_t)c}
#define H8D(c) HID::Key{HID::Page::SCALES, (uint16_t)c}
#define H20(c) HID::Key{HID::Page::SENSORS, (uint16_t)c}
#define H02(c) HID::Key{HID::Page::SIMULATION_CONTROLS, (uint16_t)c}
#define H11(c) HID::Key{HID::Page::SOC, (uint16_t)c}
#define H04(c) HID::Key{HID::Page::SPORT_CONTROLS, (uint16_t)c}
#define H0B(c) HID::Key{HID::Page::TELEPHONY_DEVICE, (uint16_t)c}
#define H82(c) HID::Key{HID::Page::VESA_VIRTUAL_CONTROLS, (uint16_t)c}
#define H03(c) HID::Key{HID::Page::VR_CONTROLS, (uint16_t)c}
