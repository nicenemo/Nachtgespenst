#pragma once
#include "HID_Types.h"
#include "Bindings.h"

// 1. Include the raw namespaced definitions
<#list pages as page>
#include "hid_pages/${page.filename}" 
</#list>

// 2. Include the shortcut macros (kp_A, cp_MUTE, etc.)
<#list pages as page>
#include "hid_shortcuts/${page.filename}" 
</#list>

// 3. Short-hand namespaces for manual access
<#list pages as page>
namespace ${page.macroPrefix?upper_case} = HID::${page.namespace};
</#list>
