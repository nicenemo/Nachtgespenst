<#assign kindMap = {
    "CA": "Application Collection",
    "CP": "Physical Collection",
    "CL": "Logical Collection",
    "LC": "Linear Control (Relative/Range)",
    "DV": "Dynamic Value (Absolute/Variable)",
    "Sel": "Selector (Menu/Choice)",
    "OOC": "On/Off Control (Toggle)",
    "MC": "Momentary Control (Push)",
    "OSC": "One Shot Control",
    "RTC": "Re-trigger Control",
    "OTHER": "Other / Miscellaneous"
}>
#pragma once
#include <stdint.h>

/**
 * HID Usage Page: ${page.name} (0x${page.idHex})
 */
namespace HID::${page.namespace} {
    constexpr uint8_t PAGE_ID = 0x${page.idHex};

<#list page.groupedUsages as kind, items>
    // ==========================================
    // KIND: ${kindMap[kind]!kind} (${kind})
    // ==========================================
    <#list items as usage>
    <#if usage.description?? && usage.description?has_content>// ${usage.description}</#if>
    constexpr uint${(page.idHex == "0C" || page.idHex == "0F" || page.idHex == "20")?string("16", "8")}_t ${usage.cleanName} = 0x${usage.idHex};
    </#list>

</#list>
}
