#pragma once
#include "HID_Types.h"

/**
 * Master HID Shortcuts Entry Point
 * Generated via jbang script (Java 26)
 */

// --- Page Definitions ---
<#list pages as page>
#include "hid_pages/${page.filename}"
</#list>

// --- Shortcut Macros ---
<#list pages as page>
#include "hid_shortcuts/${page.filename}"
</#list>

// --- Manual Logic Extensions ---
#include "hid_pages/Page07_Keyboard_Modifiers.h"
#include "hid_pages/Page08_LED_Masks.h"

/**
 * Dynamic Helper Macros
 * Mapping standard constructor letters to Page IDs
 */
<#list pages as page>
<#if page.macroChar??>
#define ${page.macroChar}(c) HID::Key{HID::Page::${page.constantName}, (uint16_t)c}
</#if>
</#list>
