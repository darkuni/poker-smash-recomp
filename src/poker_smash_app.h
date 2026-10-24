// poker_smash - ReXGlue Recompiled Project
//
// Customize your app by overriding virtual hooks from rex::ReXApp.

#pragma once

#include <rex/filesystem.h>
#include <rex/rex_app.h>
#include <rex/runtime.h>
#include <rex/filesystem/vfs.h>
#include <rex/logging.h>

class PokerSmashApp : public rex::ReXApp {
 public:
  using rex::ReXApp::ReXApp;

  static std::unique_ptr<rex::ui::WindowedApp> Create(
      rex::ui::WindowedAppContext& ctx) {
    return std::unique_ptr<PokerSmashApp>(new PokerSmashApp(ctx, "poker_smash",
        PPCImageConfig));
  }

  // Override virtual hooks for customization:
  // void OnPostInitLogging() override {}
  // GPU emulation is opt-in in ReXGlue (gpu_plugin cvar). Default to the
  // Xenos plugin so the game actually renders; --gpu_plugin still overrides.
  void OnPreSetup(rex::RuntimeConfig& config) override {
    if (config.gpu_plugin.empty()) {
      config.gpu_plugin = "xenos";
    }
  }
  // void OnLoadXexImage(std::string& xex_image) override {}
  // Poker Smash opens some content with bare relative paths ("data\\textures\\").
  // Map the "data" prefix onto the game partition so those resolve.
  void OnPostLoadXexImage() override {
    auto* fs = runtime()->file_system();
    if (fs && fs->RegisterSymbolicLink("data", "\\Device\\Harddisk0\\Partition1\\data")) {
      REXLOG_INFO("Registered relative 'data' path -> game:\\data");
    }
  }
  // void OnPostSetup() override {}
  // void OnCreateDialogs(rex::ui::ImGuiDrawer* drawer) override {}
  // std::unique_ptr<rex::ui::ImGuiDialog> CreateAchievementsOverlay() override;
  // std::unique_ptr<rex::ui::AchievementNotificationDialog>
  // CreateAchievementNotificationDialog() override;
  // void OnShutdown() override {}
  // Default the game data folder to "assets" next to the exe, so the game
  // runs with a plain double-click. --game_data_root still overrides it.
  void OnConfigurePaths(rex::PathConfig& paths) override {
    if (paths.game_data_root.empty()) {
      paths.game_data_root =
          rex::filesystem::GetExecutableFolder() / "assets";
    }
  }
};
