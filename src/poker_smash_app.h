// poker_smash - ReXGlue Recompiled Project
//
// App hooks: default paths, GPU plugin, full-game license, first-launch
// defaults, and the F1/Esc settings menu (src/game_menu.*).

#pragma once

#include <cstdlib>
#include <filesystem>

#include <rex/cvar.h>
#include <rex/filesystem.h>
#include <rex/filesystem/vfs.h>
#include <rex/input/input_system.h>
#include <rex/logging.h>
#include <rex/rex_app.h>
#include <rex/runtime.h>
#include <rex/ui/keybinds.h>

#include <imgui.h>

#include "game_menu.h"
#include "package_setup.h"

void InstallCrashHandler();  // src/crash_handler.cpp

class PokerSmashApp : public rex::ReXApp {
 public:
  using rex::ReXApp::ReXApp;

  static std::unique_ptr<rex::ui::WindowedApp> Create(
      rex::ui::WindowedAppContext& ctx) {
    return std::unique_ptr<PokerSmashApp>(new PokerSmashApp(ctx, "poker_smash",
        PPCImageConfig));
  }

  // Default the game data folder to "assets" next to the exe, so the game
  // runs with a plain double-click. --game_data_root still overrides it.
  void OnConfigurePaths(rex::PathConfig& paths) override {
    if (paths.game_data_root.empty()) {
      paths.game_data_root =
          rex::filesystem::GetExecutableFolder() / "assets";
      // First start: make "assets" from the player's own XBLA package next to the exe.
      if (!package_setup_first_run(rex::filesystem::GetExecutableFolder(),
                                   paths.game_data_root, 0x5841089Cu,
                                   "Poker Smash")) {
        std::exit(1);
      }
    }
    // Only the exact executable this build was recompiled from can run.
    if (!xex_version_check(paths.game_data_root, "9f427e7e8cc3fb48d5d5a2a61db4c0a14af93b7c", 7544832u, "Poker Smash")) {
      std::exit(1);
    }
    config_path_ = paths.config_path;
  }

  void OnPreSetup(rex::RuntimeConfig& config) override {
    // GPU emulation is opt-in in ReXGlue (gpu_plugin cvar). Default to the
    // Xenos plugin so the game actually renders; --gpu_plugin still overrides.
    if (config.gpu_plugin.empty()) {
      config.gpu_plugin = "xenos";
    }
    // XBLA titles run as the trial ("locked") unless XamContentGetLicenseMask
    // reports a purchase; bit 0 = full game. ReXGlue's license_mask cvar
    // defaults to 0 (trial), so unlock unless the user set it explicitly.
    if (rex::cvar::Query<uint32_t>("license_mask") == 0) {
      rex::cvar::SetFlagByName("license_mask", "1");
      REXLOG_INFO("License: full game (license_mask=1)");
    }
    // First launch: windowed, keyboard on, arcade-style key layout. After
    // this everything comes from poker_smash.toml (edited by the F1 menu).
    if (!rex::cvar::Query<bool>("ps_defaults_applied")) {
      rex::cvar::SetFlagByName("fullscreen", "false");
      rex::cvar::SetFlagByName("window_width", "1280");
      rex::cvar::SetFlagByName("window_height", "720");
      rex::cvar::SetFlagByName("mnk_mode", "true");
      poker_smash::GameMenu::ApplyDefaultKeys();
      rex::cvar::SetFlagByName("ps_defaults_applied", "true");
      if (!config_path_.empty()) {
        rex::cvar::SaveConfig(config_path_);
      }
      REXLOG_INFO("First launch: wrote default settings to {}",
                  config_path_.filename().string());
    }
  }

  // Poker Smash opens some content with bare relative paths ("data\\textures\\").
  // Map the "data" prefix onto the game partition so those resolve.
  void OnPostLoadXexImage() override {
    auto* fs = runtime()->file_system();
    if (fs && fs->RegisterSymbolicLink("data", "\\Device\\Harddisk0\\Partition1\\data")) {
      REXLOG_INFO("Registered relative 'data' path -> game:\\data");
    }
  }

  void OnCreateDialogs(rex::ui::ImGuiDrawer* drawer) override {
    (void)drawer;
    rex::ui::RegisterBind("bind_menu", "F1", "Open/close the settings menu",
                          [this] { ToggleMenu(); });
    rex::ui::RegisterBind("bind_menu_alt", "Escape", "Open/close the settings menu",
                          [this] { ToggleMenu(); });
    rex::ui::RegisterBind("bind_fullscreen", "F11", "Toggle fullscreen", [this] {
      if (menu_ && menu_->capturing()) return;
      bool fs = !window()->IsFullscreen();
      rex::cvar::SetFlagByName("fullscreen", fs ? "true" : "false");
      window()->SetFullscreen(fs);
      if (!config_path_.empty()) rex::cvar::SaveConfig(config_path_);
    });
  }

  // The game must not see keys/buttons while the menu is open.
  void OnPostSetup() override {
    InstallCrashHandler();  // re-assert after SDL/runtime init
    auto* input_sys = static_cast<rex::input::InputSystem*>(runtime()->input_system());
    if (input_sys && imgui_drawer()) {
      input_sys->SetActiveCallback([this]() {
        if (menu_) return false;
        return !imgui_drawer()->GetIO().WantCaptureMouse;
      });
    }
  }

 private:
  void ToggleMenu() {
    if (menu_) {
      if (menu_->capturing()) return;  // Esc cancels the key capture instead
      delete menu_;                    // removes itself from the drawer
      menu_ = nullptr;
      return;
    }
    if (!imgui_drawer()) return;
    poker_smash::MenuHost host;
    host.config_path = config_path_;
    host.set_fullscreen = [this](bool fs) {
      app_context().CallInUIThreadDeferred([this, fs] { window()->SetFullscreen(fs); });
    };
    host.quit = [this] {
      app_context().CallInUIThreadDeferred([this] { window()->RequestClose(); });
    };
    host.on_closed = [this] { menu_ = nullptr; };
    menu_ = new poker_smash::GameMenu(imgui_drawer(), std::move(host));
  }

  std::filesystem::path config_path_;
  poker_smash::GameMenu* menu_ = nullptr;  // owned by the ImGui drawer
};
