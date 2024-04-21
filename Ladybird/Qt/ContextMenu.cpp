/*
 * Copyright (c) 2024, Tim Flynn <trflynn89@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/WeakPtr.h>
#include <Ladybird/Qt/ContextMenu.h>
#include <Ladybird/Qt/StringUtils.h>
#include <LibWebView/ContextMenu.h>
#include <QAction>
#include <QMenu>
#include <QWidget>

namespace Ladybird {

static void add_items_to_menu(QMenu& menu, QWidget* parent, Span<WebView::ContextMenu::MenuItem> menu_items)
{
    for (auto& menu_item : menu_items) {
        menu_item.visit(
            [&](NonnullRefPtr<WebView::Action>& action) {
                if (action->chrome_action != nullptr) {
                    menu.addAction(static_cast<QAction*>(action->chrome_action));
                    return;
                }

                auto* qaction = new QAction(qstring_from_ak_string(action->name), parent);
                action->chrome_action = qaction;

                QObject::connect(qaction, &QAction::triggered, [action = action->make_weak_ptr()]() {
                    if (action)
                        action->action();
                });

                menu.addAction(qaction);
            },
            [&](NonnullRefPtr<WebView::ContextMenu> const& submenu) {
                auto* qsubmenu = new QMenu(qstring_from_ak_string(submenu->title()), parent);
                add_items_to_menu(*qsubmenu, parent, submenu->items());

                menu.addMenu(qsubmenu);
            },
            [&](WebView::Separator) {
                menu.addSeparator();
            });
    }
}

QMenu* create_context_menu(QWidget* parent, WebView::ContextMenu& context_menu)
{
    auto* menu = new QMenu(qstring_from_ak_string(context_menu.title()), parent);
    add_items_to_menu(*menu, parent, context_menu.items());

    return menu;
}

void update_context_menu_action_names(WebView::ContextMenu& context_menu, Function<Optional<String>(u32, StringView)> callback)
{
    context_menu.for_each_action<QAction>([&](auto& action, auto const& context_menu_action) {
        if (!context_menu_action.id.has_value())
            return;

        auto name = callback(*context_menu_action.id, context_menu_action.name);
        if (!name.has_value())
            return;

        action.setText(qstring_from_ak_string(*name));
    });
}

}
