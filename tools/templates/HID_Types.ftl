#pragma once
#include <stdint.h>

namespace HID {
    enum class UsageKind : uint8_t {
        OTHER = 0, CA, CP, CL, LC, DV, Sel, OOC, MC, OSC, RTC
    };

    struct Usage {
        uint16_t id;
        UsageKind kind;
        const char* identifier;
    };

    enum class Page : uint16_t {
        UNDEFINED = 0x0000,
    <#list pages as page>
        ${page.constantName} = 0x${page.idHex}<#if page?has_next>,</#if>
    </#list>
    };
}
