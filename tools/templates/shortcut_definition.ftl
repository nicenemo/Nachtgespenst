#pragma once
#include "../HID_Shortcuts.h"

/**
 * HID Shortcuts for ${pageName}
 * Generated via jbang script (Java 26) + Apache FreeMarker
 */

<#-- Changed 'usages' to 'page.UsageIds' and fields to PascalCase -->
<#list page.UsageIds as usage>
#define ${prefix}_${usage.Name} ${macroName}(0x${usage.Id?upper_case})
</#list>

/* * Example:
 * #define ${prefix}_SAMPLE ${macroName}(0x01)
 */
