#pragma once
#include "../HID_Shortcuts.h"

/**
 * HID Shortcuts for ${page.name}
 */
<#list page.usages as usage>
#define ${page.macroChar}_${usage.cleanName} ${page.macroChar}(0x${usage.idHex})
</#list>
