#pragma once
/**
 * Shortcuts for ${page.name}
 */
<#list page.usages as u>
<#assign safeName = u.cleanName>
<#if safeName?matches("^[0-9].*")><#assign safeName = "ID_" + safeName></#if>
#ifndef ${page.macroPrefix}_${safeName}
#define ${page.macroPrefix}_${safeName} Binding(HID::Page::${page.constantName}, HID::${page.namespace}::${u.getPrimaryKind()}::${safeName})
#endif
</#list>
