
#include "evenhook.hpp"

// Do NOT change this function.
APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

void addEasyMotionKeybinds()
{
	g_pKeybindManager->addKeybind(SKeybind{"escape", 0, 0, 0, "easymotionexit", "", 0, "__easymotionsubmap__", 0, 0, 0, 0, 0, 0});

	// hycov shortcut
	g_pKeybindManager->addKeybind(SKeybind{"tab", 0, 0, g_pKeybindManager->stringToModMask("alt"), "hycov:leaveoverview", "", 0, "__easymotionsubmap__", 0, 0, 0, 0, 0, 0});
	g_pKeybindManager->addKeybind(SKeybind{"tab", 0, 0, g_pKeybindManager->stringToModMask("alt"), "easymotionexit", "", 0, "__easymotionsubmap__", 0, 0, 0, 0, 0, 0});
	g_pKeybindManager->addKeybind(SKeybind{"q", 0, 0, g_pKeybindManager->stringToModMask("alt"), "killactive", "", 0, "__easymotionsubmap__", 0, 0, 0, 0, 0, 0});
	g_pKeybindManager->addKeybind(SKeybind{"left", 0, 0, g_pKeybindManager->stringToModMask("alt"), "hycov:movefocus", "leftcross", 0, "__easymotionsubmap__", 0, 0, 0, 0, 0, 0});
	g_pKeybindManager->addKeybind(SKeybind{"right", 0, 0, g_pKeybindManager->stringToModMask("alt"), "hycov:movefocus", "rightcross", 0, "__easymotionsubmap__", 0, 0, 0, 0, 0, 0});
	g_pKeybindManager->addKeybind(SKeybind{"up", 0, 0, g_pKeybindManager->stringToModMask("alt"), "hycov:movefocus", "upcross", 0, "__easymotionsubmap__", 0, 0, 0, 0, 0, 0});
	g_pKeybindManager->addKeybind(SKeybind{"down", 0, 0, g_pKeybindManager->stringToModMask("alt"), "hycov:movefocus", "downcross", 0, "__easymotionsubmap__", 0, 0, 0, 0, 0, 0});

	//catchall
	g_pKeybindManager->addKeybind(SKeybind{"", 0, 1, 0, "", "", 0, "__easymotionsubmap__", 0, 0, 0, 0, 0, 0});
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    PHANDLE = handle;

	HyprlandAPI::addConfigValue(PHANDLE, "plugin:easymotion:textsize", Hyprlang::INT{15});

	HyprlandAPI::addConfigValue(PHANDLE, "plugin:easymotion:textcolor", Hyprlang::INT{configStringToInt("rgba(ffffffff)")});
	HyprlandAPI::addConfigValue(PHANDLE, "plugin:easymotion:bgcolor", Hyprlang::INT{configStringToInt("rgba(000000ff)")});
	HyprlandAPI::addConfigValue(PHANDLE, "plugin:easymotion:textfont", Hyprlang::STRING{"Sans"});
	HyprlandAPI::addConfigValue(PHANDLE, "plugin:easymotion:textpadding", Hyprlang::STRING{"0"});
	HyprlandAPI::addConfigValue(PHANDLE, "plugin:easymotion:bordersize", Hyprlang::INT{0});
	HyprlandAPI::addConfigValue(PHANDLE, "plugin:easymotion:bordercolor", Hyprlang::STRING{"rgba(ffffffff)"});
	HyprlandAPI::addConfigValue(PHANDLE, "plugin:easymotion:rounding", Hyprlang::INT{0});
	HyprlandAPI::addConfigValue(PHANDLE, "plugin:easymotion:motionkeys", Hyprlang::STRING{"abcdefghijklmnopqrstuvwxyz1234567890"});

	registerDispatchers();
	registerEventHook();

	HyprlandAPI::registerCallbackDynamic(PHANDLE, "configReloaded", [&](void *self, SCallbackInfo&, std::any data) {addEasyMotionKeybinds();});
    HyprlandAPI::reloadConfig();

    return {"hypreasymotion", "Easymotion navigation", "Zakk", "1.0"};
}

APICALL EXPORT void PLUGIN_EXIT() {
}
