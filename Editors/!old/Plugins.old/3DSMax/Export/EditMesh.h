//----------------------------------------------------
// file: StaticMesh.h
//----------------------------------------------------
#ifndef EditMeshH
#define EditMeshH

//----------------------------------------------------
// refs
class 	CSurface;
struct 	SRayPickInfo;
//struct CFrustum;
struct 	FSChunkDef;
class 	CExporter;
class	CCustomObject;

#ifdef _EDITOR
#	include "pick_defs.h"
#endif

#pragma pack( push,1 )
const u8	vmtUV		= 0;
const u8	vmtWeight	= 1;

struct ECORE_API st_WB{                       
	int 	bone;
	float 	weight;
			st_WB	():bone(-1),weight(0){;}
			st_WB	(int b, float w):bone(b),weight(w){;}
	void	set		(int b, float w){bone=b;weight=w;}
};
DEFINE_VECTOR(st_WB,WBVec,WBIt);
struct ECORE_API st_VertexWB:public WBVec{
protected:
	static bool compare_by_weight(const st_WB& a, const st_WB& b)
	{
		return a.weight > b.weight; // ������������� �� ��������
	}
	static bool compare_by_bone(const st_WB& a, const st_WB& b)
	{
		return a.bone < b.bone; // ������������� �� �����������
	}
public:
	void sort_by_bone()
	{
		std::sort(begin(),end(),compare_by_bone);
	}
	void sort_by_weight()
	{
		std::sort(begin(),end(),compare_by_weight);
	}
	void normalize_weights(int max_influence)
	{
		if ((int)size()>max_influence){	
			WBIt it;
			sort_by_weight	();
			erase			(begin()+2,end()); // delete >2 weight
			float sum_weight=0;
			for (it=begin(); it!=end(); it++) sum_weight+=it->weight;
			for (it=begin(); it!=end(); it++) it->weight/=sum_weight;
			sort_by_bone	();
		}
	}
};
DEFINE_VECTOR(st_VertexWB,VWBVec,VWBIt);

struct ECORE_API st_VMapPt{
	int				vmap_index;	// ������ �� ����
	int				index;		// ������ � V-���� �� uv/w
	st_VMapPt(){vmap_index=-1;index=-1;}
};
// uv's
class ECORE_API st_VMap{
    FloatVec    	vm;			// u,v - ���������� ��� weight
public:
	shared_str		name;		// vertex uv map name
    struct{
        u8			type	:2;
        u8			polymap	:1;
        u8			dim		:2;
        u8			reserved:3;
    };
	IntVec			vindices;
	IntVec			pindices;
public:
	st_VMap			(LPCSTR nm=0, u8 t=vmtUV, bool pm=false)
    {
		type		= t;
		polymap		= pm;
		name		= nm;
		if (t==vmtUV) dim=2; else dim=1;
	}
    IC Fvector2&	getUV		(int idx)				{VERIFY(type==vmtUV);		return (Fvector2&)vm[idx*dim];}
    IC float&		getW		(int idx)				{VERIFY(type==vmtWeight);	return vm[idx];}
    IC FloatVec&	getVM		()						{return vm;}
	IC float*		getVMdata	()						{return &*vm.begin();}
    IC float*		getVMdata	(int start)				{return &*(vm.begin()+start*dim);}
    IC int			VMdatasize	()						{return vm.size()*sizeof(float);}
    IC int*			getVIdata	()						{return &*vindices.begin();}
    IC int			VIdatasize	()						{return vindices.size()*sizeof(int);}
    IC int*			getPIdata	()						{return &*pindices.begin();}
    IC int			PIdatasize	()						{return pindices.size()*sizeof(int);}
    IC int			size		()						{return vm.size()/dim;}
    IC void			resize		(int cnt)				{vm.resize(cnt*dim);vindices.resize(cnt);if (polymap) pindices.resize(cnt); }
	IC void			appendUV	(float u, float v)		{vm.push_back(u);vm.push_back(v);}
	IC void			appendUV	(Fvector2& uv)			{appendUV(uv.x,uv.y);}
	IC void			appendW		(float w)				{vm.push_back(w);}
	IC void			appendVI	(int vi)				{vindices.push_back(vi);}
	IC void			appendPI	(int pi)				{VERIFY(polymap); pindices.push_back(pi);}
    IC void			copyfrom	(float* src, int cnt)	{resize(cnt); CopyMemory(&*vm.begin(),src,cnt*dim*4);}
};

struct st_VMapPtLst{
	u8				count;
	st_VMapPt*		pts;
};
DEFINE_VECTOR		(st_VMapPtLst,VMRefsVec,VMRefsIt);

struct ECORE_API st_SVert{
	Fvector			offs;
    Fvector			norm;
    u16				bone0;
    u16				bone1;
    float			w;
    Fvector2		uv;
};
// faces
struct ECORE_API st_FaceVert{
	int 			pindex;		// point index in PointList
    int				vmref;		// vm-ref index 
};
struct ECORE_API st_Face{
    st_FaceVert		pv[3];		// face vertices (P->P...)
};

// mesh options
struct ECORE_API st_MeshOptions{
	int 			m_Reserved0;
	int 			m_Reserved1;
    st_MeshOptions	(){m_Reserved0=0;m_Reserved1=0;}
};
#pragma pack( pop )

DEFINE_VECTOR		(IntVec,AdjVec,AdjIt);
DEFINE_VECTOR		(st_VMap*,VMapVec,VMapIt);
DEFINE_MAP			(CSurface*,IntVec,SurfFaces,SurfFacesPairIt);

//refs
struct st_RenderBuffer;
class CSurface;
class CSector;

#ifdef _EDITOR
	struct ECORE_API st_RenderBuffer{
		u32			dwStartVertex;
	    u32			dwNumVertex;
        ref_geom 	pGeom;
		st_RenderBuffer	(u32 sv, u32 nv):dwStartVertex(sv),dwNumVertex(nv),pGeom(0){;}
	};
	DEFINE_VECTOR(st_RenderBuffer,RBVector,RBVecIt);
	DEFINE_MAP(CSurface*,RBVector,RBMap,RBMapPairIt);
#endif

class ECORE_API CEditableMesh {
	friend class MeshExpUtility;
	friend class CEditableObject;
    friend class CSectorItem;
    friend class CSector;
    friend class CPortalUtils;
    friend class SceneBuilder;
    friend class CExportSkeleton;
    friend class CExportObjectOGF;
    friend class TfrmEditLibrary;
	friend class CExporter;
	friend class CXRayObjectExport;
	friend class CXRaySkinExport;

	shared_str			m_Name;

    CEditableObject*	m_Parent;

    void            GenerateCFModel		();
	void 			GenerateRenderBuffers();
    void			UnloadCForm     	();
#ifdef _EDITOR
	void 			UnloadRenderBuffers	();
#endif
public:
    void 			GenerateFNormals	();
    void 			GenerateVNormals	();
    void            GenerateSVertices	();
	void 			GenerateAdjacency	();
    void			UnloadFNormals   	(bool force=false);
    void			UnloadVNormals   	(bool force=false);
    void			UnloadSVertices  	(bool force=false);
    void			UnloadAdjacency  	(bool force=false);
private:
    // internal variables
	enum{
		flVisible	= (1<<0),
		flLocked	= (1<<1),
		flSGMask	= (1<<2),
	};
	Flags8			m_Flags;
public:
	st_MeshOptions	m_Ops;
protected:
	Fbox			m_Box;

	int				m_FNormalsRefs;
	int				m_VNormalsRefs;
	int				m_AdjsRefs;
	int				m_SVertRefs;

    u32				m_VertCount;
    u32				m_FaceCount;
    
    Fvector*	    m_Verts;	// |
    AdjVec*			m_Adjs;    	// + some array size!!!
	u32*			m_SGs;		// |
    Fvector*		m_FNormals;	// |
    Fvector*		m_VNormals;	// | *3
    st_SVert*		m_SVertices;// | *3
    st_Face*	    m_Faces;    // + some array size!!!
    SurfFaces	    m_SurfFaces;
    VMapVec		    m_VMaps;
    VMRefsVec	    m_VMRefs;

#ifdef _EDITOR
    CDB::MODEL*		m_CFModel;
	RBMap*			m_RenderBuffers;
#endif

	void 			FillRenderBuffer		(IntVec& face_lst, int start_face, int num_face, const CSurface* surf, LPBYTE& data);

	void 			RecurseTri				(int id);


	// mesh optimize routine
	bool 			OptimizeFace			(st_Face& face);
public:
	void			RecomputeBBox			();
	void 			Optimize				(BOOL NoOpt);
public:
	                CEditableMesh			(CEditableObject* parent){m_Parent=parent;Construct();}
	                CEditableMesh			(CEditableMesh* source,CEditableObject* parent){m_Parent=parent;Construct();CloneFrom(source);}
	virtual         ~CEditableMesh			();
	void			Construct				();
    void			Clear					();

	IC void			SetName					(LPCSTR name){m_Name=name;}
	IC const char*	GetName					(){return m_Name.c_str();}
	void            GetBox					(Fbox& box){box.set(m_Box);}
	CSurface*		GetSurfaceByFaceID		(u32 fid);
	void			GetFaceTC				(u32 fid, const Fvector2* tc[3]);
	void			GetFacePT				(u32 fid, const Fvector* pt[3]);
	IC BOOL 		Visible					(){return m_Flags.is(flVisible); }
	IC void 		Show					(BOOL bVisible){m_Flags.set(flVisible,bVisible);}

    // mesh modify routine
	void            CloneFrom				(CEditableMesh *source);
	void            Transform				(const Fmatrix& parent);

	IC CEditableObject*	Parent				(){ return m_Parent;	}
    IC u32			GetFCount				(){ return m_FaceCount;	}
    IC st_Face*		GetFaces				(){ return m_Faces;		}
	IC u32*			GetSGs					(){ return m_SGs;		}
    IC Fvector*		GetVerts				(){ return m_Verts;		}
    IC u32			GetVCount				(){ return m_VertCount;	}
	IC VMapVec&		GetVMaps				(){ return m_VMaps;		}
	IC VMRefsVec&	GetVMRefs				(){ return m_VMRefs;	}
	IC SurfFaces&	GetSurfFaces			(){ return m_SurfFaces;	}
    IC Fvector*		GetFNormals				(){ VERIFY(0!=m_FNormals); return m_FNormals;	}
    IC Fvector*		GetVNormals				(){ VERIFY(0!=m_VNormals); return m_VNormals;	}
    IC st_SVert*	GetSVertices			(){ VERIFY(0!=m_SVertices);return m_SVertices;	}
	    
    // pick routine
	bool            RayPick					(float& dist, const Fvector& start, const Fvector& dir, const Fmatrix& inv_parent, SRayPickInfo* pinf = NULL);
#ifdef _EDITOR
	void            RayQuery				(SPickQuery& pinf);
	void            RayQuery				(const Fmatrix& parent, const Fmatrix& inv_parent, SPickQuery& pinf);
	void            BoxQuery				(const Fmatrix& parent, const Fmatrix& inv_parent, SPickQuery& pinf);
    bool 			BoxPick					(const Fbox& box, const Fmatrix& inv_parent, SBoxPickInfoVec& pinf);
	bool            FrustumPick				(const CFrustum& frustum, const Fmatrix& parent);
    void            FrustumPickFaces		(const CFrustum& frustum, const Fmatrix& parent, U32Vec& fl);
    bool			CHullPickMesh			(PlaneVec& pl, const Fmatrix& parent);
	void 			GetTiesFaces			(int start_id, U32Vec& fl, float fSoftAngle, bool bRecursive);
#endif

    // render routine
	void 			Render					(const Fmatrix& parent, CSurface* S);
	void 			RenderSkeleton			(const Fmatrix& parent, CSurface* S);
	void            RenderList				(const Fmatrix& parent, u32 color, bool bEdge, IntVec& fl);
	void 			RenderSelection			(const Fmatrix& parent, CSurface* s, u32 color);
	void 			RenderEdge				(const Fmatrix& parent, CSurface* s, u32 color);

    // statistics methods
    int 			GetFaceCount			(bool bMatch2Sided=true);
	int 			GetVertexCount			(){return m_VertCount;}
    int 			GetSurfFaceCount		(CSurface* surf, bool bMatch2Sided=true);
    float			CalculateSurfaceArea	(CSurface* surf, bool bMatch2Sided);
    float			CalculateSurfacePixelArea(CSurface* surf, bool bMatch2Sided);

    // IO - routine
    void			SaveMesh				(IWriter&);
	bool 			LoadMesh				(IReader&);

    // debug
    void			DumpAdjacency			();

	// convert
#ifdef _MAX_EXPORT
    void			FlipFaces				();
	TriObject*		ExtractTriObject		(INode *node, int &deleteIt);
	bool			Convert					(INode *node);
	bool			Convert					(CExporter* exporter);
#endif

	int				FindSimilarUV			(st_VMap* vmap, Fvector2& _uv);
	int				FindSimilarWeight		(st_VMap* vmap, float _w);
	int				FindVMapByName			(VMapVec& vmaps, const char* name, u8 t, bool polymap);
	void			RebuildVMaps			();

    bool			Validate				();
};
//----------------------------------------------------
#endif /*_INCDEF_EditableMesh_H_*/

