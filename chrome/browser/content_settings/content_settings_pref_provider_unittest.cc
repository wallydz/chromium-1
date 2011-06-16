// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/content_settings/content_settings_pref_provider.h"

#include "base/auto_reset.h"
#include "base/command_line.h"
#include "chrome/browser/content_settings/mock_settings_observer.h"
#include "chrome/browser/prefs/browser_prefs.h"
#include "chrome/browser/prefs/default_pref_store.h"
#include "chrome/browser/prefs/overlay_persistent_pref_store.h"
#include "chrome/browser/prefs/pref_service.h"
#include "chrome/browser/prefs/pref_service_mock_builder.h"
#include "chrome/browser/prefs/testing_pref_store.h"
#include "chrome/common/chrome_switches.h"
#include "chrome/common/pref_names.h"
#include "chrome/common/url_constants.h"
#include "chrome/test/testing_browser_process_test.h"
#include "chrome/test/testing_pref_service.h"
#include "chrome/test/testing_profile.h"
#include "content/browser/browser_thread.h"
#include "googleurl/src/gurl.h"
#include "testing/gtest/include/gtest/gtest.h"

using ::testing::_;

namespace content_settings {

class PrefDefaultProviderTest : public TestingBrowserProcessTest {
 public:
  PrefDefaultProviderTest()
      : ui_thread_(BrowserThread::UI, &message_loop_) {
  }

 protected:
  MessageLoop message_loop_;
  BrowserThread ui_thread_;
};

TEST_F(PrefDefaultProviderTest, DefaultValues) {
  TestingProfile profile;
  content_settings::PrefDefaultProvider provider(&profile);

  ASSERT_FALSE(
      provider.DefaultSettingIsManaged(CONTENT_SETTINGS_TYPE_COOKIES));

  // Check setting defaults.
  EXPECT_EQ(CONTENT_SETTING_ALLOW,
            provider.ProvideDefaultSetting(CONTENT_SETTINGS_TYPE_COOKIES));
  provider.UpdateDefaultSetting(CONTENT_SETTINGS_TYPE_COOKIES,
                                CONTENT_SETTING_BLOCK);
  EXPECT_EQ(CONTENT_SETTING_BLOCK,
            provider.ProvideDefaultSetting(CONTENT_SETTINGS_TYPE_COOKIES));
  provider.ResetToDefaults();
  EXPECT_EQ(CONTENT_SETTING_ALLOW,
            provider.ProvideDefaultSetting(CONTENT_SETTINGS_TYPE_COOKIES));
}

TEST_F(PrefDefaultProviderTest, Observer) {
  TestingProfile profile;
  PrefDefaultProvider provider(&profile);
  MockSettingsObserver observer;

  EXPECT_CALL(observer,
              OnContentSettingsChanged(profile.GetHostContentSettingsMap(),
                                       CONTENT_SETTINGS_TYPE_IMAGES, false,
                                       _, _, true));
  // Expect a second call because the PrefDefaultProvider in the TestingProfile
  // also observes the default content settings preference.
  EXPECT_CALL(observer,
              OnContentSettingsChanged(profile.GetHostContentSettingsMap(),
                                       CONTENT_SETTINGS_TYPE_DEFAULT, true,
                                       _, _, true));
  provider.UpdateDefaultSetting(
      CONTENT_SETTINGS_TYPE_IMAGES, CONTENT_SETTING_BLOCK);
}

TEST_F(PrefDefaultProviderTest, ObserveDefaultPref) {
  TestingProfile profile;
  PrefDefaultProvider provider(&profile);

  PrefService* prefs = profile.GetPrefs();

  // Make a copy of the default pref value so we can reset it later.
  scoped_ptr<Value> default_value(prefs->FindPreference(
      prefs::kDefaultContentSettings)->GetValue()->DeepCopy());

  provider.UpdateDefaultSetting(
      CONTENT_SETTINGS_TYPE_COOKIES, CONTENT_SETTING_BLOCK);
  EXPECT_EQ(CONTENT_SETTING_BLOCK,
            provider.ProvideDefaultSetting(CONTENT_SETTINGS_TYPE_COOKIES));

  // Make a copy of the pref's new value so we can reset it later.
  scoped_ptr<Value> new_value(prefs->FindPreference(
      prefs::kDefaultContentSettings)->GetValue()->DeepCopy());

  // Clearing the backing pref should also clear the internal cache.
  prefs->Set(prefs::kDefaultContentSettings, *default_value);
  EXPECT_EQ(CONTENT_SETTING_ALLOW,
            provider.ProvideDefaultSetting(CONTENT_SETTINGS_TYPE_COOKIES));

  // Reseting the pref to its previous value should update the cache.
  prefs->Set(prefs::kDefaultContentSettings, *new_value);
  EXPECT_EQ(CONTENT_SETTING_BLOCK,
            provider.ProvideDefaultSetting(CONTENT_SETTINGS_TYPE_COOKIES));
}

TEST_F(PrefDefaultProviderTest, OffTheRecord) {
  TestingProfile profile;
  PrefDefaultProvider provider(&profile);

  profile.set_incognito(true);
  PrefDefaultProvider otr_provider(&profile);
  profile.set_incognito(false);

  EXPECT_EQ(CONTENT_SETTING_ALLOW,
            provider.ProvideDefaultSetting(CONTENT_SETTINGS_TYPE_COOKIES));
  EXPECT_EQ(CONTENT_SETTING_ALLOW,
            otr_provider.ProvideDefaultSetting(CONTENT_SETTINGS_TYPE_COOKIES));

  // Changing content settings on the main provider should also affect the
  // incognito map.
  provider.UpdateDefaultSetting(CONTENT_SETTINGS_TYPE_COOKIES,
                                CONTENT_SETTING_BLOCK);
  EXPECT_EQ(CONTENT_SETTING_BLOCK,
            provider.ProvideDefaultSetting(CONTENT_SETTINGS_TYPE_COOKIES));
  EXPECT_EQ(CONTENT_SETTING_BLOCK,
            otr_provider.ProvideDefaultSetting(CONTENT_SETTINGS_TYPE_COOKIES));

  // Changing content settings on the incognito provider should be ignored.
  otr_provider.UpdateDefaultSetting(CONTENT_SETTINGS_TYPE_COOKIES,
                                   CONTENT_SETTING_ALLOW);
  EXPECT_EQ(CONTENT_SETTING_BLOCK,
            provider.ProvideDefaultSetting(CONTENT_SETTINGS_TYPE_COOKIES));
  EXPECT_EQ(CONTENT_SETTING_BLOCK,
            otr_provider.ProvideDefaultSetting(CONTENT_SETTINGS_TYPE_COOKIES));
}

// ////////////////////////////////////////////////////////////////////////////
// PrefProviderTest
//

bool SettingsEqual(const ContentSettings& settings1,
                   const ContentSettings& settings2) {
  for (int i = 0; i < CONTENT_SETTINGS_NUM_TYPES; ++i) {
    if (settings1.settings[i] != settings2.settings[i])
      return false;
  }
  return true;
}

class PrefProviderTest : public TestingBrowserProcessTest {
 public:
  PrefProviderTest() : ui_thread_(
      BrowserThread::UI, &message_loop_) {
  }

 protected:
  MessageLoop message_loop_;
  BrowserThread ui_thread_;
};

TEST_F(PrefProviderTest, Observer) {
  TestingProfile profile;
  profile.GetHostContentSettingsMap();
  Profile* p = &profile;
  PrefProvider pref_content_settings_provider(p);
  MockSettingsObserver observer;
  ContentSettingsPattern pattern =
      ContentSettingsPattern::FromString("[*.]example.com");

  // Expect 2 calls: One from the update and one from canonicalization.
  EXPECT_CALL(observer,
              OnContentSettingsChanged(profile.GetHostContentSettingsMap(),
                                       CONTENT_SETTINGS_TYPE_IMAGES,
                                       false,
                                       pattern,
                                       ContentSettingsPattern::Wildcard(),
                                       false));
  EXPECT_CALL(observer,
              OnContentSettingsChanged(profile.GetHostContentSettingsMap(),
                                       CONTENT_SETTINGS_TYPE_DEFAULT, true,
                                       _, _, true));
  pref_content_settings_provider.SetContentSetting(
      pattern,
      ContentSettingsPattern::Wildcard(),
      CONTENT_SETTINGS_TYPE_IMAGES,
      "",
      CONTENT_SETTING_ALLOW);
}

// Test for regression in which the PrefProvider modified the user pref store
// of the OTR unintentionally: http://crbug.com/74466.
TEST_F(PrefProviderTest, Incognito) {
  PersistentPrefStore* user_prefs = new TestingPrefStore();
  OverlayPersistentPrefStore* otr_user_prefs =
      new OverlayPersistentPrefStore(user_prefs);

  PrefServiceMockBuilder builder;
  PrefService* regular_prefs = builder.WithUserPrefs(user_prefs).Create();

  Profile::RegisterUserPrefs(regular_prefs);
  browser::RegisterUserPrefs(regular_prefs);

  PrefService* otr_prefs = builder.WithUserPrefs(otr_user_prefs).Create();

  Profile::RegisterUserPrefs(otr_prefs);
  browser::RegisterUserPrefs(otr_prefs);

  TestingProfile profile;
  TestingProfile* otr_profile = new TestingProfile;
  profile.SetOffTheRecordProfile(otr_profile);
  profile.SetPrefService(regular_prefs);
  otr_profile->set_incognito(true);
  otr_profile->SetPrefService(otr_prefs);
  profile.GetHostContentSettingsMap();

  PrefProvider pref_content_settings_provider(&profile);
  PrefProvider pref_content_settings_provider_incognito(otr_profile);
  ContentSettingsPattern pattern =
      ContentSettingsPattern::FromString("[*.]example.com");
  pref_content_settings_provider.SetContentSetting(
      pattern,
      pattern,
      CONTENT_SETTINGS_TYPE_IMAGES,
      "",
      CONTENT_SETTING_ALLOW);

  GURL host("http://example.com/");
  // The value should of course be visible in the regular PrefProvider.
  EXPECT_EQ(CONTENT_SETTING_ALLOW,
            pref_content_settings_provider.GetContentSetting(
                host, host, CONTENT_SETTINGS_TYPE_IMAGES, ""));
  // And also in the OTR version.
  EXPECT_EQ(CONTENT_SETTING_ALLOW,
            pref_content_settings_provider_incognito.GetContentSetting(
                host, host, CONTENT_SETTINGS_TYPE_IMAGES, ""));
  // But the value should not be overridden in the OTR user prefs accidentally.
  EXPECT_FALSE(otr_user_prefs->IsSetInOverlay(prefs::kContentSettingsPatterns));
}

TEST_F(PrefProviderTest, Patterns) {
  TestingProfile testing_profile;
  testing_profile.GetHostContentSettingsMap();
  PrefProvider pref_content_settings_provider(
      testing_profile.GetOriginalProfile());

  GURL host1("http://example.com/");
  GURL host2("http://www.example.com/");
  GURL host3("http://example.org/");
  GURL host4("file:///tmp/test.html");
  ContentSettingsPattern pattern1 =
      ContentSettingsPattern::FromString("[*.]example.com");
  ContentSettingsPattern pattern2 =
      ContentSettingsPattern::FromString("example.org");
  ContentSettingsPattern pattern3 =
      ContentSettingsPattern::FromString("file:///tmp/test.html");

  EXPECT_EQ(CONTENT_SETTING_DEFAULT,
            pref_content_settings_provider.GetContentSetting(
                host1, host1, CONTENT_SETTINGS_TYPE_IMAGES, ""));
  pref_content_settings_provider.SetContentSetting(
      pattern1,
      pattern1,
      CONTENT_SETTINGS_TYPE_IMAGES,
      "",
      CONTENT_SETTING_BLOCK);
  EXPECT_EQ(CONTENT_SETTING_BLOCK,
            pref_content_settings_provider.GetContentSetting(
                host1, host1, CONTENT_SETTINGS_TYPE_IMAGES, ""));
  EXPECT_EQ(CONTENT_SETTING_BLOCK,
            pref_content_settings_provider.GetContentSetting(
                host2, host2, CONTENT_SETTINGS_TYPE_IMAGES, ""));

  EXPECT_EQ(CONTENT_SETTING_DEFAULT,
            pref_content_settings_provider.GetContentSetting(
                host3, host3, CONTENT_SETTINGS_TYPE_IMAGES, ""));
  pref_content_settings_provider.SetContentSetting(
      pattern2,
      pattern2,
      CONTENT_SETTINGS_TYPE_IMAGES,
      "",
      CONTENT_SETTING_BLOCK);
  EXPECT_EQ(CONTENT_SETTING_BLOCK,
            pref_content_settings_provider.GetContentSetting(
                host3, host3, CONTENT_SETTINGS_TYPE_IMAGES, ""));

  EXPECT_EQ(CONTENT_SETTING_DEFAULT,
            pref_content_settings_provider.GetContentSetting(
                host4, host4, CONTENT_SETTINGS_TYPE_IMAGES, ""));
  pref_content_settings_provider.SetContentSetting(
      pattern3,
      pattern3,
      CONTENT_SETTINGS_TYPE_IMAGES,
      "",
      CONTENT_SETTING_BLOCK);
  EXPECT_EQ(CONTENT_SETTING_BLOCK,
            pref_content_settings_provider.GetContentSetting(
                host4, host4, CONTENT_SETTINGS_TYPE_IMAGES, ""));
}

TEST_F(PrefProviderTest, ResourceIdentifier) {
  // This feature is currently behind a flag.
  CommandLine* cmd = CommandLine::ForCurrentProcess();
  AutoReset<CommandLine> auto_reset(cmd, *cmd);
  cmd->AppendSwitch(switches::kEnableResourceContentSettings);

  TestingProfile testing_profile;
  testing_profile.GetHostContentSettingsMap();
  PrefProvider pref_content_settings_provider(
      testing_profile.GetOriginalProfile());

  GURL host("http://example.com/");
  ContentSettingsPattern pattern =
      ContentSettingsPattern::FromString("[*.]example.com");
  std::string resource1("someplugin");
  std::string resource2("otherplugin");

  EXPECT_EQ(CONTENT_SETTING_DEFAULT,
            pref_content_settings_provider.GetContentSetting(
                host, host, CONTENT_SETTINGS_TYPE_PLUGINS, resource1));
  pref_content_settings_provider.SetContentSetting(
      pattern,
      pattern,
      CONTENT_SETTINGS_TYPE_PLUGINS,
      resource1,
      CONTENT_SETTING_BLOCK);
  EXPECT_EQ(CONTENT_SETTING_BLOCK,
            pref_content_settings_provider.GetContentSetting(
                host, host, CONTENT_SETTINGS_TYPE_PLUGINS, resource1));
  EXPECT_EQ(CONTENT_SETTING_DEFAULT,
            pref_content_settings_provider.GetContentSetting(
                host, host, CONTENT_SETTINGS_TYPE_PLUGINS, resource2));
}

TEST_F(PrefProviderTest, MigrateSinglePatternSettings) {
  // Setup single pattern settings.
  TestingProfile profile;
  PrefService* prefs = profile.GetPrefs();

  DictionaryValue* settings_dictionary = new DictionaryValue();
  settings_dictionary->SetInteger("cookies", 2);
  settings_dictionary->SetInteger("images", 2);
  settings_dictionary->SetInteger("popups", 2);

  ContentSettingsPattern pattern =
      ContentSettingsPattern::FromString("http://www.example.com");
  DictionaryValue* all_settings_dictionary = new DictionaryValue();
  all_settings_dictionary->SetWithoutPathExpansion(
      pattern.ToString(), settings_dictionary);

  prefs->Set(prefs::kContentSettingsPatterns, *all_settings_dictionary);

  // Test if single pattern settings are properly migrated.
  content_settings::PrefProvider provider(profile.GetOriginalProfile());

  const DictionaryValue* const_all_settings_dictionary =
      prefs->GetDictionary(prefs::kContentSettingsPatterns);
  EXPECT_EQ(1U, const_all_settings_dictionary->size());
  EXPECT_FALSE(const_all_settings_dictionary->HasKey(pattern.ToString()));
  EXPECT_TRUE(const_all_settings_dictionary->HasKey(
      pattern.ToString() + "," +
      ContentSettingsPattern::Wildcard().ToString()));

  // TODO test provider
  EXPECT_EQ(CONTENT_SETTING_BLOCK,  provider.GetContentSetting(
      GURL("http://www.example.com"),
      GURL("http://www.example.com"),
      CONTENT_SETTINGS_TYPE_IMAGES,
      ""));

  EXPECT_EQ(CONTENT_SETTING_BLOCK,  provider.GetContentSetting(
      GURL("http://www.example.com"),
      GURL("http://www.example.com"),
      CONTENT_SETTINGS_TYPE_POPUPS,
      ""));
}

}  // namespace content_settings
