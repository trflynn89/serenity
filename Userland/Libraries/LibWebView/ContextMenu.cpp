/*
 * Copyright (c) 2024, Tim Flynn <trflynn89@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibWebView/ContextMenu.h>

namespace WebView {

Action::Action(StringView name, Function<void()> action)
    : name(name)
    , action(move(action))
{
}

ContextMenu::ContextMenu(StringView title)
    : m_title(title)
{
}

void ContextMenu::add_action(NonnullRefPtr<Action> action)
{
    m_items.append(move(action));
}

void ContextMenu::add_submenu(NonnullRefPtr<ContextMenu> submenu)
{
    m_items.append(move(submenu));
}

void ContextMenu::add_separator()
{
    m_items.append(Separator {});
}

}
