#pragma once

#include "FixedMap.h"

class ENGINE_API FCached;
class ENGINE_API CVisual;

namespace SceneGraph
{
	// Low level
	struct _MatrixItem	{
		CObject*		pObject;
		CVisual*		pVisual;
		Fmatrix			Matrix;				// matrix (copy)
		Fvector			vCenter;
		BOOL			nearer;
	};
	struct _PatchItem	{
		Shader*			S;
		Fvector			P;
		float			size;
		float			angle;
		BOOL			nearer;
	};

	// Higher level	- NORMAL
	struct mapNormalDirect
	{
		FixedMAP<float,CVisual*>			sorted;
		std::vector<CVisual*>				unsorted;
	};
	struct mapNormalCached
	{
		FixedMAP<float,FCached*>			sorted;
		std::vector<FCached*>				unsorted;
	};
	struct mapNormalItems 
	{
		float								ssa;
		mapNormalDirect						direct;
		mapNormalCached						cached;
	};
	struct mapNormalConstants	: public	FixedMAP<SConstantList*,mapNormalItems>
	{
		float								ssa;
	};
	struct mapNormalMatrices	: public	FixedMAP<SMatrixList*,mapNormalConstants>
	{
		float								ssa;
	};
	struct mapNormalTextures	: public	FixedMAP<STextureList*,mapNormalMatrices>
	{
		float								ssa;
	};
	typedef FixedMAP<DWORD,mapNormalTextures>		mapNormalCodes;

	// Higher level - MATRIX
	typedef FixedMAP<float,_MatrixItem>		mapMatrixItem;

	// Top level
	typedef mapNormalCodes					mapNormal_T		[8];

	typedef FixedMAP<Shader*,mapMatrixItem>	mapMatrix_T;
	typedef mapMatrix_T::TNode				mapMatrix_Node;

	typedef FixedMAP<float,_MatrixItem>		mapSorted_T;
	typedef mapSorted_T::TNode				mapSorted_Node;

	typedef std::vector<_PatchItem>				vecPatches_T;
};
