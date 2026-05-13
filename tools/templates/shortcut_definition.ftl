#pragma once
/**
 * Shortcuts for ${page.name}
 */
<#list page.usages as u>
#define ${page.macroPrefix}_${u.cleanName} Binding(HID::Page::${page.constantName}, HID::${page.namespace}::${u.getPrimaryKind()}::${u.cleanName})
</#list>
