/*
 * Copyright (c) 2022, MacDue <macdue@dueutil.tech>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "WebContentView.h"
#include <LibGfx/Point.h>
#include <LibWebView/Forward.h>
#include <QWidget>

class QMenu;

namespace Ladybird {

class WebContentView;

class InspectorWidget final : public QWidget {
    Q_OBJECT

public:
    InspectorWidget(QWidget* tab, WebContentView& content_view);
    virtual ~InspectorWidget() override;

    void inspect();
    void reset();

    void select_hovered_node();
    void select_default_node();

public slots:
    void device_pixel_ratio_changed(qreal dpi);

private:
    bool event(QEvent*) override;
    void closeEvent(QCloseEvent*) override;

    QScreen* m_current_screen;
    double m_device_pixel_ratio { 0 };

    WebContentView* m_inspector_view;
    OwnPtr<WebView::InspectorClient> m_inspector_client;

    QMenu* m_dom_node_text_context_menu { nullptr };
    QMenu* m_dom_node_tag_context_menu { nullptr };
    QMenu* m_dom_node_attribute_context_menu { nullptr };
};

}
