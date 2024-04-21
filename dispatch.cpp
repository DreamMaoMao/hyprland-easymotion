#include "dispatch.hpp"
#include <unistd.h>
#include <any>
#include <ranges>

void addLabelToWindow(CWindow *window, SMotionActionDesc *actionDesc, std::string &label)
{
	std::unique_ptr<CHyprEasyLabel> motionlabel = std::make_unique<CHyprEasyLabel>(window, actionDesc);
	auto pMl =  motionlabel.get();
	pMl->m_szLabel = label;
	g_pGlobalState->motionLabels.push_back(pMl);
	HyprlandAPI::addWindowDecoration(PHANDLE, window, std::move(motionlabel));
	hypreasymotion_log(LOG,"add label:{},owner:{}",label,window);
}


static bool parseBorderGradient(std::string VALUE, CGradientValueData *DATA) {
    std::string V = VALUE;

    CVarList   varlist(V, 0, ' ');
    DATA->m_vColors.clear();

    std::string parseError = "";

    for (auto& var : varlist) {
        if (var.find("deg") != std::string::npos) {
            // last arg
            try {
                DATA->m_fAngle = std::stoi(var.substr(0, var.find("deg"))) * (PI / 180.0); // radians
            } catch (...) {
                Debug::log(WARN, "Error parsing gradient {}", V);
								return false;
            }

            break;
        }

        if (DATA->m_vColors.size() >= 10) {
            Debug::log(WARN, "Error parsing gradient {}: max colors is 10.", V);
						return false;
            break;
        }

        try {
            DATA->m_vColors.push_back(CColor(configStringToInt(var)));
        } catch (std::exception& e) {
            Debug::log(WARN, "Error parsing gradient {}", V);
        }
    }

    if (DATA->m_vColors.size() == 0) {
        Debug::log(WARN, "Error parsing gradient {}", V);
        DATA->m_vColors.push_back(0); // transparent
    }

    return true;
}


void easymotionExitDispatch(std::string args)
{
		for (auto &ml : g_pGlobalState->motionLabels | std::ranges::views::reverse) {
			ml->getOwner()->removeWindowDeco(ml);
		}
		g_pGlobalState->motionLabels.clear();
		HyprlandAPI::invokeHyprctlCommand("dispatch", "submap reset");

}

void easymotionActionDispatch(std::string args)
{
	for (auto &ml : g_pGlobalState->motionLabels) {
		if (ml->m_szLabel == args) {
			g_pCompositor->focusWindow(ml->getOwner());
			g_pKeybindManager->m_mDispatchers["exec"](ml->m_szActionCmd);
			easymotionExitDispatch("");
			break;
		}
	}
}

void easymotionDispatch(std::string args)
{
	unsigned int CLIENTNUM = 0;

    for (auto &w : g_pCompositor->m_vWindows){
		CWindow *pWindow = w.get();
		if (pWindow->m_pWorkspace == g_pCompositor->m_pLastMonitor->activeWorkspace && !pWindow->isHidden() && pWindow->m_bIsMapped && !pWindow->m_bFadingOut) {
			CLIENTNUM++;
		}
	}

	if (CLIENTNUM == 0) {
		return;
	}

	static auto *const TEXTSIZE = (Hyprlang::INT* const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:easymotion:textsize")->getDataStaticPtr();
	static auto *const TEXTCOLOR = (Hyprlang::INT* const *)HyprlandAPI::getConfigValue(PHANDLE, "plugin:easymotion:textcolor")->getDataStaticPtr();
	static auto *const BGCOLOR = (Hyprlang::INT* const *)HyprlandAPI::getConfigValue(PHANDLE, "plugin:easymotion:bgcolor")->getDataStaticPtr();
	static auto *const TEXTFONT = (Hyprlang::STRING const *)HyprlandAPI::getConfigValue(PHANDLE, "plugin:easymotion:textfont")->getDataStaticPtr();
	static auto *const TEXTPADDING = (Hyprlang::STRING const *)HyprlandAPI::getConfigValue(PHANDLE, "plugin:easymotion:textpadding")->getDataStaticPtr();
	static auto *const BORDERSIZE = (Hyprlang::INT* const *)HyprlandAPI::getConfigValue(PHANDLE, "plugin:easymotion:bordersize")->getDataStaticPtr();
	static auto *const BORDERCOLOR = (Hyprlang::STRING const *)HyprlandAPI::getConfigValue(PHANDLE, "plugin:easymotion:bordercolor")->getDataStaticPtr();
	static auto *const ROUNDING = (Hyprlang::INT* const *)HyprlandAPI::getConfigValue(PHANDLE, "plugin:easymotion:rounding")->getDataStaticPtr();
	static auto *const MOTIONKEYS = (Hyprlang::STRING const *)HyprlandAPI::getConfigValue(PHANDLE, "plugin:easymotion:motionkeys")->getDataStaticPtr();

	CVarList emargs(args, 0, ',');
	SMotionActionDesc actionDesc;

	actionDesc.textSize = **TEXTSIZE;
	actionDesc.textColor = **TEXTCOLOR;
	actionDesc.backgroundColor = **BGCOLOR;
	actionDesc.textFont = *TEXTFONT;
	CVarList cpadding = CVarList(*TEXTPADDING);
	actionDesc.boxPadding.parseGapData(cpadding);
	actionDesc.rounding = **ROUNDING;
	actionDesc.borderSize = **BORDERSIZE;
	if(!parseBorderGradient(*BORDERCOLOR, &actionDesc.borderColor)) {
		actionDesc.borderColor.m_vColors.clear();
		actionDesc.borderColor.m_fAngle = 0;
	}
	actionDesc.motionKeys = *MOTIONKEYS;


	for(int i = 0; i < emargs.size(); i++)
	{

		CVarList kv(emargs[i], 2, ':');
		if (kv[0] == "action") {
			actionDesc.commandString = kv[1];
		} else if (kv[0] == "textsize") {
			actionDesc.textSize = configStringToInt(kv[1]);
		} else if (kv[0] == "textcolor") {
			actionDesc.textColor = CColor(configStringToInt(kv[1]));
		} else if (kv[0] == "bgcolor") {
			actionDesc.backgroundColor = CColor(configStringToInt(kv[1]));
		} else if (kv[0] == "textfont") {
			actionDesc.textFont = kv[1];
		} else if (kv[0] == "textpadding") {
			CVarList padVars = CVarList(kv[1], 0, 's');
			actionDesc.boxPadding.parseGapData(padVars);
		} else if (kv[0] == "rounding") {
			actionDesc.rounding = configStringToInt(kv[1]);
		} else if (kv[0] == "bordersize") {
			actionDesc.borderSize = configStringToInt(kv[1]);
		} else if (kv[0] == "bordercolor") {
			CVarList varlist(kv[1], 0, 's');
			actionDesc.borderColor.m_vColors.clear();
			actionDesc.borderColor.m_fAngle = 0;
			if(!parseBorderGradient(kv[1], &actionDesc.borderColor)) {
				actionDesc.borderColor.m_vColors.clear();
				actionDesc.borderColor.m_fAngle = 0;
			}
		} else if (kv[0] == "motionkeys") {
				actionDesc.motionKeys = kv[1];
		}
	}

	int key_idx = 0;
	int key_length = actionDesc.motionKeys.length();

	for (auto &w : g_pCompositor->m_vWindows) {
		for (auto &m : g_pCompositor->m_vMonitors) {
			auto pWindow = w.get();
			if (pWindow->m_pWorkspace == m->activeWorkspace &&  key_idx < key_length && !pWindow->isHidden() && pWindow->m_bIsMapped && !pWindow->m_bFadingOut) {
					std::string lstr = actionDesc.motionKeys.substr(key_idx++, 1);
					addLabelToWindow(pWindow, &actionDesc, lstr);
			}
		}
	}

	HyprlandAPI::invokeHyprctlCommand("dispatch", "submap __easymotionsubmap__");
}


void registerDispatchers()
{
	HyprlandAPI::addDispatcher(PHANDLE, "easymotion", easymotionDispatch);
	HyprlandAPI::addDispatcher(PHANDLE, "easymotionaction", easymotionActionDispatch);
	HyprlandAPI::addDispatcher(PHANDLE, "easymotionexit", easymotionExitDispatch);
}