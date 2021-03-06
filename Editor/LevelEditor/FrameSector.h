//---------------------------------------------------------------------------
#ifndef FrameSectorH
#define FrameSectorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>

#include "CustomObject.h"
#include "ui_customtools.h"
#include "multi_edit.hpp"
#include "ExtBtn.hpp"
#include "MXCtrls.hpp"
//---------------------------------------------------------------------------
class TfraSector : public TFrame
{
__published:	// IDE-managed Components
	TPanel *paSectorActions;
	TExtBtn *ebAddMesh;
	TExtBtn *ebDelMesh;
    TExtBtn *ebFaceBoxPick;
	TLabel *RxLabel1;
	TPanel *paCommands;
	TExtBtn *ebValidate;
	TLabel *APHeadLabel1;
	TExtBtn *ExtBtn2;
	TLabel *APHeadLabel2;
	TExtBtn *ExtBtn1;
	TExtBtn *ebCreateNew;
	TBevel *Bevel1;
	TExtBtn *ebCaptureInside;
	TExtBtn *ebCreateDefault;
	TExtBtn *ebRemoveDefault;
	TBevel *Bevel2;
    void __fastcall PanelMinClick(TObject *Sender);
    void __fastcall TopClick(TObject *Sender);
	void __fastcall ebCaptureInsideVolumeClick(TObject *Sender);
	void __fastcall ebCreateDefaultClick(TObject *Sender);
	void __fastcall ebRemoveDefaultClick(TObject *Sender);
	void __fastcall ebValidateClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TfraSector(TComponent* Owner);
};
//---------------------------------------------------------------------------
#endif
