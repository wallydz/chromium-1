# Copyright 2013 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'include_dirs': [
    '..',
  ],
  'defines': [
    'SYNC_IMPLEMENTATION',
  ],
  'dependencies': [
    '../base/base.gyp:base',
    '../base/base.gyp:test_support_base',
  ],
  'sources': [
    'api/attachments/attachment.cc',
    'api/attachments/attachment.h',
    'api/attachments/attachment_id.cc',
    'api/attachments/attachment_id.h',
    'api/attachments/attachment_service.cc',
    'api/attachments/attachment_service.h',
    'api/attachments/attachment_service_proxy.cc',
    'api/attachments/attachment_service_proxy.h',
    'api/attachments/attachment_store.cc',
    'api/attachments/attachment_store.h',
    'api/attachments/fake_attachment_service.cc',
    'api/attachments/fake_attachment_service.h',
    'api/attachments/fake_attachment_store.cc',
    'api/attachments/fake_attachment_store.h',
    'api/string_ordinal.h',
    'api/syncable_service.cc',
    'api/syncable_service.h',
    'api/sync_data.cc',
    'api/sync_data.h',
    'api/sync_change.cc',
    'api/sync_change.h',
    'api/sync_change_processor.cc',
    'api/sync_change_processor.h',
    'api/sync_error.cc',
    'api/sync_error.h',
    'api/sync_error_factory.cc',
    'api/sync_error_factory.h',
    'api/sync_merge_result.cc',
    'api/sync_merge_result.h',
    'api/time.h',
  ],
}
