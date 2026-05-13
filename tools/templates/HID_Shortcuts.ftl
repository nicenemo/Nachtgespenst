#pragma once
#include "HID_Types.h"

// --- Page Definitions ---
<#list pages as page>
#include "hid_pages/${page.filename}"
</#list>

// --- Shortcut Definitions ---
<#list pages as page>
#include "hid_shortcuts/${page.filename}"
</#list>

/**
 * Dynamic Helper Macros
 */
<#list pages as page>
#define ${page.macroChar}(c) HID::Key{HID::Page::${page.constantName}, (uint16_t)c}
</#list>
