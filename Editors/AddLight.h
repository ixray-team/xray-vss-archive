//---------------------------------------------------------------------------


#ifndef AddLightH
#define AddLightH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "CloseBtn.hpp"
//---------------------------------------------------------------------------
class TfraAddLight : public TFrame
{
__published:	// IDE-managed Components
        TPanel *Panel4;
        TLabel *Label2;
    TExtBtn *SpeedButton19;
        TLabel *Label5;
    TExtBtn *sbSnap;
    void __fastcall PanelMinClick(TObject *Sender);
    void __fastcall Label2Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TfraAddLight(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfraAddLight *fraAddLight;
//---------------------------------------------------------------------------
#endif
