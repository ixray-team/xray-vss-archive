//---------------------------------------------------------------------------

#ifndef float_paramH
#define float_paramH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "RXSpin.hpp"
#include <Mask.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <ImgList.hpp>
//---------------------------------------------------------------------------
class TForm9 : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
    TRxSpinEdit *WorkTime;
    TRxSpinEdit *Value;
    TSpeedButton *AddButton;
    TSpeedButton *DeleteButton;
    TImageList *ImageList;
private:	// User declarations
public:		// User declarations
    float   t, c, b;
    __fastcall TForm9(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm9 *Form9;
//---------------------------------------------------------------------------
#endif
