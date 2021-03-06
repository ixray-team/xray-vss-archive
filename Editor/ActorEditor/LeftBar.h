//---------------------------------------------------------------------------
#ifndef LeftBarH
#define LeftBarH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "ExtBtn.hpp"
#include <Menus.hpp>
#include "ExtBtn.hpp"
#include "MxMenus.hpp"
#include "mxPlacemnt.hpp"
#include "ElTree.hpp"
#include "ElXPThemedControl.hpp"

#include "UI_Tools.h"
#include "ElTreeAdvEdit.hpp"
#include "ElPgCtl.hpp"
#include "MXCtrls.hpp"

//---------------------------------------------------------------------------
class TfraLeftBar : public TFrame
{
__published:	// IDE-managed Components
    TPanel *paLeftBar;
	TFormStorage *fsStorage;
	TMxPopupMenu *pmMotionsFile;
	TMenuItem *miLoadMotions;
	TMenuItem *miSaveMotions;
	TMxPopupMenu *pmSceneFile;
	TPanel *paCurrentMotion;
	TLabel *Label1;
	TMxPopupMenu *pmShaderList;
	TMenuItem *ExpandAll1;
	TMenuItem *CollapseAll1;
	TMenuItem *N1;
	TMenuItem *CreateFolder1;
	TMenuItem *Rename1;
	TElTreeInplaceAdvancedEdit *InplaceParticleEdit;
	TPanel *paPSList;
	TPanel *Panel1;
	TBevel *Bevel1;
	TMenuItem *Load1;
	TMenuItem *Save2;
	TMenuItem *Export2;
	TMenuItem *SaevAs1;
	TMenuItem *N5;
	TMenuItem *Import1;
	TMenuItem *N2;
	TMenuItem *miRecentFiles;
	TPanel *paModel;
	TLabel *Label4;
	TExtBtn *ExtBtn2;
	TExtBtn *ebMakePreview;
	TExtBtn *ebRenderEditorStyle;
	TExtBtn *ebRenderEngineStyle;
	TLabel *Label5;
	TExtBtn *ebBonePart;
	TPanel *paObjectProperties;
	TLabel *Label6;
	TExtBtn *ExtBtn5;
	TBevel *Bevel6;
	TPanel *paObjectProps;
	TPanel *paMotions;
	TLabel *Label2;
	TPanel *Panel4;
	TBevel *Bevel3;
	TBevel *Bevel4;
	TElTree *tvMotions;
	TPanel *Panel5;
	TExtBtn *ebMotionsRemove;
	TExtBtn *ebMotionsClear;
	TExtBtn *ebMotionsFile;
	TExtBtn *ebMotionsAppend;
	TExtBtn *ExtBtn10;
	TExtBtn *ebCurrentPlay;
	TExtBtn *ebCurrentStop;
	TMxLabel *lbCurFrames;
	TMxLabel *lbCurFPS;
	TMxLabel *RxLabel2;
	TMxLabel *RxLabel1;
	TExtBtn *ebCurrentPause;
	TExtBtn *ebMixMotion;
	TMxLabel *MxLabel1;
	TMxLabel *lbName;
	TPanel *paMotionProps;
	TSplitter *Splitter1;
	TSplitter *Splitter2;
	TPanel *Panel6;
	TMxPopupMenu *pmPreviewObject;
	TMenuItem *Custom1;
	TMenuItem *N3;
	TMenuItem *none1;
	TMenuItem *Preferences1;
	TMenuItem *Clear1;
	TPanel *paScene;
	TLabel *APHeadLabel2;
	TExtBtn *ebSceneMin;
	TExtBtn *ebSceneCommands;
	TExtBtn *ebPreferences;
	TExtBtn *ExtBtn3;
	TExtBtn *ebPreviewObjectClick;
    void __fastcall ebSaveClick(TObject *Sender);
    void __fastcall PanelMimimizeClick(TObject *Sender);
    void __fastcall PanelMaximizeClick(TObject *Sender);
    void __fastcall ebEditorPreferencesClick(TObject *Sender);
    void __fastcall ebRefreshTexturesClick(TObject *Sender);
	void __fastcall ebResetAnimationClick(TObject *Sender);
	void __fastcall ebActorMotionsFileMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall ebSceneCommandsMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall tvMotionsMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall CreateFolder1Click(TObject *Sender);
	void __fastcall ExpandAll1Click(TObject *Sender);
	void __fastcall CollapseAll1Click(TObject *Sender);
	void __fastcall ebMotionsRemoveClick(TObject *Sender);
	void __fastcall tvMotionsItemFocused(TObject *Sender);
	void __fastcall tvMotionsKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall Rename1Click(TObject *Sender);
	void __fastcall InplaceParticleEditValidateResult(TObject *Sender,
          bool &InputValid);
	void __fastcall eMotionsAppendClick(TObject *Sender);
	void __fastcall tvMotionsStartDrag(TObject *Sender,
          TDragObject *&DragObject);
	void __fastcall tvMotionsDragOver(TObject *Sender, TObject *Source, int X,
          int Y, TDragState State, bool &Accept);
	void __fastcall tvMotionsDragDrop(TObject *Sender, TObject *Source, int X,
          int Y);
	void __fastcall ebCurrentPlayClick(TObject *Sender);
	void __fastcall ebCurrentStopClick(TObject *Sender);
	void __fastcall Import1Click(TObject *Sender);
	void __fastcall Load1Click(TObject *Sender);
	void __fastcall Save2Click(TObject *Sender);
	void __fastcall SaevAs1Click(TObject *Sender);
	void __fastcall fsStorageSavePlacement(TObject *Sender);
	void __fastcall fsStorageRestorePlacement(TObject *Sender);
	void __fastcall ebCurrentPauseClick(TObject *Sender);
	void __fastcall miRecentFilesClick(TObject *Sender);
	void __fastcall ebBonePartClick(TObject *Sender);
	void __fastcall LoadClick(TObject *Sender);
	void __fastcall miSaveMotionsClick(TObject *Sender);
	void __fastcall ebMotionsClearClick(TObject *Sender);
	void __fastcall Export2Click(TObject *Sender);
	void __fastcall ebMakePreviewClick(TObject *Sender);
	void __fastcall ebRenderStyleClick(TObject *Sender);
	void __fastcall Custom1Click(TObject *Sender);
	void __fastcall none1Click(TObject *Sender);
	void __fastcall Preferences1Click(TObject *Sender);
	void __fastcall ebPreviewObjectClickMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall Clear1Click(TObject *Sender);
private:	// User declarations
	void __fastcall ShowPPMenu		(TMxPopupMenu* M, TObject* btn);
    TElTreeItem*	DragItem;
public:		// User declarations
        __fastcall TfraLeftBar		(TComponent* Owner);
    void 			UpdateBar		();
	void 			AddMotion		(LPCSTR full_name, bool bLoadMode);
	void 			ClearMotionList	();
    void			UpdateMotionList();
    void			UpdateProperties();
    void			UpdateMotionProperties();
	void 			AppendRecentFile(LPCSTR name);
    void			SetRenderStyle	(bool bEngineStyle);
};
//---------------------------------------------------------------------------
extern PACKAGE TfraLeftBar *fraLeftBar;
//---------------------------------------------------------------------------
#endif
