//---------------------------------------------------------------------------
#ifndef EScenePortalToolsH
#define EScenePortalToolsH

#include "ESceneCustomOTools.h"

class EScenePortalTools: public ESceneCustomOTools
{
	typedef ESceneCustomOTools inherited;
    friend class 		CPortal;
protected:
	enum{
    	flDrawSimpleModel	= (1<<31),
    };
    Flags32				m_Flags;
    // controls
    virtual void 		CreateControls			();
	virtual void 		RemoveControls			();
public:
						EScenePortalTools		():ESceneCustomOTools(OBJCLASS_PORTAL){m_Flags.zero();}
	// definition
    IC LPCSTR			ClassName				(){return "portal";}
    IC LPCSTR			ClassDesc				(){return "Portal";}
    IC int				RenderPriority			(){return 20;}

	virtual void 		FillProp				(LPCSTR pref, PropItemVec& items);

    // IO
    virtual bool   		Load            		(IReader&);
    virtual void   		Save            		(IWriter&);
    virtual bool		LoadSelection      		(IReader&);
    virtual void		SaveSelection      		(IWriter&);
};
//---------------------------------------------------------------------------
#endif
