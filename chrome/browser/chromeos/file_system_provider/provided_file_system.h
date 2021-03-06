// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_CHROMEOS_FILE_SYSTEM_PROVIDER_PROVIDED_FILE_SYSTEM_H_
#define CHROME_BROWSER_CHROMEOS_FILE_SYSTEM_PROVIDER_PROVIDED_FILE_SYSTEM_H_

#include <string>

#include "base/files/file_path.h"

namespace chromeos {
namespace file_system_provider {

// Contains information about the provided file system instance.
class ProvidedFileSystem {
 public:
  ProvidedFileSystem();
  ProvidedFileSystem(const std::string& extension_id,
                     int file_system_id,
                     const std::string& file_system_name,
                     const base::FilePath& mount_path);

  ~ProvidedFileSystem();

  const std::string& extension_id() const { return extension_id_; }
  int file_system_id() const { return file_system_id_; }
  const std::string& file_system_name() const { return file_system_name_; }
  const base::FilePath& mount_path() const { return mount_path_; }

 private:
  // ID of the extension providing this file system.
  std::string extension_id_;

  // ID of the file system, used internally.
  int file_system_id_;

  // Name of the file system, can be rendered in the UI.
  std::string file_system_name_;

  // Mount path of the underlying file system.
  base::FilePath mount_path_;
};

}  // namespace file_system_provider
}  // namespace chromeos

#endif  // CHROME_BROWSER_CHROMEOS_FILE_SYSTEM_PROVIDER_PROVIDED_FILE_SYSTEM_H_
