#include "stdafx.h"
#include "CustomHUD.h"

Flags32 psHUD_Flags = {HUD_CROSSHAIR_RT|HUD_WEAPON_RT|HUD_CROSSHAIR_DYNAMIC|HUD_CROSSHAIR_RT2|HUD_DRAW_RT};

ENGINE_API CCustomHUD* g_hud = NULL;

CCustomHUD::CCustomHUD()
{
	g_hud = this;
}

CCustomHUD::~CCustomHUD()
{
	g_hud = NULL;
}

