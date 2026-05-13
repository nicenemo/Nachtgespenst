#pragma once
#include <stdint.h>

namespace HID {
    enum class UsageKind : uint8_t {
        OTHER = 0, CA, CP, CL, LC, DV, Sel, OOC, MC, OSC, RTC [cite: 10]
    };

    struct Usage {
        uint16_t id; [cite: 11]
        UsageKind kind; [cite: 11]
        const char* identifier; [cite: 11]
    };

    enum class Page : uint16_t {
    <#list pages as page>
        ${page.constantName} = 0x${page.idHex}<#if page?has_next>,</#if> [cite: 12]
    </#list>
    };
}
