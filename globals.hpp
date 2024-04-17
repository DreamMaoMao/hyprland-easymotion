#pragma once
#include <list>

#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/Window.hpp>
#include <hyprland/src/config/ConfigManager.hpp>
#include "easymotionDeco.hpp"
#include "log.hpp"

inline HANDLE PHANDLE = nullptr;
inline CFunctionHook* g_hyeasymotion_pCWindow_onUnmap = nullptr;

class CHyprEasyLabel;

struct SGlobalState {
    std::vector<CHyprEasyLabel*>   motionLabels;
};

inline std::unique_ptr<SGlobalState> g_pGlobalState = std::make_unique<SGlobalState>();

