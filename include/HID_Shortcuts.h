#pragma once
#include "HID_Types.h"
#include "Bindings.h"

// 1. Include the raw namespaced definitions
#include "hid_pages/Arcade.h" 
#include "hid_pages/AuxiliaryDisplay.h" 
#include "hid_pages/BarcodeScanner.h" 
#include "hid_pages/BatterySystem.h" 
#include "hid_pages/BrailleDisplay.h" 
#include "hid_pages/Button.h" 
#include "hid_pages/CameraControl.h" 
#include "hid_pages/Consumer.h" 
#include "hid_pages/Digitizers.h" 
#include "hid_pages/EyeandHeadTrackers.h" 
#include "hid_pages/FIDOAlliance.h" 
#include "hid_pages/GameControls.h" 
#include "hid_pages/GenericDesktop.h" 
#include "hid_pages/GenericDeviceControls.h" 
#include "hid_pages/Haptics.h" 
#include "hid_pages/KeyboardKeypad.h" 
#include "hid_pages/LED.h" 
#include "hid_pages/LightingAndIllumination.h" 
#include "hid_pages/MagneticStripeReader.h" 
#include "hid_pages/MedicalInstrument.h" 
#include "hid_pages/Monitor.h" 
#include "hid_pages/MonitorEnumerated.h" 
#include "hid_pages/Ordinal.h" 
#include "hid_pages/PhysicalInputDevice.h" 
#include "hid_pages/Power.h" 
#include "hid_pages/Scales.h" 
#include "hid_pages/Sensors.h" 
#include "hid_pages/SimulationControls.h" 
#include "hid_pages/SoC.h" 
#include "hid_pages/SportControls.h" 
#include "hid_pages/TelephonyDevice.h" 
#include "hid_pages/VESAVirtualControls.h" 
#include "hid_pages/VRControls.h" 

// 2. Include the shortcut macros (kp_A, cp_MUTE, etc.)
#include "hid_shortcuts/Arcade.h" 
#include "hid_shortcuts/AuxiliaryDisplay.h" 
#include "hid_shortcuts/BarcodeScanner.h" 
#include "hid_shortcuts/BatterySystem.h" 
#include "hid_shortcuts/BrailleDisplay.h" 
#include "hid_shortcuts/Button.h" 
#include "hid_shortcuts/CameraControl.h" 
#include "hid_shortcuts/Consumer.h" 
#include "hid_shortcuts/Digitizers.h" 
#include "hid_shortcuts/EyeandHeadTrackers.h" 
#include "hid_shortcuts/FIDOAlliance.h" 
#include "hid_shortcuts/GameControls.h" 
#include "hid_shortcuts/GenericDesktop.h" 
#include "hid_shortcuts/GenericDeviceControls.h" 
#include "hid_shortcuts/Haptics.h" 
#include "hid_shortcuts/KeyboardKeypad.h" 
#include "hid_shortcuts/LED.h" 
#include "hid_shortcuts/LightingAndIllumination.h" 
#include "hid_shortcuts/MagneticStripeReader.h" 
#include "hid_shortcuts/MedicalInstrument.h" 
#include "hid_shortcuts/Monitor.h" 
#include "hid_shortcuts/MonitorEnumerated.h" 
#include "hid_shortcuts/Ordinal.h" 
#include "hid_shortcuts/PhysicalInputDevice.h" 
#include "hid_shortcuts/Power.h" 
#include "hid_shortcuts/Scales.h" 
#include "hid_shortcuts/Sensors.h" 
#include "hid_shortcuts/SimulationControls.h" 
#include "hid_shortcuts/SoC.h" 
#include "hid_shortcuts/SportControls.h" 
#include "hid_shortcuts/TelephonyDevice.h" 
#include "hid_shortcuts/VESAVirtualControls.h" 
#include "hid_shortcuts/VRControls.h" 

// 3. Short-hand namespaces for manual access
namespace AR = HID::Arcade;
namespace AU = HID::AuxiliaryDisplay;
namespace BA = HID::BarcodeScanner;
namespace BA = HID::BatterySystem;
namespace BR = HID::BrailleDisplay;
namespace BU = HID::Button;
namespace CA = HID::CameraControl;
namespace CP = HID::Consumer;
namespace DI = HID::Digitizers;
namespace EY = HID::EyeandHeadTrackers;
namespace FI = HID::FIDOAlliance;
namespace GA = HID::GameControls;
namespace GP = HID::GenericDesktop;
namespace GE = HID::GenericDeviceControls;
namespace HA = HID::Haptics;
namespace KP = HID::KeyboardKeypad;
namespace LE = HID::LED;
namespace LI = HID::LightingAndIllumination;
namespace MA = HID::MagneticStripeReader;
namespace ME = HID::MedicalInstrument;
namespace MO = HID::Monitor;
namespace MO = HID::MonitorEnumerated;
namespace OR = HID::Ordinal;
namespace PH = HID::PhysicalInputDevice;
namespace PO = HID::Power;
namespace SC = HID::Scales;
namespace SE = HID::Sensors;
namespace SI = HID::SimulationControls;
namespace SO = HID::SoC;
namespace SP = HID::SportControls;
namespace TE = HID::TelephonyDevice;
namespace VE = HID::VESAVirtualControls;
namespace VR = HID::VRControls;
