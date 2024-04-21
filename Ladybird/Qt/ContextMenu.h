/*
 * Copyright (c) 2024, Tim Flynn <trflynn89@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Function.h>
#include <LibWebView/Forward.h>

class QAction;
class QMenu;
class QWidget;

namespace Ladybird {

QMenu* create_context_menu(QWidget* parent, WebView::ContextMenu& context_menu);
void update_context_menu_action_names(WebView::ContextMenu& context_menu, Function<Optional<String>(u32, StringView)> callback);

}
