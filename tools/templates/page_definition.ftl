#pragma once
#include "../HID_Types.h"

namespace HID::${page.namespace} {
    constexpr uint16_t PAGE_ID = 0x${page.idHex};
<#list page.groupedUsages as kind, items>
    namespace ${kind} {
    <#list items as usage>
        <#assign safeName = usage.cleanName>
        <#if safeName?matches("^[0-9].*")><#assign safeName = "ID_" + safeName></#if>
        constexpr Usage ${safeName} = { 0x${usage.idHex}, UsageKind::${kind}, "${safeName}" };
    </#list>
    }
</#list>
}
