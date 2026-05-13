#pragma once
#include \"HID_Types.h\"

namespace HID {
    struct PageInfo {
        uint16_t id;
        const char* name;
        const char* macro; 
    };

    namespace Page {
<#list pages as page>
        constexpr PageInfo ${page.namespace} = { 0x${page.idHex}, \"${page.name}\", \"${page.macroPrefix}\" };
</#list>
    }

    constexpr PageInfo ALL_PAGES[] = {
<#list pages as page>
        Page::${page.namespace}<#if page_has_next>,</#if>
</#list>
    };

    constexpr uint8_t PAGE_COUNT = sizeof(ALL_PAGES) / sizeof(ALL_PAGES[0]);

    inline const PageInfo* getPageInfo(uint16_t id) {
        for (uint8_t i = 0; i < PAGE_COUNT; i++) {
            if (ALL_PAGES[i].id == id) return &ALL_PAGES[i];
        }
        return nullptr;
    }
}
