#pragma once
#include "../HID_Types.h"

namespace HID::${page.namespace} {
    constexpr uint8_t PAGE_ID = 0x${page.idHex};
<#list page.groupedUsages as kind, items>
    namespace ${kind} {
    <#list items as usage>
        constexpr Usage ${usage.cleanName} = { 0x${usage.idHex}, UsageKind::${kind}, "${usage.cleanName}" };
    </#list>
    }
</#list>
}
