#pragma once
/**
 * Shortcuts for Magnetic Stripe Reader
 */
#define ma_MSR_DEVICE_READ_ONLY Binding(HID::Page::MAGNETIC_STRIPE_READER, HID::MagneticStripeReader::CA::MSR_DEVICE_READ_ONLY)
#define ma_TRACK_1_LENGTH Binding(HID::Page::MAGNETIC_STRIPE_READER, HID::MagneticStripeReader::DV::TRACK_1_LENGTH)
#define ma_TRACK_2_LENGTH Binding(HID::Page::MAGNETIC_STRIPE_READER, HID::MagneticStripeReader::DV::TRACK_2_LENGTH)
#define ma_TRACK_3_LENGTH Binding(HID::Page::MAGNETIC_STRIPE_READER, HID::MagneticStripeReader::DV::TRACK_3_LENGTH)
#define ma_TRACK_JIS_LENGTH Binding(HID::Page::MAGNETIC_STRIPE_READER, HID::MagneticStripeReader::DV::TRACK_JIS_LENGTH)
#define ma_TRACK_DATA Binding(HID::Page::MAGNETIC_STRIPE_READER, HID::MagneticStripeReader::OTHER::TRACK_DATA)
#define ma_TRACK_1_DATA Binding(HID::Page::MAGNETIC_STRIPE_READER, HID::MagneticStripeReader::OTHER::TRACK_1_DATA)
#define ma_TRACK_2_DATA Binding(HID::Page::MAGNETIC_STRIPE_READER, HID::MagneticStripeReader::OTHER::TRACK_2_DATA)
#define ma_TRACK_3_DATA Binding(HID::Page::MAGNETIC_STRIPE_READER, HID::MagneticStripeReader::OTHER::TRACK_3_DATA)
#define ma_TRACK_JIS_DATA Binding(HID::Page::MAGNETIC_STRIPE_READER, HID::MagneticStripeReader::OTHER::TRACK_JIS_DATA)
