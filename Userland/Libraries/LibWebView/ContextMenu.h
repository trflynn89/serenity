/*
 * Copyright (c) 2024, Tim Flynn <trflynn89@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Function.h>
#include <AK/NonnullRefPtr.h>
#include <AK/Optional.h>
#include <AK/RefCounted.h>
#include <AK/StringView.h>
#include <AK/Variant.h>
#include <AK/Vector.h>
#include <AK/Weakable.h>
#include <LibWebView/Forward.h>

namespace WebView {

struct Action
    : public RefCounted<Action>
    , public Weakable<Action> {
    Action(StringView, Function<void()>);

    template<Enum ID>
    Action(StringView name, ID id, Function<void()> action)
        : Action(name, move(action))
    {
        this->id = static_cast<u32>(id);
    }

    StringView name;
    Optional<u32> id;
    Function<void()> action;

    void* chrome_action { nullptr };
};

struct Separator { };

class ContextMenu : public RefCounted<ContextMenu> {
public:
    using MenuItem = Variant<NonnullRefPtr<Action>, NonnullRefPtr<ContextMenu>, Separator>;

    explicit ContextMenu(StringView title);

    void add_action(NonnullRefPtr<Action> action);
    void add_submenu(NonnullRefPtr<ContextMenu> submenu);
    void add_separator();

    StringView title() const { return m_title; }

    Span<MenuItem> items() { return m_items; }
    ReadonlySpan<MenuItem> items() const { return m_items; }

    template<typename ActionType>
    void for_each_action(Function<void(ActionType&, Action const&)> const& callback)
    {
        for (auto& item : m_items) {
            item.visit(
                [&](NonnullRefPtr<Action>& action) {
                    if (action->chrome_action == nullptr)
                        return;

                    auto& chrome_action = *reinterpret_cast<ActionType*>(action->chrome_action);
                    callback(chrome_action, *action);
                },
                [&](NonnullRefPtr<ContextMenu>& submenu) {
                    submenu->for_each_action(callback);
                },
                [&](Separator) {
                });
        }
    }

private:
    StringView m_title;
    Vector<MenuItem> m_items {};
};

}
