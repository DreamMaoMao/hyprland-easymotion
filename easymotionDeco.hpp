#pragma once

#include <hyprland/src/config/ConfigDataValues.hpp>
#include <string>

#include <hyprland/src/render/decorations/IHyprWindowDecoration.hpp>
#include <hyprland/src/render/OpenGL.hpp>
#include "globals.hpp"

struct SMotionActionDesc {
  int textSize = 15;
	CColor textColor = CColor(0,0,0,1);
	CColor backgroundColor = CColor(1,1,1,1);
	std::string textFont = "Sans";
	std::string commandString = "";
	CCssGapData boxPadding = CCssGapData();	
	int borderSize = 0;
	CGradientValueData borderColor = CGradientValueData();
	int rounding = 0;
	std::string motionKeys = "abcdefghijklmnopqrstuvwxyz1234567890";
};


class CHyprEasyLabel : public IHyprWindowDecoration {
  public:
    CHyprEasyLabel(CWindow*, SMotionActionDesc *actionDesc);
    virtual ~CHyprEasyLabel();

    virtual SDecorationPositioningInfo getPositioningInfo();

    virtual void                       onPositioningReply(const SDecorationPositioningReply& reply);

    virtual void                       draw(CMonitor*, float a, const Vector2D& offset = Vector2D());

    virtual eDecorationType            getDecorationType();

    virtual void                       updateWindow(CWindow*);

    virtual void                       damageEntire();

    virtual eDecorationLayer           getDecorationLayer();

    virtual uint64_t                   getDecorationFlags();

    bool                               m_bButtonsDirty = true;

    virtual std::string                getDisplayName();

    CWindow*                           getOwner();

		std::string												 m_szLabel;
		std::string  											 m_szActionCmd;
		std::string												 m_szTextFont;
		int																 m_iTextSize;
		int																 m_iPaddingTop;
		int																 m_iPaddingBottom;
		int																 m_iPaddingLeft;
		int																 m_iPaddingRight;
		int 															 m_iRounding;

		CColor		  											 m_cTextColor;
		CColor														 m_cBackgroundColor;
		int																 m_iBorderSize;
		CGradientValueData								 m_cBorderGradient;


	


  private:
		int			layoutWidth;
		int     layoutHeight;
    SWindowDecorationExtents m_seExtents;

    CWindow*                 m_pWindow = nullptr;

    CTexture                 m_tTextTex;

    bool                     m_bWindowSizeChanged = false;

    void                     renderText(CTexture& out, const std::string& text, const CColor& color, const Vector2D& bufferSize, const float scale, const int fontSize);
    CBox                     assignedBoxGlobal();
		void 										 renderMotionString(Vector2D& bufferSize, const float scale);

    // for dynamic updates
    int m_iLastHeight = 0;
};
