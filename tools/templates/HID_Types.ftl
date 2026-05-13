#pragma once
#include <stdint.h>

namespace HID {
    /**
     * Complete Page Mapping
     */
    enum class Page : uint16_t {
    <#list pages as page>
        ${page.constantName} = 0x${page.idHex}<#if page?has_next>,</#if>
    </#list>
    };

    struct Key {
        Page page;
        uint16_t code;
    };
}
