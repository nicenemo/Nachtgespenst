#pragma once
/**
 * Shortcuts for ${page.name}
 */
<#list page.usages as u>
#ifndef ${page.macroPrefix}_${u.cleanName}
#define ${page.macroPrefix}_${u.cleanName} Binding(HID::Page::${page.constantName}, HID::${page.namespace}::${u.getPrimaryKind()}::${u.cleanName})
#endif
</#list>
