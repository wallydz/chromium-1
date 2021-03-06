// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/test/test_web_contents_view.h"

namespace content {

TestWebContentsView::TestWebContentsView() {
}

TestWebContentsView::~TestWebContentsView() {
}

void TestWebContentsView::StartDragging(
    const DropData& drop_data,
    blink::WebDragOperationsMask allowed_ops,
    const gfx::ImageSkia& image,
    const gfx::Vector2d& image_offset,
    const DragEventSourceInfo& event_info) {
}

void TestWebContentsView::UpdateDragCursor(blink::WebDragOperation operation) {
}

void TestWebContentsView::GotFocus() {
}

void TestWebContentsView::TakeFocus(bool reverse) {
}

gfx::NativeView TestWebContentsView::GetNativeView() const {
  return gfx::NativeView();
}

gfx::NativeView TestWebContentsView::GetContentNativeView() const {
  return gfx::NativeView();
}

gfx::NativeWindow TestWebContentsView::GetTopLevelNativeWindow() const {
  return gfx::NativeWindow();
}

void TestWebContentsView::GetContainerBounds(gfx::Rect *out) const {
}

void TestWebContentsView::OnTabCrashed(base::TerminationStatus status,
                                       int error_code) {
}

void TestWebContentsView::SizeContents(const gfx::Size& size) {
}

void TestWebContentsView::Focus() {
}

void TestWebContentsView::SetInitialFocus() {
}

void TestWebContentsView::StoreFocus() {
}

void TestWebContentsView::RestoreFocus() {
}

DropData* TestWebContentsView::GetDropData() const {
  return NULL;
}

gfx::Rect TestWebContentsView::GetViewBounds() const {
  return gfx::Rect();
}

#if defined(OS_MACOSX)
void TestWebContentsView::SetAllowOverlappingViews(bool overlapping) {
}

bool TestWebContentsView::GetAllowOverlappingViews() const {
  return false;
}

void TestWebContentsView::SetOverlayView(
    WebContentsView* overlay, const gfx::Point& offset) {
}

void TestWebContentsView::RemoveOverlayView() {
}
#endif

void TestWebContentsView::CreateView(const gfx::Size& initial_size,
                                     gfx::NativeView context) {
}

RenderWidgetHostView* TestWebContentsView::CreateViewForWidget(
    RenderWidgetHost* render_widget_host) {
  return NULL;
}

RenderWidgetHostView* TestWebContentsView::CreateViewForPopupWidget(
    RenderWidgetHost* render_widget_host) {
  return NULL;
}

void TestWebContentsView::SetPageTitle(const base::string16& title) {
}

void TestWebContentsView::RenderViewCreated(RenderViewHost* host) {
}

void TestWebContentsView::RenderViewSwappedIn(RenderViewHost* host) {
}

void TestWebContentsView::SetOverscrollControllerEnabled(bool enabled) {
}

#if defined(OS_MACOSX)
bool TestWebContentsView::IsEventTracking() const {
  return false;
}

void TestWebContentsView::CloseTabAfterEventTracking() {
}
#endif

}  // namespace content
