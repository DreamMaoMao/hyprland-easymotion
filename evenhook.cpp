#include "evenhook.hpp"

typedef void (*origCWindow_onUnmap)(void*);

std::string getKeynameFromKeycode(wlr_keyboard_key_event* e, SKeyboard* pKeyboard) {
  struct wlr_keyboard *keyboard =  (struct wlr_keyboard *)pKeyboard->keyboard;
  xkb_keycode_t keycode = e->keycode + 8;
  xkb_keysym_t keysym = xkb_state_key_get_one_sym(keyboard->xkb_state, keycode);
  char *tmp_keyname = new char[64];
  xkb_keysym_get_name(keysym, tmp_keyname, 64);
  std::string keyname = tmp_keyname;
  delete[] tmp_keyname;
  return keyname;
}


static void hkCWindow_onUnmap(void* thisptr) {
  // call the original function,Let it do what it should do
  (*(origCWindow_onUnmap)g_hyeasymotion_pCWindow_onUnmap->m_pOriginal)(thisptr);

    unsigned int CLIENTNUM = 0; 
    for (auto &w : g_pCompositor->m_vWindows){
    	CWindow *pWindow = w.get();
    	if (pWindow->m_iWorkspaceID == g_pCompositor->m_pLastMonitor->activeWorkspace && !pWindow->isHidden() && pWindow->m_bIsMapped && !pWindow->m_bFadingOut) {
    		CLIENTNUM++;
    	}
    }    

	//auto exit,if no client in active workspace
    if (CLIENTNUM == 0) {
		easymotionExitDispatch("");
    }

}

bool oneasymotionKeypress(void *self, std::any data) {

	if (g_pGlobalState->motionLabels.empty()) return false;

	const auto MODS = g_pInputManager->accumulateModsFromAllKBs();
	if(MODS != 0)
		return false;

	auto map = std::any_cast<std::unordered_map<std::string, std::any>>(data);
	std::any kany = map["keyboard"];
	wlr_keyboard_key_event *ev = std::any_cast<wlr_keyboard_key_event *>(map["event"]);
	SKeyboard *keyboard = std::any_cast<SKeyboard *>(kany);
	std::string keyname = getKeynameFromKeycode(ev,keyboard);
	const auto KEYCODE = ev->keycode + 8;

	const xkb_keysym_t KEYSYM = xkb_state_key_get_one_sym(keyboard->xkbTranslationState, KEYCODE);

	if (ev->state != WL_KEYBOARD_KEY_STATE_PRESSED) return false;

	xkb_keysym_t actionKeysym = 0;
	for (auto &ml : g_pGlobalState->motionLabels) {
		if (ml->m_szLabel != "") {
			actionKeysym = xkb_keysym_from_name(ml->m_szLabel.c_str(), XKB_KEYSYM_NO_FLAGS);
			if (actionKeysym && (actionKeysym == KEYSYM)) {
				hypreasymotion_log(LOG,"hit key:{},owner:{}",keyname,ml->getOwner());
				easymotionActionDispatch(ml->m_szLabel);
				return true;
			}
		}
	}
	return false;
}

void registerEventHook()
{
    g_hyeasymotion_pCWindow_onUnmap = HyprlandAPI::createFunctionHook(PHANDLE, (void*)&CWindow::onUnmap, (void*)&hkCWindow_onUnmap);
    g_hyeasymotion_pCWindow_onUnmap->hook();
    HyprlandAPI::registerCallbackDynamic(PHANDLE, "keyPress", [&](void *self, SCallbackInfo &info, std::any data) {
			info.cancelled = oneasymotionKeypress(self, data);
	});
}