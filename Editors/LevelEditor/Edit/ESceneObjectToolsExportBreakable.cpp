#include "stdafx.h"
#pragma hdrstop

#include "ESceneObjectTools.h"
#include "EditObject.h"
#include "EditMesh.h"
#include "SceneObject.h"
#include "scene.h"
#include "ExportSkeleton.h"
#include "clsid_game.h"
#include "ui_main.h"
#include "GeometryCollector.h"

#include "xrServer_Objects_Abstract.h"
#include "ESceneSpawnTools.h"
#include "GeometryPartExtractor.h"

//----------------------------------------------------
IC bool build_mesh(const Fmatrix& parent, CEditableMesh* mesh, CGeomPartExtractor* extractor, u32 game_mtl_mask)
{
	bool bResult 			= true;
    // fill faces
    for (SurfFacesPairIt sp_it=mesh->GetSurfFaces().begin(); sp_it!=mesh->GetSurfFaces().end(); sp_it++){
		IntVec& face_lst 	= sp_it->second;
        CSurface* surf 		= sp_it->first;
		int gm_id			= surf->_GameMtl(); 
        if (gm_id<0){ 
        	ELog.DlgMsg		(mtError,"Surface: '%s' contains bad game material.",surf->_Name());
        	bResult 		= FALSE; 
            break; 
        }
        SGameMtl* M 		= GMLib.GetMaterialByID(gm_id);
        if (0==M){
        	ELog.DlgMsg		(mtError,"Surface: '%s' contains undefined game material.",surf->_Name());
        	bResult 		= FALSE; 
            break; 
        }
        if (!M->Flags.is(game_mtl_mask)) continue;
        
        FaceVec&	faces 	= mesh->GetFaces();
        FvectorVec&	pn	 	= mesh->GetPNormals();
        FvectorVec&	pts 	= mesh->GetPoints();
	    for (IntIt f_it=face_lst.begin(); f_it!=face_lst.end(); f_it++){
			st_Face& face 	= faces[*f_it];
            Fvector 		v[3],n[3];
            parent.transform_tiny	(v[0],pts[face.pv[0].pindex]);
            parent.transform_tiny	(v[1],pts[face.pv[1].pindex]);
            parent.transform_tiny	(v[2],pts[face.pv[2].pindex]);
            parent.transform_dir	(n[0],pn[*f_it*3+0]);
            parent.transform_dir	(n[1],pn[*f_it*3+1]);
            parent.transform_dir	(n[2],pn[*f_it*3+2]);
            const Fvector2*	uv[3];
            mesh->GetFaceTC			(*f_it,uv);
            extractor->AppendFace	(surf,v,n,uv);
        }
        if (!bResult) break;
    }
    return bResult;
}

bool ESceneObjectTools::ExportBreakableObjects(SExportStreams& F)
{
	bool bResult = true;
    CGeomPartExtractor* extractor=0;

    Fbox 		bb;
    if (!GetBox(bb)) return false;

    extractor	= xr_new<CGeomPartExtractor>();
    extractor->Initialize(bb,EPS_L,2);
    
    UI->SetStatus	("Export breakable objects...");
	// collect verts&&faces
    {
	    SPBItem* pb = UI->PBStart(m_Objects.size(),"Prepare geometry...");
        for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
            UI->PBInc(pb);
            CSceneObject* obj 		= dynamic_cast<CSceneObject*>(*it); VERIFY(obj);
            if (obj->IsStatic()){
                CEditableObject *O 	= obj->GetReference();
                const Fmatrix& T 	= obj->_Transform();
                for(EditMeshIt M=O->FirstMesh();M!=O->LastMesh();M++)
                    if (!build_mesh	(T,*M,extractor,SGameMtl::flBreakable)){bResult=false;break;}
            }
        }
	    UI->PBEnd(pb);
    }
    if (!extractor->Process())		bResult = false;
    // export parts
    if (bResult){
    	SBPartVec& parts			= extractor->GetParts();
	    SPBItem* pb = UI->PBStart(parts.size(),"Export Parts...");
        for (SBPartVecIt p_it=parts.begin(); p_it!=parts.end(); p_it++){	
            UI->PBInc(pb);
            SBPart*	P				= *p_it;
        	if (P->Valid()){
                // export visual
                AnsiString sn		= AnsiString().sprintf("meshes\\brkbl#%d.ogf",(p_it-parts.begin()));
                std::string fn		= Scene->LevelPath()+sn.c_str();
                IWriter* W			= FS.w_open(fn.c_str()); VERIFY(W);
                if (!P->Export(*W)){
                    ELog.DlgMsg		(mtError,"Invalid breakable object.");
                    bResult 		= false;
                    break;
                }
                FS.w_close			(W);
                // export spawn object
                {
                    AnsiString entity_ref		= "breakable_object";
                    ISE_Abstract*	m_Data		= create_entity(entity_ref.c_str()); 	VERIFY(m_Data);
                    CSE_Visual* m_Visual		= m_Data->visual();	VERIFY(m_Visual);
                    // set params
                    strcpy	  					(m_Data->name(),entity_ref.c_str());
                    strcpy	  					(m_Data->name_replace(),sn.c_str());
                    m_Data->position().set		(P->m_RefOffset); 
                    m_Data->angle().set			(P->m_RefRotate);
                    m_Visual->set_visual		(sn.c_str(),false);

                    NET_Packet					Packet;
                    m_Data->Spawn_Write			(Packet,TRUE);

                    F.spawn.stream.open_chunk	(F.spawn.chunk++);
                    F.spawn.stream.w			(Packet.B.data,Packet.B.count);
                    F.spawn.stream.close_chunk	();
                    destroy_entity				(m_Data);
                }
            }else{
            	ELog.Msg(mtError,"Can't export invalid part #%d",p_it-parts.begin());
            }
        }
	    UI->PBEnd(pb);
    }
    // clean up
    xr_delete(extractor);

    return bResult;
}
//----------------------------------------------------

bool ESceneObjectTools::ExportClimableObjects(SExportStreams& F)
{
	bool bResult = true;
    CGeomPartExtractor* extractor=0;

    Fbox 		bb;
    if (!GetBox(bb)) return false;

    extractor	= xr_new<CGeomPartExtractor>();
    extractor->Initialize(bb,EPS_L,int_max);
    
    UI->SetStatus	("Export climable objects...");
	// collect verts&&faces
    {
	    SPBItem* pb = UI->PBStart(m_Objects.size(),"Prepare geometry...");
        for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
            UI->PBInc(pb);
            CSceneObject* obj 		= dynamic_cast<CSceneObject*>(*it); VERIFY(obj);
            if (obj->IsStatic()){
                CEditableObject *O 	= obj->GetReference();
                const Fmatrix& T 	= obj->_Transform();
                for(EditMeshIt M=O->FirstMesh();M!=O->LastMesh();M++)
                    if (!build_mesh	(T,*M,extractor,SGameMtl::flClimable)){bResult=false;break;}
            }
        }
	    UI->PBEnd(pb);
    }
    if (!extractor->Process())		bResult = false;
    // export parts
    if (bResult){
    	SBPartVec& parts			= extractor->GetParts();
	    SPBItem* pb = UI->PBStart(parts.size(),"Export Parts...");
        for (SBPartVecIt p_it=parts.begin(); p_it!=parts.end(); p_it++){	
            UI->PBInc(pb);
            SBPart*	P				= *p_it;
        	if (P->Valid()){
                // export visual
                AnsiString sn		= AnsiString().sprintf("meshes\\clmbl#%d.ogf",(p_it-parts.begin()));
                std::string fn		= Scene->LevelPath()+sn.c_str();
                IWriter* W			= FS.w_open(fn.c_str()); VERIFY(W);
                if (!P->Export(*W)){
                    ELog.DlgMsg		(mtError,"Invalid climable object.");
                    bResult 		= false;
                    break;
                }
                FS.w_close			(W);
                // export spawn object
                {
                    AnsiString entity_ref		= "climable_object";
                    ISE_Abstract*	m_Data		= create_entity(entity_ref.c_str()); 	VERIFY(m_Data);
                    CSE_Visual* m_Visual		= m_Data->visual();	VERIFY(m_Visual);
                    // set params
                    strcpy	  					(m_Data->name(),entity_ref.c_str());
                    strcpy	  					(m_Data->name_replace(),sn.c_str());
                    m_Data->position().set		(P->m_RefOffset); 
                    m_Data->angle().set			(P->m_RefRotate);
                    m_Visual->set_visual		(sn.c_str(),false);

                    NET_Packet					Packet;
                    m_Data->Spawn_Write			(Packet,TRUE);

                    F.spawn.stream.open_chunk	(F.spawn.chunk++);
                    F.spawn.stream.w			(Packet.B.data,Packet.B.count);
                    F.spawn.stream.close_chunk	();
                    destroy_entity				(m_Data);
                }
            }else{
            	ELog.Msg(mtError,"Can't export invalid part #%d",p_it-parts.begin());
            }
        }
	    UI->PBEnd(pb);
    }
    // clean up
    xr_delete(extractor);

    return bResult;
}
//----------------------------------------------------

