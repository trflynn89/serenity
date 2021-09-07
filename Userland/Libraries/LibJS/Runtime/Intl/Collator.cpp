/*
 * Copyright (c) 2021, Tim Flynn <trflynn89@pm.me>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Intl/Collator.h>

namespace JS::Intl {

Vector<StringView> const& Collator::relevant_extension_keys()
{
    // 10.2.3 Internal Slots, https://tc39.es/ecma402/#sec-intl-collator-internal-slots
    // The value of the [[RelevantExtensionKeys]] internal slot is a List that must include the element "co", may include any or all of the elements "kf" and "kn", and must not include any other elements.
    static Vector<StringView> relevant_extension_keys { "co"sv, "kf"sv, "kn"sv };
    return relevant_extension_keys;
}

// 10 Collator Objects, https://tc39.es/ecma402/#collator-objects
Collator::Collator(Object& prototype)
    : Object(prototype)
{
}

void Collator::set_usage(StringView type)
{
    if (type == "sort"sv) {
        m_usage = Usage::Sort;
    } else if (type == "search"sv) {
        m_usage = Usage::Search;
    } else {
        VERIFY_NOT_REACHED();
    }
}

StringView Collator::usage_string() const
{
    switch (m_usage) {
    case Usage::Sort:
        return "sort"sv;
    case Usage::Search:
        return "search"sv;
    default:
        VERIFY_NOT_REACHED();
    }
}

void Collator::set_sensitivity(StringView type)
{
    if (type == "base"sv) {
        m_sensitivity = Sensitivity::Base;
    } else if (type == "accent"sv) {
        m_sensitivity = Sensitivity::Accent;
    } else if (type == "case"sv) {
        m_sensitivity = Sensitivity::Case;
    } else if (type == "variant"sv) {
        m_sensitivity = Sensitivity::Variant;
    } else {
        VERIFY_NOT_REACHED();
    }
}

StringView Collator::sensitivity_string() const
{
    switch (m_sensitivity) {
    case Sensitivity::Base:
        return "base"sv;
    case Sensitivity::Accent:
        return "accent"sv;
    case Sensitivity::Case:
        return "case"sv;
    case Sensitivity::Variant:
        return "variant"sv;
    default:
        VERIFY_NOT_REACHED();
    }
}

}
