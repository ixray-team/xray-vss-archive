#include "stdafx.h"
#pragma hdrstop

#include "PropertiesListTypes.h"
#include "xr_tokens.h"

AnsiString prop_draw_text;
//------------------------------------------------------------------------------
// TextValue
//------------------------------------------------------------------------------
LPCSTR TextValue::GetText()
{
    prop_draw_text=values.front();
    if (OnDrawValue)OnDrawValue(this, &prop_draw_text);
    return prop_draw_text.c_str();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// AnsiTextValue
//------------------------------------------------------------------------------
LPCSTR AnsiTextValue::GetText(){
    prop_draw_text=*values.front();
    if (OnDrawValue)OnDrawValue(this, &prop_draw_text);
    return prop_draw_text.c_str();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// BOOLValue
//------------------------------------------------------------------------------
LPCSTR BOOLValue::GetText()
{
	BOOL val = *values.front();
    if (OnDrawValue)OnDrawValue(this, &val);
    return LPCSTR(val);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// WAVEValue
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// COLORValue
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// IntValue
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// VectorValue
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// TokenValue
//------------------------------------------------------------------------------
LPCSTR 	TokenValue::GetText()
{
	int draw_val 	= GetValue();
    if (OnDrawValue)OnDrawValue(this, &draw_val);
	for(int i=0; token[i].name; i++) if (token[i].id==draw_val) return token[i].name;
    return 0;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// TokenValue2
//------------------------------------------------------------------------------
LPCSTR 	TokenValue2::GetText()
{
	DWORD draw_val 	= GetValue();
    if (OnDrawValue)OnDrawValue(this, &draw_val);
    if ((draw_val<0)||(draw_val>items.size())) return "";
	return items[draw_val].c_str();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// TokenValue3
//------------------------------------------------------------------------------
LPCSTR TokenValue3::GetText()
{
	VERIFY(GetValue()<cnt);
	DWORD draw_val 	= GetValue();
    if (OnDrawValue)OnDrawValue(this, &draw_val);
    return items[draw_val].str;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ListValue
//------------------------------------------------------------------------------
LPCSTR	ListValue::GetText()
{
    prop_draw_text=GetValue();
    if (OnDrawValue)OnDrawValue(this, &prop_draw_text);
    return prop_draw_text.c_str();
}
//------------------------------------------------------------------------------


