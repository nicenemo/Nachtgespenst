#pragma once
#include <stdint.h>

/**
 * HID Core Types
 * Generated via jbang script (Java 26) + Apache FreeMarker
 * Source: HID Usage Tables (HUT)
 */
namespace HID {
    /**
     * Complete Page Mapping
     * Generated from all ${pages?size} pages in HUT JSON
     */
    enum class Page : uint8_t {
    <#list pages as page>
        ${page.constantName} = 0x${page.id?upper_case}<#if page?has_next>,</#if>
    </#list>
    };

    /**
     * Universal HID Key structure
     * page: The Usage Page (e.g., Keyboard, Consumer)
     * code: The 16-bit Usage ID
     */
    struct Key {
        Page page;
        uint16_t code;
    };
}
