#pragma once
#include "HID_Types.h"
#include <string_view>

namespace HID::Metadata {

    struct UsageInfo {
        uint16_t id;
        std::string_view name;
        std::string_view description;
    };

    struct PageInfo {
        HID::Page id;
        std::string_view name;
        std::string_view ns;
        const UsageInfo* usages;
        size_t usageCount;
    };

    <#list pages as page>
    <#if page.usages?has_content>
    static constexpr UsageInfo USAGES_${page.constantName}[] = {
        <#list page.usages as u>
        { 0x${u.idHex}, "${u.name}", "${(u.description!"")?j_string}" }<#if u?has_next>,</#if>
        </#list>
    };
    </#if>
    </#list>

    inline constexpr PageInfo Pages[] = {
    <#list pages as page>
        { 
            HID::Page::${page.constantName}, 
            "${page.name}", 
            "${page.namespace}", 
            <#if page.usages?has_content>USAGES_${page.constantName}<#else>nullptr</#if>, 
            ${page.usages?size} 
        }<#if page?has_next>,</#if>
    </#list>
    };
}
