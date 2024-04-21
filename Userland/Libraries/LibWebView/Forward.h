/*
 * Copyright (c) 2022, The SerenityOS developers
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Traits.h>

namespace WebView {

class ContextMenu;
class CookieJar;
class Database;
class InspectorClient;
class OutOfProcessWebView;
class ProcessManager;
class ViewImplementation;
class WebContentClient;

struct Action;
struct Attribute;
struct CookieStorageKey;
struct ProcessHandle;
struct SearchEngine;

}

namespace AK {

template<>
struct Traits<WebView::CookieStorageKey>;

}
