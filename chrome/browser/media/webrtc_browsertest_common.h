// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_MEDIA_WEBRTC_BROWSERTEST_COMMON_H_
#define CHROME_BROWSER_MEDIA_WEBRTC_BROWSERTEST_COMMON_H_

#include <string>

#include "base/files/file_path.h"
#include "base/process/process_handle.h"

namespace content {
class WebContents;
}

namespace test {

// Reference video locations.

// Checks if the user has the reference video files, returns true if so.
// If the user's checkout don't have these dirs, they need to configure their
// .gclient as described in kAdviseOnGclientSolution. The reason for this is
// that we don't want to burden regular chrome devs with downloading these
// sizable reference files by default.
bool HasReferenceFilesInCheckout();

// Retrieves the reference video dir, to which file names can be appended.
base::FilePath GetReferenceVideosDir();

extern const base::FilePath::CharType kReferenceFileName360p[];
extern const base::FilePath::CharType kYuvFileExtension[];
extern const base::FilePath::CharType kY4mFileExtension[];

// Executes javascript code which will sleep for |timeout_msec| milliseconds.
// Returns true on success.
bool SleepInJavascript(content::WebContents* tab_contents, int timeout_msec);

// This function will execute the provided |javascript| until it causes a call
// to window.domAutomationController.send() with |evaluates_to| as the message.
// That is, we are NOT checking what the javascript evaluates to. Returns false
// if we exceed the TestTimeouts::action_max_timeout().
// TODO(phoglund): Consider a better interaction method with the javascript
// than polling javascript methods.
bool PollingWaitUntil(const std::string& javascript,
                      const std::string& evaluates_to,
                      content::WebContents* tab_contents);
bool PollingWaitUntil(const std::string& javascript,
                      const std::string& evaluates_to,
                      content::WebContents* tab_contents,
                      int poll_interval_msec);

class PeerConnectionServerRunner {
 public:
  static const char kDefaultPort[];

  PeerConnectionServerRunner(): server_pid_(0) {}
  ~PeerConnectionServerRunner() {}

  // Starts the peerconnection server on localhost on |kDefaultPort|.
  bool Start();

  // Stops the peerconnection server.
  bool Stop();

  static void KillAllPeerConnectionServers();

 private:
  base::ProcessHandle server_pid_;
};

}  // namespace test

#endif  // CHROME_BROWSER_MEDIA_WEBRTC_BROWSERTEST_COMMON_H_
