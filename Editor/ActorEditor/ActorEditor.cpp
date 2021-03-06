#include "stdafx.h"
#pragma hdrstop
USEFORM("BottomBar.cpp", fraBottomBar);
USEFORM("EditorPref.cpp", frmEditorPreferences);
USEFORM("Splash.cpp", frmSplash);
USEFORM("main.cpp", frmMain);
USEFORM("LeftBar.cpp", fraLeftBar); /* TFrame: File Type */
USEFORM("LogForm.cpp", frmLog);
USEFORM("Editor\ChoseForm.cpp", frmChoseItem);
USEFORM("Editor\ShaderFunction.cpp", frmShaderFunction);
USEFORM("Editor\FrameEmitter.cpp", fraEmitter); /* TFrame: File Type */
USEFORM("Editor\TopBar.cpp", fraTopBar); /* TFrame: File Type */
USEFORM("Editor\PropertiesList.cpp", frmProperties);
USEFORM("BonePart.cpp", frmBonePart);
USEFORM("KeyBar.cpp", frmKeyBar);
USEFORM("Editor\NumericVector.cpp", frmNumericVector);
//---------------------------------------------------------------------------
#include "main.h"
#include "splash.h"
#include "UI_Main.h"
#include "LogForm.h"
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
    try{
        TfrmLog::CreateLog();

        frmSplash = new TfrmSplash(0);
        frmSplash->Show();
        frmSplash->Repaint();

        frmSplash->SetStatus("Initializing");

        Application->Initialize();

        frmSplash->SetStatus("Loading...");

// startup create
		Application->Title = "Actor Editor";
		Application->CreateForm(__classid(TfrmMain), &frmMain);
		Application->CreateForm(__classid(TfrmEditorPreferences), &frmEditorPreferences);
		Application->CreateForm(__classid(TfrmBonePart), &frmBonePart);
		frmMain->SetHInst(hInst);

        _DELETE(frmSplash);

        Application->Run();

        TfrmLog::DestroyLog();
    }
    catch (Exception &exception)
    {
           Application->ShowException(&exception);
    }
    return 0;
}
//---------------------------------------------------------------------------




