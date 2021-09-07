/*
 * Copyright (c) 2021, Tim Flynn <trflynn89@pm.me>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Optional.h>
#include <AK/String.h>
#include <AK/StringView.h>
#include <LibJS/Runtime/Object.h>

namespace JS::Intl {

class Collator final : public Object {
    JS_OBJECT(Collator, Object);

    enum class Usage {
        Invalid,
        Sort,
        Search,
    };

    enum class Sensitivity {
        Invalid,
        Base,
        Accent,
        Case,
        Variant,
    };

public:
    static Vector<StringView> const& relevant_extension_keys(); // [[RelevantExtensionKeys]]

    Collator(Object& prototype);
    virtual ~Collator() override = default;

    String const& locale() const { return m_locale; }
    void set_locale(String locale) { m_locale = move(locale); }

    Usage usage() const { return m_usage; }
    void set_usage(StringView usage);
    StringView usage_string() const;

    Sensitivity sensitivity() const { return m_sensitivity; }
    void set_sensitivity(StringView sensitivity);
    StringView sensitivity_string() const;

    bool has_case_first() const { return m_case_first.has_value(); }
    String const& case_first() const { return m_case_first.value(); }
    void set_case_first(String case_first) { m_case_first = move(case_first); }

    String const& collation() const { return m_collation; }
    void set_collation(String collation) { m_collation = move(collation); }

    bool ignore_punctuation() const { return m_ignore_punctuation; }
    void set_ignore_punctuation(bool ignore_punctuation) { m_ignore_punctuation = ignore_punctuation; }

    bool numeric() const { return m_numeric; }
    void set_numeric(bool numeric) { m_numeric = numeric; }

private:
    String m_locale;                                    // [[Locale]]
    Usage m_usage { Usage::Invalid };                   // [[Usage]]
    Sensitivity m_sensitivity { Sensitivity::Invalid }; // [[Sensitivity]]
    Optional<String> m_case_first;                      // [[CaseFirst]]
    String m_collation;                                 // [[Collation]]
    bool m_ignore_punctuation { false };                // [[IgnorePunctuation]]
    bool m_numeric { false };                           // [[Numeric]]
};

}
