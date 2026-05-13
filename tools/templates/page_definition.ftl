#pragma once
#include <stdint.h>

/**
 * HID Usage Page: ${pageName} (0x${pageId?upper_case})
 * Generated via jbang script (Java 26) + Apache FreeMarker
 */
namespace HID::${namespaceName} {
    // The official Usage Page ID
    constexpr uint8_t PAGE_ID = 0x${pageId?upper_case};

<#-- Changed 'usages' to 'page.UsageIds' and fields to PascalCase -->
<#list page.UsageIds as usage>
    <#if usage.Description??>// ${usage.Description}</#if>
    constexpr uint${(pageId == "0C" || pageId == "20")?string("16", "8")}_t ${usage.Name} = 0x${usage.Id?upper_case};
</#list>

}
