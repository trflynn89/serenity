/*
 * Copyright (c) 2022, MacDue <macdue@dueutil.tech>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "InspectorWidget.h"
#include <Ladybird/Qt/ContextMenu.h>
#include <Ladybird/Qt/StringUtils.h>
#include <LibWebView/Attribute.h>
#include <LibWebView/ContextMenu.h>
#include <LibWebView/InspectorClient.h>
#include <QAction>
#include <QCloseEvent>
#include <QGuiApplication>
#include <QMenu>
#include <QVBoxLayout>
#include <QWindow>

namespace Ladybird {

extern bool is_using_dark_system_theme(QWidget&);

InspectorWidget::InspectorWidget(QWidget* tab, WebContentView& content_view)
    : QWidget(tab, Qt::Window)
{
    m_inspector_view = new WebContentView(this, {}, {});

    if (is_using_dark_system_theme(*this))
        m_inspector_view->update_palette(WebContentView::PaletteMode::Dark);

    m_inspector_client = make<WebView::InspectorClient>(content_view, *m_inspector_view);

    m_inspector_client->on_requested_dom_node_text_context_menu = [this](auto position) {
        if (!m_dom_node_text_context_menu)
            m_dom_node_text_context_menu = create_context_menu(this, m_inspector_client->dom_node_text_context_menu());

        update_context_menu_action_names(m_inspector_client->dom_node_text_context_menu(), [&](auto id, auto name_format) -> Optional<String> {
            switch (static_cast<WebView::InspectorClient::ContextMenuActionIDs>(id)) {
            case WebView::InspectorClient::ContextMenuActionIDs::EditDOMNode:
            case WebView::InspectorClient::ContextMenuActionIDs::CopyDOMNode:
                return MUST(String::formatted(name_format, "text"sv));
            default:
                return {};
            }
        });

        m_dom_node_text_context_menu->exec(m_inspector_view->map_point_to_global_position(position));
    };

    m_inspector_client->on_requested_dom_node_tag_context_menu = [this](auto position, auto const& tag) {
        if (!m_dom_node_tag_context_menu)
            m_dom_node_tag_context_menu = create_context_menu(this, m_inspector_client->dom_node_tag_context_menu());

        update_context_menu_action_names(m_inspector_client->dom_node_tag_context_menu(), [&](auto id, auto name_format) -> Optional<String> {
            switch (static_cast<WebView::InspectorClient::ContextMenuActionIDs>(id)) {
            case WebView::InspectorClient::ContextMenuActionIDs::EditDOMNode:
                return MUST(String::formatted(name_format, tag));
            case WebView::InspectorClient::ContextMenuActionIDs::CopyDOMNode:
                return "&Copy HTML"_string;
            default:
                return {};
            }
        });

        m_dom_node_tag_context_menu->exec(m_inspector_view->map_point_to_global_position(position));
    };

    m_inspector_client->on_requested_dom_node_attribute_context_menu = [this](auto position, auto const&, WebView::Attribute const& attribute) {
        static constexpr size_t MAX_ATTRIBUTE_VALUE_LENGTH = 32;

        if (!m_dom_node_attribute_context_menu)
            m_dom_node_attribute_context_menu = create_context_menu(this, m_inspector_client->dom_node_attribute_context_menu());

        auto attribute_name = MUST(String::formatted("attribute \"{}\"", attribute.name));

        auto attribute_value = MUST(String::formatted("{:.{}}{}",
            attribute.value, MAX_ATTRIBUTE_VALUE_LENGTH,
            attribute.value.bytes_as_string_view().length() > MAX_ATTRIBUTE_VALUE_LENGTH ? "..."sv : ""sv));

        update_context_menu_action_names(m_inspector_client->dom_node_attribute_context_menu(), [&](auto id, auto name_format) -> Optional<String> {
            switch (static_cast<WebView::InspectorClient::ContextMenuActionIDs>(id)) {
            case WebView::InspectorClient::ContextMenuActionIDs::EditDOMNode:
            case WebView::InspectorClient::ContextMenuActionIDs::RemoveDOMNodeAttribute:
                return MUST(String::formatted(name_format, attribute_name));
            case WebView::InspectorClient::ContextMenuActionIDs::CopyDOMNode:
                return "&Copy HTML"_string;
            case WebView::InspectorClient::ContextMenuActionIDs::CopyDOMNodeAttributeValue:
                return MUST(String::formatted(name_format, attribute_value));
            default:
                return {};
            }
        });

        m_dom_node_attribute_context_menu->exec(m_inspector_view->map_point_to_global_position(position));
    };

    setLayout(new QVBoxLayout);
    layout()->addWidget(m_inspector_view);

    setWindowTitle("Inspector");
    resize(875, 825);

    // Listen for DPI changes
    m_device_pixel_ratio = devicePixelRatio();
    m_current_screen = screen();
    if (QT_VERSION < QT_VERSION_CHECK(6, 6, 0) || QGuiApplication::platformName() != "wayland") {
        setAttribute(Qt::WA_NativeWindow);
        setAttribute(Qt::WA_DontCreateNativeAncestors);
        QObject::connect(m_current_screen, &QScreen::logicalDotsPerInchChanged, this, &InspectorWidget::device_pixel_ratio_changed);
        QObject::connect(windowHandle(), &QWindow::screenChanged, this, [this](QScreen* screen) {
            if (m_device_pixel_ratio != screen->devicePixelRatio())
                device_pixel_ratio_changed(screen->devicePixelRatio());

            // Listen for logicalDotsPerInchChanged signals on new screen
            QObject::disconnect(m_current_screen, &QScreen::logicalDotsPerInchChanged, nullptr, nullptr);
            m_current_screen = screen;
            QObject::connect(m_current_screen, &QScreen::logicalDotsPerInchChanged, this, &InspectorWidget::device_pixel_ratio_changed);
        });
    }
}

InspectorWidget::~InspectorWidget() = default;

void InspectorWidget::inspect()
{
    m_inspector_client->inspect();
}

void InspectorWidget::reset()
{
    m_inspector_client->reset();
}

void InspectorWidget::select_hovered_node()
{
    m_inspector_client->select_hovered_node();
}

void InspectorWidget::select_default_node()
{
    m_inspector_client->select_default_node();
}

void InspectorWidget::device_pixel_ratio_changed(qreal dpi)
{
    m_device_pixel_ratio = dpi;
    m_inspector_view->set_device_pixel_ratio(m_device_pixel_ratio);
}

bool InspectorWidget::event(QEvent* event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
    if (event->type() == QEvent::DevicePixelRatioChange) {
        if (m_device_pixel_ratio != devicePixelRatio())
            device_pixel_ratio_changed(devicePixelRatio());
    }
#endif

    return QWidget::event(event);
}

void InspectorWidget::closeEvent(QCloseEvent* event)
{
    event->accept();
    m_inspector_client->clear_selection();
}

}
