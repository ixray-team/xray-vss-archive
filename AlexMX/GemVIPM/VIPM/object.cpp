/* Copyright (C) Tom Forsyth, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Tom Forsyth, 2001"
 */

#include "object.h"

int		g_iNumOfObjectVertsDrawn	= 0;
int		g_iMaxNumTrisDrawn			= -1;
BOOL	g_bOptimiseVertexOrder		= FALSE;
BOOL	g_bShowVIPMInfo				= FALSE;
BOOL	g_bUseFastButBadOptimise	= TRUE;

// Call this to reorder the tris in this trilist to get good vertex-cache coherency.
// *pwList is modified (but obviously not changed in size or memory location).
void OptimiseVertexCoherencyTriList ( WORD *pwList, int iHowManyTris );

// Finds the weighted average number of verts read when rendering this list.
float GetNumVerticesLoadedTriList ( WORD *pwList, int iHowManyTris );

Object::Object()
{
	pNextCollapse = &CollapseRoot;
	iCurSlidingWindowLevel = 0;

	pOptMesh = OptimisedMesh::Create ( this );
}


Object::~Object()
{
	BinCurrentObject();


	if ( pOptMesh != NULL )
	{
		delete pOptMesh;
		pOptMesh = NULL;
	}

	while ( CollapseRoot.ListNext() != NULL )
	{
		delete ( CollapseRoot.ListNext() );
	}

	// Bin any mesh data.
	while ( PermTriRoot.ListNext() != NULL )
	{
		delete ( PermTriRoot.ListNext() );
	}
	while ( PermEdgeRoot.ListNext() != NULL )
	{
		delete ( PermEdgeRoot.ListNext() );
	}
	while ( PermPtRoot.ListNext() != NULL )
	{
		delete ( PermPtRoot.ListNext() );
	}

}

// Call before D3D leaves.
void Object::AboutToChangeDevice ( void )
{
	// Not actually much to do.
	if ( pOptMesh != NULL )
	{
		pOptMesh->AboutToChangeDevice();
	}

	MarkAsDirty();
}



void Object::CreateTestObject ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	MarkAsDirty();

	ASSERT ( PermPtRoot.ListNext() == NULL );
	ASSERT ( PermTriRoot.ListNext() == NULL );
	ASSERT ( PermEdgeRoot.ListNext() == NULL );

#if 0
	// Make a cube.
	MeshPt *ppt000 = new MeshPt ( &PermPtRoot );
	MeshPt *ppt001 = new MeshPt ( &PermPtRoot );
	MeshPt *ppt010 = new MeshPt ( &PermPtRoot );
	MeshPt *ppt011 = new MeshPt ( &PermPtRoot );
	MeshPt *ppt100 = new MeshPt ( &PermPtRoot );
	MeshPt *ppt101 = new MeshPt ( &PermPtRoot );
	MeshPt *ppt110 = new MeshPt ( &PermPtRoot );
	MeshPt *ppt111 = new MeshPt ( &PermPtRoot );

	ppt000->mypt.vPos = D3DXVECTOR3 ( -1.0f,  1.0f, -1.0f );
	ppt001->mypt.vPos = D3DXVECTOR3 (  1.0f,  1.0f, -1.0f );
	ppt010->mypt.vPos = D3DXVECTOR3 ( -1.0f,  1.0f,  1.0f );
	ppt011->mypt.vPos = D3DXVECTOR3 (  1.0f,  1.0f,  1.0f );
	ppt100->mypt.vPos = D3DXVECTOR3 ( -1.0f, -1.0f, -1.0f );
	ppt101->mypt.vPos = D3DXVECTOR3 (  1.0f, -1.0f, -1.0f );
	ppt110->mypt.vPos = D3DXVECTOR3 ( -1.0f, -1.0f,  1.0f );
	ppt111->mypt.vPos = D3DXVECTOR3 (  1.0f, -1.0f,  1.0f );

	// Dodgy normals.
	MeshPt *pt = PermPtRoot.ListNext();
	DWORD dwIndex = 0;
	while ( pt != NULL )
	{
		//pt->mypt.dwIndex = dwIndex++;
		D3DXVec3Normalize ( &(pt->mypt.vNorm), &(pt->mypt.vPos) );

		pt = pt->ListNext();
	}


	MeshTri *ptri;
	// Top.
	ptri = new MeshTri ( ppt000, ppt010, ppt011, &PermTriRoot, &PermEdgeRoot );
	ptri = new MeshTri ( ppt000, ppt011, ppt001, &PermTriRoot, &PermEdgeRoot );
	// Bottom.
	ptri = new MeshTri ( ppt100, ppt111, ppt110, &PermTriRoot, &PermEdgeRoot );
	ptri = new MeshTri ( ppt100, ppt101, ppt111, &PermTriRoot, &PermEdgeRoot );
	// Left.
	ptri = new MeshTri ( ppt000, ppt110, ppt010, &PermTriRoot, &PermEdgeRoot );
	ptri = new MeshTri ( ppt000, ppt100, ppt110, &PermTriRoot, &PermEdgeRoot );
	// Right.
	ptri = new MeshTri ( ppt001, ppt011, ppt111, &PermTriRoot, &PermEdgeRoot );
	ptri = new MeshTri ( ppt001, ppt111, ppt101, &PermTriRoot, &PermEdgeRoot );
	// Front
	ptri = new MeshTri ( ppt000, ppt001, ppt101, &PermTriRoot, &PermEdgeRoot );
	ptri = new MeshTri ( ppt000, ppt101, ppt100, &PermTriRoot, &PermEdgeRoot );
	// Back
	ptri = new MeshTri ( ppt010, ppt111, ppt011, &PermTriRoot, &PermEdgeRoot );
	ptri = new MeshTri ( ppt010, ppt110, ppt111, &PermTriRoot, &PermEdgeRoot );
#else

	HRESULT hres;

	// Make a teapotahedron.
	LPD3DXMESH pmeshTeapot;
	ASSERT ( pd3dDevice != NULL );		// Slight fudge - shame we need a D3D device.
	hres = D3DXCreateTeapot ( pd3dDevice, &pmeshTeapot, NULL );
	// These are just some simpler test meshes
	//hres = D3DXCreatePolygon ( pd3dDevice, 1.0f, 6, &pmeshTeapot, NULL );
	//hres = D3DXCreateSphere ( pd3dDevice, 1.0f, 12, 6, &pmeshTeapot, NULL );
	//hres = D3DXCreateSphere ( pd3dDevice, 1.0f, 30, 15, &pmeshTeapot, NULL );


	// OK, now extract the data.
	int iNumVerts = pmeshTeapot->GetNumVertices();
	int iNumFaces = pmeshTeapot->GetNumFaces();

	LPDIRECT3DVERTEXBUFFER8 pVertexBuffer;
	hres = pmeshTeapot->GetVertexBuffer ( &pVertexBuffer );
	D3DVERTEXBUFFER_DESC vbdesc;
	hres = pVertexBuffer->GetDesc ( &vbdesc );
	// Create my "smart" pointer.
	MyFVFPointer pFVF ( vbdesc.FVF );
	BYTE *pbData;
	hres = pVertexBuffer->Lock ( 0, pFVF.GetFVFSize() * iNumVerts, &pbData, D3DLOCK_READONLY );
	pFVF.SetCurVertex ( pbData );

	// The de-index list.
	MeshPt **ppPts = new MeshPt*[iNumVerts];

	for ( int i = 0; i < iNumVerts; i++ )
	{
		ppPts[i] = new MeshPt ( &PermPtRoot );
		ppPts[i]->mypt.vPos		= pFVF.Position();
		ppPts[i]->mypt.vNorm	= pFVF.Normal();
		//ppPts[i]->mypt.fU		= pFVF.U0();
		//ppPts[i]->mypt.fV		= pFVF.V0();

		//ppPts[i]->mypt.dwIndex = i;

		++pFVF;
	}

	hres = pVertexBuffer->Unlock();
	pVertexBuffer->Release();


	// And now the index buffer.
	LPDIRECT3DINDEXBUFFER8 pIndexBuffer;
	hres = pmeshTeapot->GetIndexBuffer ( &pIndexBuffer );
	D3DINDEXBUFFER_DESC ibdesc;
	hres = pIndexBuffer->GetDesc ( &ibdesc );
	// Unlikely to get any 32bpp indices, but check, just in case.
	// If you do - well, I leave that as an exercise for the reader :-)

	// Oh - just found this comment in the docs:
	//
	// D3DXMESH_32BIT 
	//   The mesh has 32-bit indices instead of 16-bit indices.
	//   A 32-bit mesh can support up to 2^32-1 faces and vertices.
	//   This flag is not supported and should not be used. 
	//
	// So, that answers that question!
	ASSERT ( ibdesc.Format == D3DFMT_INDEX16 );

	// Also, assume that this defines a trilist. Not sure if the mesh tells us
	// what the primitive type is anywhere.
	WORD *pIndex;
	ASSERT ( sizeof (*pIndex) * iNumFaces * 3 == ibdesc.Size );
	hres = pIndexBuffer->Lock ( 0, ibdesc.Size, (BYTE**)&pIndex, D3DLOCK_READONLY );

	for ( int j = 0; j < iNumFaces; j++ )
	{
		MeshPt *ppt[3];
		for ( int i = 0; i < 3; i++ )
		{
			ASSERT ( *pIndex < iNumVerts );
			ppt[i] = ppPts[*pIndex];
			pIndex++;
		}

		MeshTri *ptri = new MeshTri ( ppt[0], ppt[1], ppt[2], &PermTriRoot, &PermEdgeRoot );
	}

	hres = pIndexBuffer->Unlock();
	pIndexBuffer->Release();


	delete[] ppPts;


	// And finally bin the thing.
	pmeshTeapot->Release();
#endif

	iFullNumTris = 0;
	MeshTri *tri = PermTriRoot.ListNext();
	while ( tri != NULL )
	{
		// All the pts had better be the same material.
		tri = tri->ListNext();
		iFullNumTris++;
	}

	MeshEdge *edge = PermEdgeRoot.ListNext();
	while ( edge != NULL )
	{
		edge = edge->ListNext();
	}

	iCurSlidingWindowLevel = 0;
	SetNewLevel ( iCurSlidingWindowLevel );
}

// Check that this is sensible.
void Object::CheckObject ( void )
{
	MeshEdge *edge = PermEdgeRoot.ListNext();
	while ( edge != NULL )
	{
		// All the pts had better be the same material.
		edge = edge->ListNext();
	}

	MeshTri *tri = PermTriRoot.ListNext();
	while ( tri != NULL )
	{
		// All the pts had better be the same material.
		tri = tri->ListNext();
	}

	edge = CurEdgeRoot.ListNext();
	while ( edge != NULL )
	{
		// All the pts had better be the same material.
		edge = edge->ListNext();
	}

	tri = CurTriRoot.ListNext();
	while ( tri != NULL )
	{
		// And all tris should be the current level or the next.
		ASSERT ( ( tri->mytri.iSlidingWindowLevel == iCurSlidingWindowLevel ) || 
				 ( tri->mytri.iSlidingWindowLevel == iCurSlidingWindowLevel + 1 ) );
		tri = tri->ListNext();
	}

	if ( pOptMesh != NULL )
	{
		pOptMesh->Check();
	}
}


// Bins all the current data.
void Object::BinCurrentObject ( void )
{
	MarkAsDirty();
	while ( CurTriRoot.ListNext() != NULL )
	{
		delete ( CurTriRoot.ListNext() );
	}
	while ( CurEdgeRoot.ListNext() != NULL )
	{
		delete ( CurEdgeRoot.ListNext() );
	}
	while ( CurPtRoot.ListNext() != NULL )
	{
		delete ( CurPtRoot.ListNext() );
	}
}

// Creates the current data from the permanent data.
void Object::MakeCurrentObjectFromPerm ( void )
{
	MarkAsDirty();
	BinCurrentObject();

	// Copy the points.
	for ( MeshPt *ppt = PermPtRoot.ListNext(); ppt != NULL; ppt = ppt->ListNext() )
	{
		// Temporarily link the current and permanent points for when I construct the tris.
		ppt->mypt.pTempPt = new MeshPt ( &CurPtRoot );
		ppt->mypt.pTempPt->mypt = ppt->mypt;
		ppt->mypt.pTempPt->mypt.pTempPt = ppt;
	}
	// Copy the edges.
	for ( MeshEdge *pedge = PermEdgeRoot.ListNext(); pedge != NULL; pedge = pedge->ListNext() )
	{
		MeshEdge *pNewEdge = new MeshEdge (	pedge->pPt1->mypt.pTempPt, 
											pedge->pPt2->mypt.pTempPt, 
											&CurEdgeRoot );
		pNewEdge->myedge = pedge->myedge;
	}
	// Copy the tris.
	int iNumTris = 0;
	for ( MeshTri *ptri = PermTriRoot.ListNext(); ptri != NULL; ptri = ptri->ListNext() )
	{
		MeshTri *pNewTri = new MeshTri (	ptri->pPt1->mypt.pTempPt, 
											ptri->pPt2->mypt.pTempPt, 
											ptri->pPt3->mypt.pTempPt, 
											&CurTriRoot );
		pNewTri->mytri = ptri->mytri;
		iNumTris++;
	}

	ASSERT ( iNumTris == iFullNumTris );

	iNumCollapses = 0;
	iCurSlidingWindowLevel = 0;
	SetNewLevel ( iCurSlidingWindowLevel );
}


// Renders the given material of the current state of the object.
// Set iSlidingWindowLevel to -1 if you don't care about level numbers.
void Object::RenderCurrentObject ( LPDIRECT3DDEVICE8 pd3ddev, int iSlidingWindowLevel )
{
	STDVERTEX vert[3];

	HRESULT hres;

	hres = pd3ddev->SetVertexShader ( STDVERTEX_FVF );

	MeshTri *tri = CurTriRoot.ListNext();
	while ( tri != NULL )
	{
		if ( ( iSlidingWindowLevel == -1 ) ||
				( iSlidingWindowLevel == tri->mytri.iSlidingWindowLevel ) )
		{
			// Draw this one.
			vert[0].v		= tri->pPt1->mypt.vPos;
			vert[0].norm	= tri->pPt1->mypt.vNorm;
			vert[0].tu		= tri->pPt1->mypt.fU;
			vert[0].tv		= tri->pPt1->mypt.fV;

			vert[1].v		= tri->pPt2->mypt.vPos;
			vert[1].norm	= tri->pPt2->mypt.vNorm;
			vert[1].tu		= tri->pPt2->mypt.fU;
			vert[1].tv		= tri->pPt2->mypt.fV;

			vert[2].v		= tri->pPt3->mypt.vPos;
			vert[2].norm	= tri->pPt3->mypt.vNorm;
			vert[2].tu		= tri->pPt3->mypt.fU;
			vert[2].tv		= tri->pPt3->mypt.fV;

			hres = pd3ddev->DrawPrimitiveUP ( D3DPT_TRIANGLELIST, 1, vert, sizeof(vert[0]) );
		}
		tri = tri->ListNext();
	}
}

void Object::RenderCurrentEdges ( LPDIRECT3DDEVICE8 pd3ddev )
{
	STDVERTEX vert[2];

	HRESULT hres;

	hres = pd3ddev->SetVertexShader ( STDVERTEX_FVF );

	MeshEdge *edge = CurEdgeRoot.ListNext();
	while ( edge != NULL )
	{
		// Draw this one.
		vert[0].v		= edge->pPt1->mypt.vPos;
		vert[0].norm	= edge->pPt1->mypt.vNorm;
		vert[0].tu		= edge->pPt1->mypt.fU;
		vert[0].tv		= edge->pPt1->mypt.fV;

		vert[1].v		= edge->pPt2->mypt.vPos;
		vert[1].norm	= edge->pPt2->mypt.vNorm;
		vert[1].tu		= edge->pPt2->mypt.fU;
		vert[1].tv		= edge->pPt2->mypt.fV;

		hres = pd3ddev->DrawPrimitiveUP ( D3DPT_LINELIST, 1, vert, sizeof(vert[0]) );

		edge = edge->ListNext();
	}
}

// Creates and performs a collapse of pptBinned to pptKept.
// Make sure they actually share an edge!
// Make sure the object is fully collapsed already.
void Object::CreateEdgeCollapse ( MeshPt *pptBinned, MeshPt *pptKept )
{
	MarkAsDirty();
	CheckObject();


	// The thing had better be fully collapsed.
	ASSERT ( pNextCollapse == &CollapseRoot );

	MeshEdge *pedge = pptBinned->FindEdge ( pptKept );
	ASSERT ( pedge != NULL );
	GeneralCollapseInfo *pGCI = new GeneralCollapseInfo ( &CollapseRoot );

	pGCI->fError = FindCollapseError ( pptBinned, pptKept->FindEdge ( pptBinned ) );

	// Because deleting a tri breaks the FirstTri, NextTri sequence,
	// we need to find the tris & just store their pointers.
	// Then we delete them.

	pGCI->pptBin = pptBinned;
	pGCI->pptKeep = pptKept;

	const int c_iMaxNumTris = 100;	// Grow as needed.
	MeshTri *pBinnedTris[c_iMaxNumTris];

	MeshTri *ptri;
	int iNumTrisCollapsed = 0;
	BOOL bNeedNewLevel = FALSE;
	for ( ptri = pptBinned->FirstTri(); ptri != NULL; ptri = pptBinned->NextTri() )
	{
		ASSERT ( iNumTrisCollapsed < c_iMaxNumTris );	// Grow c_iMaxNumTris as needed.
		pBinnedTris[iNumTrisCollapsed++] = ptri;
		if ( ptri->mytri.iSlidingWindowLevel != iCurSlidingWindowLevel )
		{
			ASSERT ( ptri->mytri.iSlidingWindowLevel == iCurSlidingWindowLevel + 1 );
			// Need to set a new level before doing this collapse.
			bNeedNewLevel = TRUE;
		}
	}

	if ( bNeedNewLevel )
	{
		// Store which tris were already on this level.
		for ( MeshTri *pTri = CurTriRoot.ListNext(); pTri != NULL; pTri = pTri->ListNext() )
		{
			if ( pTri->mytri.iSlidingWindowLevel != iCurSlidingWindowLevel )
			{
				ASSERT ( pTri->mytri.iSlidingWindowLevel == iCurSlidingWindowLevel + 1 );
				GeneralTriInfo *pTriInfoNew = pGCI->TriNextLevel.AddItem();
				pTriInfoNew->ppt[0] = pTri->pPt1;
				pTriInfoNew->ppt[1] = pTri->pPt2;
				pTriInfoNew->ppt[2] = pTri->pPt3;
			}
		}

		// And set the new level.
		iCurSlidingWindowLevel++;
		SetNewLevel ( iCurSlidingWindowLevel );
	}

	pGCI->iSlidingWindowLevel = iCurSlidingWindowLevel;


	// Now bin them.
	// Store the tris so that pptBinned is always ppt[0].
	// This makes some of the optimised mesh versions simpler to code.
	// Also, make two passes and store the changed tris first, then the
	// binned ones. Again, this makes life easier later on.
	// So all the tris in TriCollapsed should correspond with the
	// same-numbered item in TriOriginal.
	int i;
	for ( i = iNumTrisCollapsed - 1; i >= 0; i-- )
	{
		MeshPt *ppt[3];
		ppt[0] = pBinnedTris[i]->pPt1;
		ppt[1] = pBinnedTris[i]->pPt2;
		ppt[2] = pBinnedTris[i]->pPt3;
		while ( ppt[0] != pptBinned )
		{
			MeshPt *pptTemp = ppt[0];
			ppt[0] = ppt[1];
			ppt[1] = ppt[2];
			ppt[2] = pptTemp;
		}
		ASSERT ( iCurSlidingWindowLevel == pBinnedTris[i]->mytri.iSlidingWindowLevel );
		
		if ( ( ppt[0] == pptKept ) || 
			 ( ppt[1] == pptKept ) || 
			 ( ppt[2] == pptKept ) )
		{
			// This tri will be binned. Store it next time round.
		}
		else
		{
			GeneralTriInfo *pTriInfo = pGCI->TriOriginal.AddItem();
			pTriInfo->ppt[0] = ppt[0];
			pTriInfo->ppt[1] = ppt[1];
			pTriInfo->ppt[2] = ppt[2];

			pBinnedTris[i]->Delete ( TRUE );
			pBinnedTris[i] = NULL;
		}
	}
	// Do it again, adding the binned tris this time.
	int iNumBinned = 0;
	for ( i = iNumTrisCollapsed - 1; i >= 0; i-- )
	{
		if ( pBinnedTris[i] != NULL )
		{
			iNumBinned++;

			MeshPt *ppt[3];
			ppt[0] = pBinnedTris[i]->pPt1;
			ppt[1] = pBinnedTris[i]->pPt2;
			ppt[2] = pBinnedTris[i]->pPt3;
			while ( ppt[0] != pptBinned )
			{
				MeshPt *pptTemp = ppt[0];
				ppt[0] = ppt[1];
				ppt[1] = ppt[2];
				ppt[2] = pptTemp;
			}
			ASSERT ( iCurSlidingWindowLevel == pBinnedTris[i]->mytri.iSlidingWindowLevel );
			
			ASSERT ( ( ppt[0] == pptKept ) || ( ppt[1] == pptKept ) || ( ppt[2] == pptKept ) );

			GeneralTriInfo *pTriInfo = pGCI->TriOriginal.AddItem();
			pTriInfo->ppt[0] = ppt[0];
			pTriInfo->ppt[1] = ppt[1];
			pTriInfo->ppt[2] = ppt[2];

			pBinnedTris[i]->Delete ( TRUE );
			pBinnedTris[i] = NULL;
		}
	}

	// And add in the new tris.
	for ( i = 0; i < iNumTrisCollapsed; i++ )
	{
		GeneralTriInfo *pTriInfo = pGCI->TriOriginal.Item ( i );

		// ppt[0] should always be the binned pt.
		ASSERT ( pTriInfo->ppt[0] == pptBinned );

		// Now, are either of the other two the kept point?
		// If so, these are tris that get binned, rather than remapped.
		if ( ( pTriInfo->ppt[1] == pptKept ) ||
			 ( pTriInfo->ppt[2] == pptKept ) )
		{
			// Binned tri - these should be the last few.
			ASSERT ( i >= iNumTrisCollapsed - iNumBinned );
		}
		else
		{
			// A remapped tri.
			ASSERT ( pGCI->TriCollapsed.Size() == i );
			GeneralTriInfo *pTriInfoNew = pGCI->TriCollapsed.AddItem();
			pTriInfoNew->ppt[0] = pptKept;
			pTriInfoNew->ppt[1] = pTriInfo->ppt[1];
			pTriInfoNew->ppt[2] = pTriInfo->ppt[2];

			// And actually create this tri.
			MeshTri *pTri = new MeshTri ( pTriInfoNew->ppt[0], pTriInfoNew->ppt[1], pTriInfoNew->ppt[2], &CurTriRoot, &CurEdgeRoot );
			ASSERT ( pTri != NULL );
			pTri->mytri.iSlidingWindowLevel = iCurSlidingWindowLevel + 1;
		}
	}


	pGCI->iNumTris = 0;
	for ( MeshTri *pTri = CurTriRoot.ListNext(); pTri != NULL; pTri = pTri->ListNext() )
	{
		pGCI->iNumTris++;
	}

	iNumCollapses++;

}

// Bin the last collapse.
// Returns TRUE if these was a last collapse to do.
BOOL Object::BinEdgeCollapse ( void )
{
	MarkAsDirty();
	GeneralCollapseInfo *pGCI = CollapseRoot.ListNext();
	if ( pGCI == NULL )
	{
		// No collapses to bin.
		ASSERT ( iNumCollapses == 0 );
		return FALSE;
	}
	else
	{
		iNumCollapses--;
		if ( pNextCollapse == &CollapseRoot )
		{
			// Fully collapsed - uncollapse once.
			UndoCollapse();
		}
		if ( pNextCollapse == pGCI )
		{
			// This is the next collapse to be done.
			pNextCollapse = &CollapseRoot;
		}
		pGCI->ListDel();
		delete pGCI;

		return TRUE;
	}
}

// Returns TRUE if a collapse was undone.
BOOL Object::UndoCollapse ( void )
{
	bSomethingHappened = TRUE;

	if ( pNextCollapse->ListNext() == NULL )
	{
		// No more to undo.
		return FALSE;
	}
	else
	{
		pNextCollapse = pNextCollapse->ListNext();

		ASSERT ( pNextCollapse->iSlidingWindowLevel == iCurSlidingWindowLevel );

		int i;
		for ( i = 0; i < pNextCollapse->TriCollapsed.Size(); i++ )
		{
			GeneralTriInfo *pTriInfo = pNextCollapse->TriCollapsed.Item(i);
			MeshTri *pTri = pTriInfo->ppt[0]->FindTri ( pTriInfo->ppt[1], pTriInfo->ppt[2] );
			ASSERT ( pTri != NULL );
			pTri->Delete ( TRUE );
		}

		for ( i = 0; i < pNextCollapse->TriOriginal.Size(); i++ )
		{
			GeneralTriInfo *pTriInfo = pNextCollapse->TriOriginal.Item(i);
			MeshTri *pTri = new MeshTri ( pTriInfo->ppt[0], pTriInfo->ppt[1], pTriInfo->ppt[2], &CurTriRoot, &CurEdgeRoot );
			ASSERT ( pTri != NULL );
			pTri->mytri.iSlidingWindowLevel = iCurSlidingWindowLevel;
		}


		// Now see if the _previous_ collapse is on a different level.
		if ( pNextCollapse->ListNext() != NULL )
		{
			if ( pNextCollapse->ListNext()->iSlidingWindowLevel != iCurSlidingWindowLevel )
			{
				// Need to go back a level.
				ASSERT ( pNextCollapse->ListNext()->iSlidingWindowLevel == iCurSlidingWindowLevel - 1 );
				iCurSlidingWindowLevel--;
				SetNewLevel ( iCurSlidingWindowLevel );

				// Except that some tris will already be at the lower level.
				for ( int i = 0; i < pNextCollapse->TriNextLevel.Size(); i++ )
				{
					GeneralTriInfo *pTriInfo = pNextCollapse->TriNextLevel.Item(i);
					MeshTri *pTri = pTriInfo->ppt[0]->FindTri ( pTriInfo->ppt[1], pTriInfo->ppt[2] );
					ASSERT ( pTri != NULL );
					pTri->mytri.iSlidingWindowLevel = iCurSlidingWindowLevel + 1;
				}
			}
		}

		return TRUE;
	}
}

// Returns TRUE if a collapse was done.
BOOL Object::DoCollapse ( void )
{
	bSomethingHappened = TRUE;

	if ( pNextCollapse == &CollapseRoot )
	{
		// No more to do.
		return FALSE;
	}
	else
	{
		if ( pNextCollapse->iSlidingWindowLevel != iCurSlidingWindowLevel )
		{
			// Need to start a new level.
			ASSERT ( pNextCollapse->TriNextLevel.Size() > 0 );
			ASSERT ( pNextCollapse->iSlidingWindowLevel == iCurSlidingWindowLevel + 1 );
			iCurSlidingWindowLevel++;
			SetNewLevel ( iCurSlidingWindowLevel );
		}
		else
		{
			// No new level to start.
			ASSERT ( pNextCollapse->TriNextLevel.Size() == 0 );
		}


		int i;
		for ( i = 0; i < pNextCollapse->TriOriginal.Size(); i++ )
		{
			GeneralTriInfo *pTriInfo = pNextCollapse->TriOriginal.Item(i);
			MeshTri *pTri = pTriInfo->ppt[0]->FindTri ( pTriInfo->ppt[1], pTriInfo->ppt[2] );
			ASSERT ( pTri != NULL );

			ASSERT ( pTri->mytri.iSlidingWindowLevel == iCurSlidingWindowLevel );

			pTri->Delete ( TRUE );
		}

		for ( i = 0; i < pNextCollapse->TriCollapsed.Size(); i++ )
		{
			GeneralTriInfo *pTriInfo = pNextCollapse->TriCollapsed.Item(i);
			MeshTri *pTri = new MeshTri ( pTriInfo->ppt[0], pTriInfo->ppt[1], pTriInfo->ppt[2], &CurTriRoot, &CurEdgeRoot );
			ASSERT ( pTri != NULL );
			pTri->mytri.iSlidingWindowLevel = iCurSlidingWindowLevel + 1;
		}

		pNextCollapse = pNextCollapse->ListPrev();

		return TRUE;
	}
}



void Object::SetNewLevel ( int iLevel )
{
	for ( MeshTri *pTri = CurTriRoot.ListNext(); pTri != NULL; pTri = pTri->ListNext() )
	{
		pTri->mytri.iSlidingWindowLevel = iLevel;
	}
}

BOOL Object::CollapseAllowedForLevel ( MeshPt *pptBinned, int iLevel )
{
	// All the tris that use the binned point must be at the current level.
	BOOL bRes = TRUE;
	for ( MeshTri *pTri = pptBinned->FirstTri(); pTri != NULL; pTri = pptBinned->NextTri() )
	{
		if ( iLevel != pTri->mytri.iSlidingWindowLevel )
		{
			bRes = FALSE;
		}
	}
	return bRes;
}

// Return the error from this edge collapse.
// Set bTryToCacheResult=TRUE if you can pass pptBinned in multiple times.
// Make sure you call this with bTryToCacheResult=FALSE if any data changes,
//	or you'll confuse the poor thing.
float Object::FindCollapseError ( MeshPt *pptBinned, MeshEdge *pedgeCollapse, BOOL bTryToCacheResult /*= FALSE*/ )
{
	static MeshPt *pptLast;
	static Quad qLast;

	if ( pptBinned == NULL )
	{
		// You can call it like this to flush the cache.
		pptLast = NULL;
		return 0.0f;
	}


	MeshPt *pptKept = pedgeCollapse->OtherPt ( pptBinned );
	ASSERT ( pptKept != NULL );


	Quad qSum;
	if ( bTryToCacheResult && ( pptLast == pptBinned ) )
	{
		qSum = qLast;
	}
	else
	{
		// Find the sum of the QEMs of the tris that will be binned.
		for ( MeshTri *ptri = pptBinned->FirstTri(); ptri != NULL; ptri = pptBinned->NextTri() )
		{
			qSum += Quad ( ptri->pPt1->mypt.vPos, ptri->pPt2->mypt.vPos, ptri->pPt3->mypt.vPos );
		}

		if ( bTryToCacheResult )
		{
			qLast = qSum;
			pptLast = pptBinned;
		}
		else
		{
			pptLast = NULL;
		}
	}

	// And find the error once the collapse has happened.
	return qSum.FindError ( pptKept->mypt.vPos );
}



// Call this if you make a change to the mesh.
// It will mark all the OptimisedMeshes hanging off it as dirty.
void Object::MarkAsDirty ( void )
{
	bSomethingHappened = TRUE;

	if ( pOptMesh != NULL )
	{
		pOptMesh->MarkAsDirty ( g_bShowVIPMInfo );
	}
}



ObjectInstance::ObjectInstance ( Object *pObject /*= NULL*/, ObjectInstance *pRoot /*= NULL*/ )
{
	pObj = pObject;
	D3DXMatrixIdentity( &matOrn );

	ListInit();
	if ( pRoot != NULL )
	{
		ListAddAfter ( pRoot );
	}

	pOptMeshInst = NULL;

	if ( pObj != NULL )
	{
		if ( pObj->pOptMesh != NULL )
		{
			pOptMeshInst = pObj->pOptMesh->CreateInstance ( this );
		}
	}

	iCurCollapses = 0;
}

ObjectInstance::~ObjectInstance ( void )
{
	pObj = NULL;

	ListDel();

	if ( pOptMeshInst != NULL )
	{
		delete pOptMeshInst;
		pOptMeshInst = NULL;
	}
}



void ObjectInstance::RenderCurrentObject ( LPDIRECT3DDEVICE8 pd3ddev, int iSlidingWindowLevel /*= -1*/, BOOL bShowOptiMesh /*= FALSE*/ )
{
	if ( bShowOptiMesh && ( pOptMeshInst != NULL ) )
	{
		pOptMeshInst->RenderCurrentObject ( pd3ddev, iCurCollapses );
	}
	else
	{
		// Want plain bog-slow rendering, or the OptMeshInstance doesn't exist.
		pObj->RenderCurrentObject ( pd3ddev, iSlidingWindowLevel );
	}
}


void ObjectInstance::SetNumCollapses ( int iNum )
{
	iCurCollapses = iNum;
}

int ObjectInstance::GetNumCollapses ( void )
{
	return iCurCollapses;
}


// Call before D3D leaves.
void ObjectInstance::AboutToChangeDevice ( void )
{
	// Not actually much to do.
	ASSERT ( pObj != NULL );
	pObj->AboutToChangeDevice();

	if ( pOptMeshInst != NULL )
	{
		pOptMeshInst->AboutToChangeDevice();
	}
}





#ifdef DEBUG

// Set this to 1 if you want to do extra debugging on the OptimisedMesh stuff.
// But it will change where things like index buffers go, and so potentially
// hide other bugs.
// It's also very slow, because it does a lot of checking.
#define EXTRA_DEBUGGING 0

#else
// Not a debug build - no extras.
#define EXTRA_DEBUGGING 0
#endif


OptimisedMesh::OptimisedMesh ( void )
{
	iVersion = 1234;
	bDirty = TRUE;
	bWillGetInfo = FALSE;
}

OptimisedMesh::~OptimisedMesh() {}


// No real need to override this - override Update() instead.
// Tell this method that the underlying mesh has changed.
// bWillGetInfo = TRUE if you are going to call any of the Info* functions.
// This causes a speed hit, so only do it when necessary.
void OptimisedMesh::MarkAsDirty ( BOOL bNewWillGetInfo )
{
	iVersion++;
	bDirty = TRUE;
	bWillGetInfo = bNewWillGetInfo;
}



OptimisedMeshInstance::OptimisedMeshInstance ( void )
{
	iVersion = 1233;		// One less than the initialiser of OptimisedMesh
}

OptimisedMeshInstance::~OptimisedMeshInstance() {}

///////// SLIDING WINDOW VIPM ////////////
// A table of these exists, one per collapse.
struct SlidingWindowRecord
{
	DWORD	dwFirstIndexOffset;			// Offset of the first index in the index buffer to start at (note! no retrictions. Can easily be >64k)
	WORD	wNumTris;					// Number of tris to render (most cards can't do more than 65536)
	WORD	wNumVerts;					// Number of vertices to render with (using WORD indices)
};

class OMSlidingWindow : public OptimisedMesh
{
	friend class OMISlidingWindow;

	Object *pObj;

	int										iNumVerts;			// Number of verts at full rez.
	int										iNumCollapses;		// Total number of collapses.
	int										iNumLevels;			// Total number of levels.
	int										iSizeOfIB;			// In indices. Just for info.
	//ArbitraryList<WORD>						wIndices;			// The index list, all levels clumped together. Always using TRILISTs.
	ArbitraryList<SlidingWindowRecord>		swrRecords;			// The records of the collapses.

	BOOL									bOptimised;			// Have the tris been reordered optimally?

	LPDIRECT3DVERTEXBUFFER8		pVB;							// The VB with the vertices in.
	LPDIRECT3DINDEXBUFFER8		pIB;							// The index list, all levels clumped together. Always using TRILISTs.

public:
	OMSlidingWindow ( Object *pObject );
	virtual ~OMSlidingWindow ( void );
	virtual OptimisedMeshInstance *CreateInstance ( ObjectInstance *pObjectInstance );
	virtual void Check ( void );
	virtual void Update ( void );
	virtual void AboutToChangeDevice ( void );
};


class OMISlidingWindow : public OptimisedMeshInstance
{
	friend class OMSlidingWindow;

	ObjectInstance *pObjInst;
	OMSlidingWindow *pOptMesh;

	SlidingWindowRecord *pswrLast;		// For the Info* functions.

	// No per-instance VIPM data. Hooray!


public:

	OMISlidingWindow ( ObjectInstance *pObjectInstance, OMSlidingWindow *pOptimisedMesh );
	virtual ~OMISlidingWindow ( void );
	virtual void RenderCurrentObject ( LPDIRECT3DDEVICE8 pd3ddev, int iLoD );
	virtual BOOL bNeedsUpdate ( void );
	virtual void Update ( void );
	virtual void Check ( void );
	virtual void AboutToChangeDevice ( void );
	virtual const void InfoGetGlobal ( DWORD *pdwMemoryUsed, DWORD *pdwOfWhichAGP );
	virtual const void InfoGetInstance ( DWORD *pdwMemoryUsed, DWORD *pdwOfWhichAGP, DWORD *pdwVerticesLoaded, DWORD *pdwRealTrisDrawn, DWORD *pdwTotalVertices );
};




OMSlidingWindow::OMSlidingWindow ( Object *pObject )
{
	pObj = pObject;
	pVB = NULL;
	pIB = NULL;
	bOptimised = FALSE;
}

OMSlidingWindow::~OMSlidingWindow ( void )
{
	pObj = NULL;
	ASSERT ( pVB == NULL );
	ASSERT ( pIB == NULL );
}

void OMSlidingWindow::AboutToChangeDevice ( void )
{
	HRESULT hres = g_pd3dDevice->SetStreamSource ( 0, NULL, 0 );
	ASSERT ( SUCCEEDED ( hres ) );
	hres = g_pd3dDevice->SetIndices ( NULL, 0 );
	ASSERT ( SUCCEEDED ( hres ) );
	SAFE_RELEASE ( pVB );
	SAFE_RELEASE ( pIB );

	MarkAsDirty ( g_bShowVIPMInfo );
	bOptimised = FALSE;
}


// Create an instance of this optimised mesh, and returns the pointer to it.
// Pass in the object instance you wish to associate it with.
OptimisedMeshInstance *OMSlidingWindow::CreateInstance ( ObjectInstance *pObjectInstance )
{
	return new OMISlidingWindow ( pObjectInstance, this );
}


void OMSlidingWindow::Check ( void )
{
}


void OMSlidingWindow::Update ( void )
{
	if ( !bDirty && ( bOptimised == g_bOptimiseVertexOrder ) )
	{
		// Nothing to do!
		return;
	}

	bDirty = FALSE;
	iVersion++;			// Just for luck.


	bOptimised = g_bOptimiseVertexOrder;


	SAFE_RELEASE ( pVB );
	SAFE_RELEASE ( pIB );
	swrRecords.SizeTo ( 0 );




	MeshPt *pt;
	MeshTri *tri;


	// Undo all the collapses, so we start from the maximum mesh.
	while ( pObj->UndoCollapse() ) {}


	// How many vertices are we looking at?
	iNumVerts = 0;
	for ( pt = pObj->CurPtRoot.ListNext(); pt != NULL; pt = pt->ListNext() )
	{
		pt->mypt.dwIndex = -1;
		iNumVerts++;
	}
	// Er... word indices, guys... nothing supports 32-bit indices yet.
	ASSERT ( iNumVerts < 65535 );


	// How many tris are we looking at?
	int iNumTris = 0;
	for ( tri = pObj->CurTriRoot.ListNext(); tri != NULL; tri = tri->ListNext() )
	{
		tri->mytri.dwIndex = -1;
		iNumTris++;
	}
	// A lot of cards have this annoying limit - see D3DCAPS8.MaxPrimitiveCount, which is
	// exactly 65535 for DX7 and previous devices. So might as well stick with that number.
	ASSERT ( iNumTris < 65535 );




	// Create a place to put indices while we build up the list. Because we don't know
	// how many we need yet, we can't create the IB until the end. Then we'll copy this into it.
	ArbitraryList<WORD> wIndices;
	wIndices.SizeTo ( 0 );




	// Create the vertex buffer.
	HRESULT hres = g_pd3dDevice->CreateVertexBuffer ( iNumVerts * sizeof ( STDVERTEX ),
										D3DUSAGE_WRITEONLY,
										STDVERTEX_FVF,
										D3DPOOL_DEFAULT,
										&pVB );
	ASSERT ( SUCCEEDED ( hres ) );

	STDVERTEX *pVertices, *pCurVertex;
	hres = pVB->Lock ( 0, 0, (BYTE **)(&pVertices), 0 );
	ASSERT ( SUCCEEDED ( hres ) );

	pCurVertex = pVertices;






	// Now do all the collapses, so we start from the minimum mesh.
	// Along the way, mark the vertices in reverse order.
	int iCurVerts = iNumVerts;
	int iCurCollapse = 0;
	int iCurSlidingWindowLevel = 0;
	while ( TRUE )
	{
		GeneralCollapseInfo *pCollapse = pObj->pNextCollapse;
		if ( pObj->pNextCollapse == &(pObj->CollapseRoot) )
		{
			break;
		}

		iCurSlidingWindowLevel = pCollapse->iSlidingWindowLevel;

		iCurCollapse++;

		iCurVerts--;
		pCollapse->pptBin->mypt.dwIndex = iCurVerts;

		pObj->DoCollapse();
	}

	iNumCollapses = iCurCollapse;


	// Add the remaining existing pts in any old order.
	WORD wCurIndex = 0;
	for ( pt = pObj->CurPtRoot.ListNext(); pt != NULL; pt = pt->ListNext() )
	{
		if ( pt->mypt.dwIndex == (DWORD)-1 )
		{
			// Not binned in a collapse.
			pt->mypt.dwIndex = wCurIndex;
			pCurVertex->v = pt->mypt.vPos;
			pCurVertex->norm = pt->mypt.vNorm;
			pCurVertex->tu = pt->mypt.fU;
			pCurVertex->tv = pt->mypt.fV;

			pCurVertex++;
			wCurIndex++;
		}
	}

	// Should meet in the middle!
	ASSERT ( wCurIndex == iCurVerts );


	// And count the tris that are left.
	int iCurNumTris = 0;
	for ( tri = pObj->CurTriRoot.ListNext(); tri != NULL; tri = tri->ListNext() )
	{
		iCurNumTris++;
	}



	// Reserve space for the collapse table - this is stored so that
	// "number of collapses" is the index. So we'll be adding from the
	// back.
	iCurCollapse++;		// Implicit "last collapse" is state after last collapse.
	swrRecords.SizeTo ( iCurCollapse );
	// And add the last one.
	iCurCollapse--;
	SlidingWindowRecord *swr = swrRecords.Item ( iCurCollapse );
	swr->dwFirstIndexOffset = 0;
	swr->wNumTris = (WORD)iCurNumTris;
	swr->wNumVerts = wCurIndex;


	// Now go through each level in turn.

	int iCurTriBinned = 0;


	// Useful thing.
	ArbitraryList<WORD> wTempIndices;


	int iMaxSlidingWindowLevel = iCurSlidingWindowLevel;

	while ( TRUE )
	{
		// Now we go through the collapses for this level, undoing them.
		// As we go, we add the binned tris to the start of the index list,
		// 

		// This coming list will be three sections:
		// 1. the tris that get binned by the splits.
		// 2. the tris that aren't affected.
		// 3. the tris that are added by the splits.
		//
		// We know that at the moment, we are just rendering
		// 1+2, which must equal the number of tris.
		// So 3 starts iCurNumTris on from the start of 1.
		int iCurTriAdded = iCurTriBinned + iCurNumTris;
		wIndices.SizeTo ( iCurTriAdded * 3 );



		int iJustCheckingNumTris = 0;
		for ( tri = pObj->CurTriRoot.ListNext(); tri != NULL; tri = tri->ListNext() )
		{
			tri->mytri.dwIndex = -1;		// Mark them as not being in a collapse.
			iJustCheckingNumTris++;
		}
		ASSERT ( iJustCheckingNumTris == iCurNumTris );


		BOOL bJustStartedANewLevel = TRUE;


		// Now undo all the collapses for this level in turn, adding vertices,
		// binned tris, and SlidingWindowRecords as we go.
		while ( ( pObj->pNextCollapse->ListNext() != NULL ) &&
			    ( pObj->pNextCollapse->ListNext()->iSlidingWindowLevel == iCurSlidingWindowLevel ) )
		{
			GeneralCollapseInfo *pCollapse = pObj->pNextCollapse->ListNext();


			// If we've just started a new level, EXCEPT on the last level,
			// we don't need to store the post-collapse version of the tris,
			// since we'll just switch down to the next level instead.
			if ( !bJustStartedANewLevel || ( iCurSlidingWindowLevel == iMaxSlidingWindowLevel  ) )
			{
				// These tris will be binned by the split.
				if ( pCollapse->TriCollapsed.Size() > 0 )
				{
					wTempIndices.SizeTo(pCollapse->TriCollapsed.Size() * 3);
					for ( int i = 0; i < pCollapse->TriCollapsed.Size(); i++ )
					{
						GeneralTriInfo *pTriInfo = pCollapse->TriCollapsed.Item(i);
						MeshTri *tri = pTriInfo->ppt[0]->FindTri ( pTriInfo->ppt[1], pTriInfo->ppt[2] );
						ASSERT ( tri != NULL );
						ASSERT ( tri->mytri.dwIndex == -1 );	// Should not have been in a collapse this level.

						ASSERT ( pTriInfo->ppt[0]->mypt.dwIndex < wCurIndex );
						ASSERT ( pTriInfo->ppt[1]->mypt.dwIndex < wCurIndex );
						ASSERT ( pTriInfo->ppt[2]->mypt.dwIndex < wCurIndex );
						*wTempIndices.Item(i*3+0) = (WORD)pTriInfo->ppt[0]->mypt.dwIndex;
						*wTempIndices.Item(i*3+1) = (WORD)pTriInfo->ppt[1]->mypt.dwIndex;
						*wTempIndices.Item(i*3+2) = (WORD)pTriInfo->ppt[2]->mypt.dwIndex;
						iCurNumTris--;
					}

					// Now try to order them as best you can.
					if ( g_bOptimiseVertexOrder )
					{
						OptimiseVertexCoherencyTriList ( wTempIndices.Ptr(), pCollapse->TriCollapsed.Size() );
					}

					// And write them to the index list.
					wIndices.CopyFrom ( iCurTriBinned * 3, wTempIndices, 0, 3 * pCollapse->TriCollapsed.Size() );
					//memcpy ( wIndices.Item ( iCurTriBinned * 3 ), wTempIndices.Ptr(), sizeof(WORD) * 3 * pCollapse->TriCollapsed.Size() );
					iCurTriBinned += pCollapse->TriCollapsed.Size();
				}
			}
			else
			{
				// Keep the bookkeeping happy.
				iCurNumTris -= pCollapse->TriCollapsed.Size();
				// And move the added tris pointer back, because it didn't know we weren't
				// going to store these.
				iCurTriAdded -= pCollapse->TriCollapsed.Size();
			}

			bJustStartedANewLevel = FALSE;


			// Do the uncollapse.
			pObj->UndoCollapse();


			// Add the unbinned vertex.
			MeshPt *pPt = pCollapse->pptBin;
			ASSERT ( pPt->mypt.dwIndex == wCurIndex );
			pCurVertex->v = pPt->mypt.vPos;
			pCurVertex->norm = pPt->mypt.vNorm;
			pCurVertex->tu = pPt->mypt.fU;
			pCurVertex->tv = pPt->mypt.fV;
			pCurVertex++;
			wCurIndex++;


			// These tris will be added by the split.
			if ( pCollapse->TriOriginal.Size() > 0 )
			{
				wTempIndices.SizeTo(pCollapse->TriOriginal.Size() * 3);
				for ( int i = 0; i < pCollapse->TriOriginal.Size(); i++ )
				{
					GeneralTriInfo *pTriInfo = pCollapse->TriOriginal.Item(i);
					MeshTri *tri = pTriInfo->ppt[0]->FindTri ( pTriInfo->ppt[1], pTriInfo->ppt[2] );
					ASSERT ( tri != NULL );
					tri->mytri.dwIndex = 1;		// Mark it has having been in a collapse.

					ASSERT ( pTriInfo->ppt[0]->mypt.dwIndex < wCurIndex );
					ASSERT ( pTriInfo->ppt[1]->mypt.dwIndex < wCurIndex );
					ASSERT ( pTriInfo->ppt[2]->mypt.dwIndex < wCurIndex );
					*wTempIndices.Item(i*3+0) = (WORD)pTriInfo->ppt[0]->mypt.dwIndex;
					*wTempIndices.Item(i*3+1) = (WORD)pTriInfo->ppt[1]->mypt.dwIndex;
					*wTempIndices.Item(i*3+2) = (WORD)pTriInfo->ppt[2]->mypt.dwIndex;
					iCurNumTris++;
				}

				// Now try to order them as best you can.
				if ( g_bOptimiseVertexOrder )
				{
					OptimiseVertexCoherencyTriList ( wTempIndices.Ptr(), pCollapse->TriOriginal.Size() );
				}

				// And write them to the index list.
				wIndices.SizeTo ( ( iCurTriAdded + pCollapse->TriOriginal.Size() ) * 3 );
				wIndices.CopyFrom ( iCurTriAdded * 3, wTempIndices, 0, 3 * pCollapse->TriOriginal.Size() );
				//memcpy ( wIndices.Item ( iCurTriAdded * 3 ), wTempIndices.Ptr(), sizeof(WORD) * 3 * pCollapse->TriOriginal.Size() );
				iCurTriAdded += pCollapse->TriOriginal.Size();
			}


			// Add the collapse record.
			iCurCollapse--;
			SlidingWindowRecord *swr = swrRecords.Item ( iCurCollapse );
			swr->dwFirstIndexOffset = iCurTriBinned * 3;
			swr->wNumTris = (WORD)iCurNumTris;
			swr->wNumVerts = wCurIndex;
		}


		// OK, finished this level. Any tris that are left with an index of -1
		// were not added during this level, and therefore need to be
		// added into the middle of the list.
		iJustCheckingNumTris = 0;
		int iTempTriNum = 0;
		for ( tri = pObj->CurTriRoot.ListNext(); tri != NULL; tri = tri->ListNext() )
		{
			iJustCheckingNumTris++;
			if ( tri->mytri.dwIndex == -1 )
			{
				// This tri has not been created by a collapse this level.
				wTempIndices.SizeTo ( iTempTriNum * 3 + 3 );
				*(wTempIndices.Item(iTempTriNum*3+0)) = tri->pPt1->mypt.dwIndex;
				*(wTempIndices.Item(iTempTriNum*3+1)) = tri->pPt2->mypt.dwIndex;
				*(wTempIndices.Item(iTempTriNum*3+2)) = tri->pPt3->mypt.dwIndex;
				iTempTriNum++;
			}
		}
		ASSERT ( iJustCheckingNumTris == iCurNumTris );

		// Now try to order them as best you can.
		if ( g_bOptimiseVertexOrder )
		{
			OptimiseVertexCoherencyTriList ( wTempIndices.Ptr(), iTempTriNum );
		}

		// And write them to the index list.
		wIndices.CopyFrom ( iCurTriBinned * 3, wTempIndices, 0, 3 * iTempTriNum );
		//memcpy ( wIndices.Item ( iCurTriBinned * 3 ), wTempIndices.Ptr(), sizeof(WORD) * 3 * iTempTriNum );

		if ( pObj->pNextCollapse->ListNext() == NULL )
		{
			// No more collapses.
			ASSERT ( iCurCollapse == 0 );
			ASSERT ( iCurSlidingWindowLevel == 0 );
			break;
		}

		// Start a new level by skipping past all the indices so far.
		iCurTriBinned += iCurNumTris;
		// Check the maths.
		ASSERT ( iCurTriBinned == iCurTriAdded );

	    iCurSlidingWindowLevel = pObj->pNextCollapse->ListNext()->iSlidingWindowLevel;
	}



	hres = pVB->Unlock();
	ASSERT ( SUCCEEDED ( hres ) );

	// And now check everything is OK.
	ASSERT ( swrRecords.Size() == iNumCollapses + 1 );
	for ( int i = 0; i <= iNumCollapses; i++ )
	{
		SlidingWindowRecord *swr = swrRecords.Item ( i );
		for ( int j = 0; j < swr->wNumTris * 3; j++ )
		{
			ASSERT ( *(wIndices.Item ( j + swr->dwFirstIndexOffset )) < swr->wNumVerts );
		}
	}


	DWORD dwFlags = 0;
#if !EXTRA_DEBUGGING
// If doing extra debugging, we're always going to do reads from the IB.
	if ( !bWillGetInfo )
	{
		// We don't need to do reads from the IB to get vertex-cache info.
		dwFlags |= D3DUSAGE_WRITEONLY;
	}
#endif


	// Create the index buffer.
	iSizeOfIB = wIndices.Size();
	hres = g_pd3dDevice->CreateIndexBuffer ( sizeof(WORD) * wIndices.Size(),
										dwFlags,
										D3DFMT_INDEX16,
										D3DPOOL_DEFAULT,
										&pIB );
	ASSERT ( SUCCEEDED ( hres ) );

	// Copy to the index buffer
	WORD *pwDest;
	hres = pIB->Lock ( 0, sizeof(WORD) * wIndices.Size(), (BYTE**)&pwDest, 0 );
	ASSERT ( SUCCEEDED ( hres ) );

	memcpy ( pwDest, wIndices.Ptr(), sizeof(WORD) * wIndices.Size() );

	hres = pIB->Unlock();
	ASSERT ( SUCCEEDED ( hres ) );

	wIndices.SizeTo ( 0 );

}








OMISlidingWindow::OMISlidingWindow ( ObjectInstance *pObjectInstance, OMSlidingWindow *pOptimisedMesh )
{
	pObjInst = pObjectInstance;
	pOptMesh = pOptimisedMesh;
}

OMISlidingWindow::~OMISlidingWindow ( void )
{
	pObjInst = NULL;
	pOptMesh = NULL;
}

void OMISlidingWindow::AboutToChangeDevice ( void )
{
	ASSERT ( pObjInst != NULL );
	pOptMesh->AboutToChangeDevice();
}

const void OMISlidingWindow::InfoGetGlobal ( DWORD *pdwMemoryUsed, DWORD *pdwOfWhichAGP )
{
	ASSERT ( pOptMesh != NULL );
	ASSERT ( pOptMesh->bWillGetInfo );

	DWORD dwMemoryUsed = 0;
	DWORD dwOfWhichAGP = 0;

	// The thing itself.
	dwMemoryUsed += sizeof ( OMSlidingWindow );
	// Collapse list.
	dwMemoryUsed += pOptMesh->swrRecords.Size() * sizeof ( SlidingWindowRecord );
	// The size of the IB.
	dwOfWhichAGP += pOptMesh->iSizeOfIB * sizeof ( WORD );
	// And the size of the VB.
	dwOfWhichAGP += pOptMesh->iNumVerts * sizeof ( STDVERTEX );

	// Add the AGP memory in.
	dwMemoryUsed += dwOfWhichAGP;


	if ( pdwMemoryUsed != NULL )
	{
		*pdwMemoryUsed = dwMemoryUsed;
	}
	if ( pdwOfWhichAGP != NULL )
	{
		*pdwOfWhichAGP = dwOfWhichAGP;
	}

}

const void OMISlidingWindow::InfoGetInstance ( DWORD *pdwMemoryUsed, DWORD *pdwOfWhichAGP, DWORD *pdwVerticesLoaded, DWORD *pdwRealTrisDrawn, DWORD *pdwTotalVertices )
{
	ASSERT ( pOptMesh != NULL );
	ASSERT ( pOptMesh->bWillGetInfo );

	DWORD dwMemoryUsed = 0;
	DWORD dwOfWhichAGP = 0;


	// The thing itself.
	dwMemoryUsed += sizeof ( OMISlidingWindow );
	// And that's it per-instance!


	// Add the AGP memory in.
	dwMemoryUsed += dwOfWhichAGP;

	if ( pdwMemoryUsed != NULL )
	{
		*pdwMemoryUsed = dwMemoryUsed;
	}
	if ( pdwOfWhichAGP != NULL )
	{
		*pdwOfWhichAGP = dwOfWhichAGP;
	}


	if ( pdwVerticesLoaded != NULL )
	{
		ASSERT ( pswrLast != NULL );


		// Find the vertex-cache info for the current render.
		WORD *pwIndices;
		HRESULT hres = pOptMesh->pIB->Lock ( 0,
							pOptMesh->iSizeOfIB * sizeof ( WORD ),
							(BYTE**)&pwIndices,
							D3DLOCK_READONLY );
		ASSERT ( SUCCEEDED ( hres ) );

		float fVertexScore = GetNumVerticesLoadedTriList ( pwIndices + pswrLast->dwFirstIndexOffset, pswrLast->wNumTris );
		*pdwVerticesLoaded = (DWORD)(fVertexScore + 0.4999f);

		hres = pOptMesh->pIB->Unlock();
		ASSERT ( SUCCEEDED ( hres ) );
	}

	if ( pdwRealTrisDrawn != NULL )
	{
		ASSERT ( pswrLast != NULL );

		// We don't get degenerate tris in Sliding Window.
		*pdwRealTrisDrawn = pswrLast->wNumTris;
	}

	if ( pdwTotalVertices != NULL )
	{
		*pdwTotalVertices = pswrLast->wNumVerts;
	}

}



// Renders the given material of the object with the given level of detail.
void OMISlidingWindow::RenderCurrentObject ( LPDIRECT3DDEVICE8 pd3ddev, int iLoD )
{
	// Do an update if necessary.
	if ( bNeedsUpdate() )
	{
		Update();
	}


	// Find the record.
	if ( iLoD < 0 )
	{
		iLoD = 0;
	}
	else if ( iLoD > pOptMesh->iNumCollapses )
	{
		iLoD = pOptMesh->iNumCollapses;
	}
	SlidingWindowRecord *pswr = pOptMesh->swrRecords.Item ( iLoD );

	// Store it for the Info* calls.
	pswrLast = pswr;


	// And draw the object.
	HRESULT hres;
	
	hres = pd3ddev->SetVertexShader ( STDVERTEX_FVF );
	ASSERT ( SUCCEEDED ( hres ) );

	ASSERT ( pOptMesh->pIB != NULL );
	hres = pd3ddev->SetIndices ( pOptMesh->pIB, 0 );
	ASSERT ( SUCCEEDED ( hres ) );

	ASSERT ( pOptMesh->pVB != NULL );
	hres = pd3ddev->SetStreamSource ( 0, pOptMesh->pVB, sizeof ( STDVERTEX ) );
	ASSERT ( SUCCEEDED ( hres ) );

	if ( g_iMaxNumTrisDrawn > 0 )
	{
		// Limit the number of tris.
		int iNumTris = g_iMaxNumTrisDrawn;
		if ( pswr->wNumTris < iNumTris )
		{
			iNumTris = pswr->wNumTris;
		}
		hres = pd3ddev->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST,
												0,
												pswr->wNumVerts,
												pswr->dwFirstIndexOffset,
												iNumTris );
		ASSERT ( SUCCEEDED ( hres ) );
	}
	else
	{
		hres = pd3ddev->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST,
												0,
												pswr->wNumVerts,
												pswr->dwFirstIndexOffset,
												pswr->wNumTris );
		ASSERT ( SUCCEEDED ( hres ) );
	}
}

BOOL OMISlidingWindow::bNeedsUpdate ( void )
{
	ASSERT ( pOptMesh != NULL );
	if ( pOptMesh->bOptimised != g_bOptimiseVertexOrder )
	{
		return TRUE;
	}
	if ( iVersion != pOptMesh->iVersion )
	{
		return TRUE;
	}
	return FALSE;
}


void OMISlidingWindow::Update ( void )
{
	ASSERT ( pOptMesh != NULL );
	pOptMesh->Update();

	if ( bNeedsUpdate() )
	{
		// Not much to do really :-)
		iVersion = pOptMesh->iVersion;
	}

	Check();
}


void OMISlidingWindow::Check ( void )
{
	// Check my global data is the same type as me!
	ASSERT ( pOptMesh != NULL );
	pOptMesh->Check();
}


////////////////////////// Utility functions /////////////////////
// Creates the given type of optimised mesh, and returns the pointer to it.
/*static*/ OptimisedMesh *OptimisedMesh::Create ( Object *pObject )
{
	return new OMSlidingWindow ( pObject );
}


// Some functions for simulating a vertex cache.

void CacheInit ( int iNumberOfVertices );
void CacheBin ( void );
// Returns the weighted number of vertex misses so far.
float CacheAddTri ( WORD wIndex1, WORD wIndex2, WORD wIndex3, BOOL bDontActuallyAdd = FALSE );
// Returns the weighted number of vertex misses so far (after the tri has been removed).
#if 0
float CacheRemoveTri ( void );
#else
void CacheRemoveTri ( void );
#endif





float GetNumVerticesLoadedTriList ( WORD *pwList, int iHowManyTris )
{
	// What's the top index?
	WORD wHighest = 0;
	int i;
	for ( i = 0; i < iHowManyTris * 3; i++ )
	{
		if ( wHighest < pwList[i] )
		{
			wHighest = pwList[i];
		}
	}

	CacheInit ( wHighest + 1 );

	// Let's find the current score of this list.
	float fScore = 0.0f;
	WORD *pwCurTri = pwList;
	for ( i = 0; i < iHowManyTris; i++ )
	{
		fScore += CacheAddTri ( pwCurTri[0], pwCurTri[1], pwCurTri[2] );
		pwCurTri += 3;
	}

	CacheBin();

	return fScore;
}




// This stuff simulates a FIFO cache. There are lots of types, but (a) this is the
// easiest to model and (b) it's actually used in lots of real hardware (mainly
// because it's also the easiest to construct).


// Some data structures to track cache scores.


struct CacheTypeInfo
{
	// How long the FIFO is.
	int		iLength;
	// The score for a vertex miss.
	// If they all sum to 1.0f, you get a weighted average
	// of the number of vertices loaded coming out the end.
	float	fMissScore;
};


// The lengths of the various FIFOs and their relative weightings.
// These weightings are purely arbitrary - bias them to whichever
// your target hardware area is. You can usually find out FIFO
// lengths (or equivalent) from your friendly IHV devrel team.

#if 0

// General settings.
const int c_iCacheNumTypes = 4;
const CacheTypeInfo c_ctiCacheTypeSize[c_iCacheNumTypes] = 
{
	{  3, 0.15f }, 		// Sort of strip simulation. Sort of.
	{  4, 0.15f },		// Some hardware knows about quads.
	{ 12, 0.35f },		// Newer hardware is around 12-16. Heavier weighting on them.
	{ 16, 0.35f }
};

#elif 0

// Dedicated 12-entry cache.
const int c_iCacheNumTypes = 1;
const CacheTypeInfo c_ctiCacheTypeSize[c_iCacheNumTypes] = 
{
	{ 12, 1.0f }
};

#else

// 12-entry cache, with some weighting towards sharing that last tri.
// This slightly speeds up things that have a decent-sized secondary cache,
// but also a lightning-fast primary cache of the last tri.
// Only needs a tiny weighting to clean up the 12-entry-only stuff,
// which tends to look a bit fragmented, since it doesn't care where in
// the last 12 the used vertices are.
// Note that using this will increase the displayed "miss" count a bit, even
// though there are (usually) no extra misses for the second-level cache,
// and the result will still be optimal.
const int c_iCacheNumTypes = 2;
const CacheTypeInfo c_ctiCacheTypeSize[c_iCacheNumTypes] = 
{
	{ 20, 0.95f },
	{ 3, 0.05f }
};


#endif



struct CacheTypeTriInfo
{
	int		iIndicesAdded;		// Were 0, 1, 2 or 3 indices added to the start of the FIFO.
	WORD	wIndicesBinned[4];	// And the indices that fell off the end.
};


struct CacheTri
{
	WORD				wIndex[3];					// The tri added this time.
	CacheTypeTriInfo	ctti[c_iCacheNumTypes];		// The FIFO histories.
};


struct CacheVert
{
	BYTE				bRefcount[c_iCacheNumTypes];	// How many times each vertex is referenced by the cache.
};

ArbitraryList<CacheVert>	cvCacheStatus;
ArbitraryList<CacheTri>		ctCacheHistory;		// The cache history.

struct CurrentCacheState
{
	int iCacheCurrentMisses;		// The current number of vertex loads for each type.
	WORD *pwCacheFifoState;			// The current FIFO contents (circular buffer).
	int iCacheFifoWritePos;			// Where the current "tail" of the FIFO is.
} ccsState[c_iCacheNumTypes];


#ifdef _DEBUG
// Set this to 1 if you want to check the cache works, but it's pretty slow.
#define CACHE_DEBUG 0
#else
#define CACHE_DEBUG 0
#endif


void CacheInit ( int iNumberOfVertices )
{
	int i;
	for ( i = 0; i < c_iCacheNumTypes; i++ )
	{
		ccsState[i].iCacheCurrentMisses = 0;
		ccsState[i].pwCacheFifoState = new WORD [ c_ctiCacheTypeSize[i].iLength ];
		for ( int j = 0; j < c_ctiCacheTypeSize[i].iLength; j++ )
		{
			ccsState[i].pwCacheFifoState[j] = 0xFFFF;		// "unused" value - won't cause a cache hit.
		}
		ccsState[i].iCacheFifoWritePos = 0;
	}
	ctCacheHistory.SizeTo ( 0 );
	cvCacheStatus.SizeTo ( iNumberOfVertices );
	for ( i = 0; i < iNumberOfVertices; i++ )
	{
		for ( int j = 0; j < c_iCacheNumTypes; j++ )
		{
			(cvCacheStatus.Item(i))->bRefcount[j] = 0;
		}
	}
}

void CacheBin ( void )
{
	for ( int i = 0; i < c_iCacheNumTypes; i++ )
	{
		ASSERT ( ccsState[i].pwCacheFifoState != NULL );
		delete[] ccsState[i].pwCacheFifoState;
		ccsState[i].pwCacheFifoState = NULL;
	}
	ctCacheHistory.SizeTo ( 0 );
	cvCacheStatus.SizeTo ( 0 );
}


// Returns the number of extra vertex misses due to this addition.
// Set bDontActuallyAdd to TRUE to just find the number of cache misses if this tri was added.
float CacheAddTri ( WORD wIndex1, WORD wIndex2, WORD wIndex3, BOOL bDontActuallyAdd )
{
	WORD wIndex[3];
	wIndex[0] = wIndex1;
	wIndex[1] = wIndex2;
	wIndex[2] = wIndex3;

	CacheTri *pct = NULL;
	if ( !bDontActuallyAdd )
	{
		pct = ctCacheHistory.AddItem();
		pct->wIndex[0] = wIndex[0];
		pct->wIndex[1] = wIndex[1];
		pct->wIndex[2] = wIndex[2];
	}

	float fTotalScore = 0.0f;
	for ( int iType = 0; iType < c_iCacheNumTypes; iType++ )
	{
		CurrentCacheState &ccs = ccsState[iType];

		int iCurNumMisses = ccs.iCacheCurrentMisses;

		// We must scan all three indices before adding any, otherwise you get nasty cases like:
		// 0,1,2 followed by 3,0,1. If you add as you go along, the 3 kicks out the 0,
		// then the 0 kicks out the 1, then the 1 kicks out the 2. And you have
		// 0 cache hits, rather than 2.
		// Most hardware does all three checks before adding as well.
		// NOTE! If your target hardware doesn't, shorten the FIFO a bit to get roughly the same behaviour.
		BOOL bFound[3];
		int iVert;
		for ( iVert = 0; iVert < 3; iVert++ )
		{
			bFound[iVert] = ( (cvCacheStatus.Item(wIndex[iVert]))->bRefcount[iType] > 0 );

#if CACHE_DEBUG
			WORD wInd = wIndex[iVert];
			// Is this in the cache already?
			int j;
			BOOL bFoundThis = FALSE;
			//bFound[iVert] = FALSE;
			for ( j = c_ctiCacheTypeSize[iType].iLength - 1 ; j >= 0; j-- )
			{
				if ( ccs.pwCacheFifoState[j] == wInd )
				{
					// Found.
					//bFound[iVert] = TRUE;
					bFoundThis = TRUE;
					break;
				}
			}
			ASSERT ( bFoundThis == bFound[iVert] );
#endif
		}

		if ( bDontActuallyAdd )
		{
			// Just find what the cost is.
			for ( iVert = 0; iVert < 3; iVert++ )
			{
				if ( !bFound[iVert] )
				{
					fTotalScore += c_ctiCacheTypeSize[iType].fMissScore;
				}
			}
		}
		else
		{
			// Actually add the thing.
			pct->ctti[iType].iIndicesAdded = 0;
			for ( iVert = 0; iVert < 3; iVert++ )
			{
				if ( !bFound[iVert] )
				{
					// Not in the cache. Add it to the FIFO.
					WORD wInd = wIndex[iVert];

					// Store the current tail of the FIFO.
					pct->ctti[iType].wIndicesBinned[pct->ctti[iType].iIndicesAdded] = ccs.pwCacheFifoState[ccs.iCacheFifoWritePos];
					pct->ctti[iType].iIndicesAdded++;

					// This vert just fell off the end.
					WORD wOldInd = ccs.pwCacheFifoState[ccs.iCacheFifoWritePos];
					if ( wOldInd != 0xffff )
					{
						(cvCacheStatus.Item(wOldInd))->bRefcount[iType]--;
						ASSERT ( (cvCacheStatus.Item(wOldInd))->bRefcount[iType] == 0 );
					}


					// Put this index in.
					ccs.pwCacheFifoState[ccs.iCacheFifoWritePos] = wInd;

					// This vert is in.
					(cvCacheStatus.Item(wInd))->bRefcount[iType]++;
					ASSERT ( (cvCacheStatus.Item(wInd))->bRefcount[iType] == 1 );

					// Move the tail on.
					ccs.iCacheFifoWritePos++;
					if ( ccs.iCacheFifoWritePos >= c_ctiCacheTypeSize[iType].iLength )
					{
						ccs.iCacheFifoWritePos = 0;
					}

					// And we missed.
					ccs.iCacheCurrentMisses++;

					// And if this tri is degenerate, we need to spot that we've loaded the others.
					if ( iVert == 0 )
					{
						if ( wIndex[0] == wIndex[1] )
						{
							ASSERT ( !bFound[1] );
							bFound[1] = TRUE;
						}
						if ( wIndex[0] == wIndex[2] )
						{
							ASSERT ( !bFound[2] );
							bFound[2] = TRUE;
						}
					}
					else if ( iVert == 1 )
					{
						if ( wIndex[1] == wIndex[2] )
						{
							ASSERT ( !bFound[2] );
							bFound[2] = TRUE;
						}
					}
				}
				else
				{
					// Found. Do nothing.
				}
			}
			fTotalScore += c_ctiCacheTypeSize[iType].fMissScore * ( ccs.iCacheCurrentMisses - iCurNumMisses );
		}
	}

	return fTotalScore;
}

void CacheRemoveTri ( void )
{
	// Copy the last tri info.
	CacheTri ct = *(ctCacheHistory.Item ( ctCacheHistory.Size() - 1 ));
	// And bin the recorded one.
	ctCacheHistory.RemoveItem ( ctCacheHistory.Size() - 1 );

	// Now undo the tri info.
	for ( int iType = 0; iType < c_iCacheNumTypes; iType++ )
	{
		CurrentCacheState &ccs = ccsState[iType];

		// Unroll the FIFO.
		for ( int i = ct.ctti[iType].iIndicesAdded - 1; i >= 0; i-- )
		{
			// Move the tail back.
			ccs.iCacheFifoWritePos--;
			if ( ccs.iCacheFifoWritePos < 0  )
			{
				ccs.iCacheFifoWritePos = c_ctiCacheTypeSize[iType].iLength - 1;
			}

			// This vert just fell off the start.
			WORD wOldInd = ccs.pwCacheFifoState[ccs.iCacheFifoWritePos];
			(cvCacheStatus.Item(wOldInd))->bRefcount[iType]--;
			ASSERT ( (cvCacheStatus.Item(wOldInd))->bRefcount[iType] == 0 );


			// Stuff this into the last entry in the fifo.
			WORD wInd = ct.ctti[iType].wIndicesBinned[i];
			ccs.pwCacheFifoState[ccs.iCacheFifoWritePos] = wInd;

			// Push this vert into the end.
			if ( wInd != 0xffff )
			{
				(cvCacheStatus.Item(wInd))->bRefcount[iType]++;
				ASSERT ( (cvCacheStatus.Item(wInd))->bRefcount[iType] == 1 );
			}


			// And we un-missed. :-)
			ccs.iCacheCurrentMisses--;
		}
	}
}

// Find the tri that was last added to the cache.
bool CacheGetLastTri ( WORD pwIndices[3] )
{
	if ( ctCacheHistory.Size() > 0 )
	{
		CacheTri ct = *(ctCacheHistory.Item ( ctCacheHistory.Size() - 1 ));
		pwIndices[0] = ct.wIndex[0];
		pwIndices[1] = ct.wIndex[1];
		pwIndices[2] = ct.wIndex[2];
		return TRUE;
	}
	else
	{
		pwIndices[0] = -1;
		pwIndices[1] = -1;
		pwIndices[2] = -1;
		return FALSE;
	}
}


struct ScoreTri
{
	WORD	wIndex[3];
	bool	bUsed;
	bool	bAllowed;					// TRUE if we're even going to consider this tri.
};

struct ScoreVertex
{
	ArbitraryList<ScoreTri*> stri;		// The tris that use this vertex.
	int		iCurrentValence;			// Current unused valence of the vertex.
};



ArbitraryList<ScoreVertex> svVertex;
ArbitraryList<ScoreTri> stTri;
ArbitraryList<ScoreTri*> pstTri;


//ArbitraryList<int> iValenceCounts;			// The (unused) valencies of each vertex.



// Set to 1 to disable valence gubbins from lookaheads.
// Doesn't seem to affect scores much.
#define IGNORE_VALENCE_FOR_LOOKAHEADS 0


// The table of what score increase you get for reducing a valence to a certain amount.
// Any valence over the max value gets the max value boost.
// Bear in mind that three of these get added each tri.
const int c_iMaxValenceBoost = 5;
float fValenceBoost[c_iMaxValenceBoost] = 
{
	0.0f,
	0.68f,
	1.0f,
	1.1f,
	1.15f
};


int LOOKAHEAD = 5;
// Limit the number of tris to lookahead with.
int iLookaheadCutoff = 16;
// Limits how bad tris can be when adding to the heap.
const float fCutoffFactor = 2.0f;
// Another slightly different limit.
const float fExpensiveFactor = 1.5f;


float FindBestScoreLookahead ( int iCountdown, float fCurrentScore, float fInputBestSoFar, 
					 int iNumTris, WORD *pwResult )
{

	ASSERT ( iCountdown <= LOOKAHEAD );

	int iLookahead = iCountdown;
	if ( iLookahead > LOOKAHEAD )
	{
		iLookahead = LOOKAHEAD;
	}


	float fBestSoFar = fInputBestSoFar;

	// Given the BestSoFar score, what is the average cost of each lookahead tri?
	float fAverageCost = ( fBestSoFar - fCurrentScore ) / (int)iLookahead;
	// And now allow tris that are a bit worse.
	float fExpensiveCutoff = fAverageCost * fExpensiveFactor;



	ASSERT ( iCountdown > 0 );
	ASSERT ( iNumTris > 0 );

	int j;



#if 0
	if ( iNumTris == 1 )
	{
		// Well, that's an easy decision then.

		// Find score after removal.
		//float fTriScore = CacheAddTri ( pwIndices[0], pwIndices[1], pwIndices[2], TRUE );
		ScoreTri *pstCur = ppstCurTris[0];
		float fTriScore = CacheAddTri ( pstCur->wIndex[0], pstCur->wIndex[1], pstCur->wIndex[2], TRUE );
		for ( j = 0; j < 3; j++ )
		{
			// Use the valence score after the tri has been removed.
			//int iValence = (*(iValenceCounts.Item(pwIndices[j]))) - 1;
			int iValence = (svVertex.Item(pstCur->wIndex[j])->iCurrentValence) - 1;
			ASSERT ( iValence >= 0 );
			if ( iValence < c_iMaxValenceBoost )
			{
				fTriScore += fValenceBoost[iValence];
			}
			else
			{
				fTriScore += fValenceBoost[c_iMaxValenceBoost-1];
			}
		}

		fTriScore += fCurrentScore;

		//pwResult[0] = pwIndices[0];
		//pwResult[1] = pwIndices[1];
		//pwResult[2] = pwIndices[2];
		pwResult[0] = pstCur->wIndex[0];
		pwResult[1] = pstCur->wIndex[1];
		pwResult[2] = pstCur->wIndex[2];

		if ( fTriScore > fBestSoFar )
		{
			// Oh well.
			// (actually, not sure this ever happens).
			return fInputBestSoFar;
		}
		else
		{
			return fTriScore;
		}
	}
#endif



#if 0
#ifdef _DEBUG
	for ( int k = 0; k < iNumTris; k++ )
	{
		ASSERT ( !ppstCurTris[k]->bAllowed );
		ASSERT ( !ppstCurTris[k]->bUsed );
	}
#endif
#endif


	// Should we limit ourselves to tris that share at least one vert with the previous one?
	bool bNoMadJumps = FALSE;
	WORD wPrevIndices[3];
	//if ( ( fBestSoFar - fCurrentScore ) < (float)iCountdown )
		// The lookahead score is lower than the countdown, so doing mad jumps
		// is not going to do sensible things.


	ArbitraryList<ScoreTri*> pstTriLocal;

	//if ( iCountdown == iLookahead )
	{
		// Ah - this is probably a lookahead, not assembling the real list.
		// Find the previous indices from the cache (a bit of a roundabout method).
		int iNumAllowed = 0;
		BOOL bRes = CacheGetLastTri ( wPrevIndices );
		ASSERT ( bRes );
		{
			bNoMadJumps = TRUE;
			// And mark all the tris that these used as "allowed"
			for ( int i = 0; i < 3; i++ )
			{
				ScoreVertex *psv = svVertex.Item(wPrevIndices[i]);
				for ( int j = 0; j < psv->stri.Size(); j++ )
				{
					ScoreTri *pst = (*(psv->stri.Item(j)));
					if ( ( !pst->bUsed ) && ( !pst->bAllowed ) )
					{
						pst->bAllowed = TRUE;
						*(pstTriLocal.AddItem()) = pst;
						iNumAllowed++;
					}
				}
			}

			if ( iNumAllowed < 1 )
			{
				// Oops - we've gone into a dead-end, so that very few tris
				// are allowed. So open the selection up again.
				pstTriLocal.SizeTo(0);
				bNoMadJumps = FALSE;
				int i;
				for ( i = 0; i < 3; i++ )
				{
					ScoreVertex *psv = svVertex.Item(wPrevIndices[i]);
					for ( int j = 0; j < psv->stri.Size(); j++ )
					{
						ScoreTri *pst = (*(psv->stri.Item(j)));
						if ( !pst->bUsed )
						{
							pst->bAllowed = FALSE;
						}
					}
				}

				for ( i = 0; i < stTri.Size(); i++ )
				{
					if ( !(stTri.Item(i))->bUsed )
					{
						*(pstTriLocal.AddItem()) = stTri.Item(i);
					}
				}
			}
		}
	}


	// Add the tris to a new priority queue, sorting by score.
	//BinaryHeap<WORD, float> NewHeap;
	BinaryHeap<ScoreTri, float> NewHeap;

	int i;
	//WORD *pwCurIndex = pwIndices;
	for ( i = pstTriLocal.Size() - 1; i >= 0; i-- )
	{
		ScoreTri *pstCurTri = *(pstTriLocal.Item(i));
		float fTriScore = 0.0f;
		if ( bNoMadJumps )
		{
			if ( !pstCurTri->bAllowed )
			{
				continue;
			}

#if IGNORE_VALENCE_FOR_LOOKAHEADS
			// Only use the valence stuff on non-lookaheads.
			// To make the scores comparable, add the maximum valence boost all the time.
			fTriScore += 3.0f * fValenceBoost[c_iMaxValenceBoost-1];
#endif
		}
#if IGNORE_VALENCE_FOR_LOOKAHEADS
		else
#endif
		{
			// Only use the valence stuff on non-lookaheads.
			for ( j = 0; j < 3; j++ )
			{
				// Use the valence score after the tri has been removed.
				//int iValence = (*(iValenceCounts.Item(pwCurIndex[j]))) - 1;
				int iValence = (svVertex.Item(pstCurTri->wIndex[j])->iCurrentValence) - 1;
				ASSERT ( iValence >= 0 );
				if ( iValence < c_iMaxValenceBoost )
				{
					fTriScore += fValenceBoost[iValence];
				}
				else
				{
					fTriScore += fValenceBoost[c_iMaxValenceBoost-1];
				}
			}
		}

		if ( fTriScore > fExpensiveCutoff )
		{
			// This tri is a lot more expensive than the average cost of the BestSoFar tris.
			// It's very unlikely to give us a good result.
			continue;
		}

		if ( fCurrentScore + fTriScore >= fBestSoFar )
		{
			// We've already gone more than the best score.
			//pwCurIndex += 3;
			continue;
		}


		// And the vertex cost.
		fTriScore += CacheAddTri ( pstCurTri->wIndex[0], pstCurTri->wIndex[1], pstCurTri->wIndex[2], TRUE );

		if ( fTriScore > fExpensiveCutoff )
		{
			// This tri is a lot more expensive than the average cost of the BestSoFar tris.
			// It's very unlikely to give us a good result.
			continue;
		}

		if ( fCurrentScore + fTriScore >= fBestSoFar )
		{
			// We've already gone more than the best score.
			//pwCurIndex += 3;
			continue;
		}

		// And bung it in the heap.
		// We -ve the score so that the first in the heap is the one with the _least_ score.
		NewHeap.Add ( pstCurTri, -fTriScore );
		//pwCurIndex += 3;
	}



	// Undo the "allowed" flags.
	if ( bNoMadJumps )
	{
		for ( int i = 0; i < 3; i++ )
		{
			ScoreVertex *psv = svVertex.Item(wPrevIndices[i]);
			for ( int j = 0; j < psv->stri.Size(); j++ )
			{
				(*(psv->stri.Item(j)))->bAllowed = FALSE;
			}
		}
	}


	// Now extract from the heap, best score to worst.
	// This attempts to get early-outs very quickly and prevent too much recursion.
	//WORD *pwBest = NULL;
	//WORD *pwCur = NewHeap.FindFirst();
	ScoreTri *pstBest = NULL;
	ScoreTri *pstCur = NewHeap.FindFirst();

	//if ( pwCur == NULL )
	if ( pstCur == NULL )
	{
		// Found nothing that was better.
		return fBestSoFar;
	}

	// Above this score, just don't bother.
	float fCutoffScore = fCutoffFactor * NewHeap.GetCurrentSort();

#ifdef _DEBUG
	float fPrevScore = 1.0f;
#endif

	int iTried = 0;
	//while ( pwCur != NULL )
	while ( pstCur != NULL )
	{
		float fThisTriNegScore = NewHeap.GetCurrentSort();
		NewHeap.RemoveFirst();

#ifdef _DEBUG
		// Check this heap actually works!
		ASSERT ( fThisTriNegScore <= fPrevScore );
		fPrevScore = fThisTriNegScore;
#endif


		if ( fThisTriNegScore < fCutoffScore )
		{
			// Reached the cutoff for this tri - don't bother continuing.
			break;
		}

		float fScore = fCurrentScore - fThisTriNegScore;

		if ( fScore >= fBestSoFar )
		{
			// We've already gone more than the best score.
			// The reast of the heap is bound to be greater, so don't bother continuing.
			break;
		}

		if ( bNoMadJumps )
		{
			iTried++;
			if ( iTried > iLookaheadCutoff )
			{
				// Tried enough tris - don't want to cause a combinatorial explosion.
				break;
			}
		}

		// Do the valencies.
#ifdef _DEBUG
		float fValenceScore = 0.0f;
#endif
#if IGNORE_VALENCE_FOR_LOOKAHEADS
		if ( bNoMadJumps )
		{
			// Only use the valence stuff on non-lookaheads.
			// To make the scores comparable, add the maximum valence boost all the time.
			fValenceScore = 3.0f * fValenceBoost[c_iMaxValenceBoost-1];
		}
		else
#endif
		{
			for ( j = 0; j < 3; j++ )
			{
				//int iValence = --(*(iValenceCounts.Item(pwCur[j])));
				int iValence = --(svVertex.Item(pstCur->wIndex[j])->iCurrentValence);
	#ifdef _DEBUG
				ASSERT ( iValence >= 0 );
				if ( iValence < c_iMaxValenceBoost )
				{
					fValenceScore += fValenceBoost[iValence];
				}
				else
				{
					fValenceScore += fValenceBoost[c_iMaxValenceBoost-1];
				}
	#endif
			}
		}

		// Add it to the cache.
		float fScoreTemp = CacheAddTri ( pstCur->wIndex[0], pstCur->wIndex[1], pstCur->wIndex[2] );

		ASSERT ( !pstCur->bUsed );
		pstCur->bUsed = TRUE;


#ifdef _DEBUG
		fScoreTemp += fValenceScore;
		ASSERT ( fabs ( fScoreTemp + fThisTriNegScore ) < 0.0001f );
#endif

		if ( iLookahead > 1 )
		{
			// Swap pwCur to the start of the list.
			// And look ahead a bit more.
			//fScore = FindBestScoreLookahead ( iLookahead - 1, fScore, fBestSoFar, pwIndices + 3, iNumTris - 1, pwResult + 3 );
			float fNewScore = FindBestScoreLookahead ( iLookahead - 1, fScore, fBestSoFar, iNumTris - 1, pwResult + 3 );
			//ASSERT ( fNewScore < 1e9 );
			fScore = fNewScore;
		}
		//ASSERT ( fScore < 1e9 );
		if ( fScore < fBestSoFar )
		{
			fBestSoFar = fScore;
			//pwBest = pwCur;
			pstBest = pstCur;
		}

		CacheRemoveTri();
		ASSERT ( pstCur->bUsed );
		pstCur->bUsed = FALSE;

#if IGNORE_VALENCE_FOR_LOOKAHEADS
		if ( !bNoMadJumps )
#endif
		{
			// Restore the valencies.
			for ( j = 0; j < 3; j++ )
			{
				//++(*(iValenceCounts.Item(pwCur[j])));
				++(svVertex.Item(pstCur->wIndex[j])->iCurrentValence);
			}
		}

		//pwCur = NewHeap.FindFirst();
		pstCur = NewHeap.FindFirst();
	}


	//if ( pwBest == NULL )
	if ( pstBest == NULL )
	{
		// We didn't find a solution that was better than the existing one.
		//ASSERT ( fBestSoFar < 1e9 );
		return fInputBestSoFar;
	}
	else
	{
		// Found a better solution.


		// Swap the best tri to the start of the list.
		float fNewBestScore = fBestSoFar;

		// Dump the tri to the result buffer.
		//ScoreTri *pstBest = *ppstBest;
		//pwResult[0] = pwBest[0];
		//pwResult[1] = pwBest[1];
		//pwResult[2] = pwBest[2];
		pwResult[0] = pstBest->wIndex[0];
		pwResult[1] = pstBest->wIndex[1];
		pwResult[2] = pstBest->wIndex[2];
		if ( iCountdown > 1 )
		{
			//float fScore = CacheAddTri ( wTemp[0], wTemp[1], wTemp[2] );
			float fScore = CacheAddTri ( pstBest->wIndex[0], pstBest->wIndex[1], pstBest->wIndex[2] );
			ASSERT ( !pstBest->bUsed );
			pstBest->bUsed = TRUE;

			// And the valence.
			float fValenceScore = 0.0f;
#if IGNORE_VALENCE_FOR_LOOKAHEADS
			//if ( !bNoMadJumps )
#endif
			{
				for ( j = 0; j < 3; j++ )
				{
					//int iValence = --(*(iValenceCounts.Item(wTemp[j])));
					int iValence = --(svVertex.Item(pstBest->wIndex[j])->iCurrentValence);
					ASSERT ( iValence >= 0 );
					if ( iValence < c_iMaxValenceBoost )
					{
						fValenceScore += fValenceBoost[iValence];
					}
					else
					{
						fValenceScore += fValenceBoost[c_iMaxValenceBoost-1];
					}
				}
			}

			fScore += fValenceScore + fCurrentScore;

#ifdef DEBUG
			// Keep track of our progress.
			if ( iCountdown > LOOKAHEAD )
			{
				// Ah - we must be assembling the real list.
				TRACE ( "Countdown %i\n", iCountdown );
			}
#endif


			//ASSERT ( fScore < 1e9 );

			//fNewBestScore = FindBestScoreLookahead ( iCountdown - 1, fScore, 1e10, pwIndices + 3, iNumTris - 1, pwResult + 3 );
			fNewBestScore = FindBestScoreLookahead ( iCountdown - 1, fScore, 1e10, iNumTris - 1, pwResult + 3 );

			//ASSERT ( fNewBestScore < 1e9 );

			CacheRemoveTri();
			ASSERT ( pstBest->bUsed );
			pstBest->bUsed = FALSE;

			// Restore the valencies.
#if IGNORE_VALENCE_FOR_LOOKAHEADS
			//if ( !bNoMadJumps )
#endif
			{
				for ( j = 0; j < 3; j++ )
				{
					//++(*(iValenceCounts.Item(wTemp[j])));
					++(svVertex.Item(pstBest->wIndex[j])->iCurrentValence);
				}
			}
		}

		return fNewBestScore;
	}
}

float FindBestScore ( int iCountdown, float fCurrentScore, float fInputBestSoFar, 
					 //WORD *pwIndices,
					 ScoreTri **ppstCurTris,		// Pointer to a list of pointers to ScoreTris
					 int iNumTris, WORD *pwResult )
{
	int iLookahead = iCountdown;
	if ( iLookahead > LOOKAHEAD )
	{
		iLookahead = LOOKAHEAD;
	}

	//ASSERT ( fCurrentScore < 1e9 );


	// At the start, limit the lookahead, or it takes ages.
	if ( ( iNumTris > 100 ) && ( fCurrentScore < 50.0f ) && ( iLookahead > 2 ) )
	{
		iLookahead = 2;
	}


	float fBestSoFar = fInputBestSoFar;

	// Given the BestSoFar score, what is the average cost of each lookahead tri?
	float fAverageCost = ( fBestSoFar - fCurrentScore ) / (int)iLookahead;
	// And now allow tris that are a bit worse.
	float fExpensiveCutoff = fAverageCost * fExpensiveFactor;



	ASSERT ( iCountdown > 0 );
	ASSERT ( iNumTris > 0 );

	int j;



	if ( iNumTris == 1 )
	{
		// Well, that's an easy decision then.

		// Find score after removal.
		//float fTriScore = CacheAddTri ( pwIndices[0], pwIndices[1], pwIndices[2], TRUE );
		ScoreTri *pstCur = ppstCurTris[0];
		float fTriScore = CacheAddTri ( pstCur->wIndex[0], pstCur->wIndex[1], pstCur->wIndex[2], TRUE );
		for ( j = 0; j < 3; j++ )
		{
			// Use the valence score after the tri has been removed.
			//int iValence = (*(iValenceCounts.Item(pwIndices[j]))) - 1;
			int iValence = (svVertex.Item(pstCur->wIndex[j])->iCurrentValence) - 1;
			ASSERT ( iValence >= 0 );
			if ( iValence < c_iMaxValenceBoost )
			{
				fTriScore += fValenceBoost[iValence];
			}
			else
			{
				fTriScore += fValenceBoost[c_iMaxValenceBoost-1];
			}
		}

		fTriScore += fCurrentScore;

		//pwResult[0] = pwIndices[0];
		//pwResult[1] = pwIndices[1];
		//pwResult[2] = pwIndices[2];
		pwResult[0] = pstCur->wIndex[0];
		pwResult[1] = pstCur->wIndex[1];
		pwResult[2] = pstCur->wIndex[2];

		if ( fTriScore > fBestSoFar )
		{
			// Oh well.
			// (actually, not sure this ever happens).
			return fInputBestSoFar;
		}
		else
		{
			return fTriScore;
		}
	}



#ifdef _DEBUG
	for ( int k = 0; k < iNumTris; k++ )
	{
		ASSERT ( !ppstCurTris[k]->bAllowed );
		ASSERT ( !ppstCurTris[k]->bUsed );
	}
#endif


	// Should we limit ourselves to tris that share at least one vert with the previous one?
	bool bNoMadJumps = FALSE;
	WORD wPrevIndices[3];
	//if ( ( fBestSoFar - fCurrentScore ) < (float)iCountdown )
		// The lookahead score is lower than the countdown, so doing mad jumps
		// is not going to do sensible things.
	if ( iCountdown == iLookahead )
	{
		// Ah - this is probably a lookahead, not assembling the real list.
		// Find the previous indices from the cache (a bit of a roundabout method).
		int iNumAllowed = 0;
		if ( CacheGetLastTri ( wPrevIndices ) )
		{
			bNoMadJumps = TRUE;
			// And mark all the tris that these used as "allowed"
			for ( int i = 0; i < 3; i++ )
			{
				ScoreVertex *psv = svVertex.Item(wPrevIndices[i]);
				for ( int j = 0; j < psv->stri.Size(); j++ )
				{
					ScoreTri *pst = (*(psv->stri.Item(j)));
					if ( ( !pst->bUsed ) && ( !pst->bAllowed ) )
					{
						pst->bAllowed = TRUE;
						iNumAllowed++;
					}
				}
			}
			if ( iNumAllowed < 1 )
			{
				// Oops - we've probably gone into a dead-end, so that very few tris
				// are allowed. So open the selection up again.
				bNoMadJumps = FALSE;
				for ( int i = 0; i < 3; i++ )
				{
					ScoreVertex *psv = svVertex.Item(wPrevIndices[i]);
					for ( int j = 0; j < psv->stri.Size(); j++ )
					{
						ScoreTri *pst = (*(psv->stri.Item(j)));
						if ( !pst->bUsed )
						{
							pst->bAllowed = FALSE;
						}
					}
				}
			}
		}
	}


	// Add the tris to a new priority queue, sorting by score.
	//BinaryHeap<WORD, float> NewHeap;
	BinaryHeap<ScoreTri*, float> NewHeap;

	int i;
	//WORD *pwCurIndex = pwIndices;
	for ( i = 0; i < iNumTris; i++ )
	{
		ScoreTri *pstCurTri = ppstCurTris[i];
		float fTriScore = 0.0f;
		if ( bNoMadJumps )
		{
			if ( !pstCurTri->bAllowed )
			{
				continue;
			}

#if IGNORE_VALENCE_FOR_LOOKAHEADS
			// Only use the valence stuff on non-lookaheads.
			// To make the scores comparable, add the maximum valence boost all the time.
			fTriScore += 3.0f * fValenceBoost[c_iMaxValenceBoost-1];
#endif
		}
#if IGNORE_VALENCE_FOR_LOOKAHEADS
		else
#endif
		{
			// Only use the valence stuff on non-lookaheads.
			for ( j = 0; j < 3; j++ )
			{
				// Use the valence score after the tri has been removed.
				//int iValence = (*(iValenceCounts.Item(pwCurIndex[j]))) - 1;
				int iValence = (svVertex.Item(pstCurTri->wIndex[j])->iCurrentValence) - 1;
				ASSERT ( iValence >= 0 );
				if ( iValence < c_iMaxValenceBoost )
				{
					fTriScore += fValenceBoost[iValence];
				}
				else
				{
					fTriScore += fValenceBoost[c_iMaxValenceBoost-1];
				}
			}
		}

		if ( fTriScore > fExpensiveCutoff )
		{
			// This tri is a lot more expensive than the average cost of the BestSoFar tris.
			// It's very unlikely to give us a good result.
			continue;
		}

		if ( fCurrentScore + fTriScore >= fBestSoFar )
		{
			// We've already gone more than the best score.
			//pwCurIndex += 3;
			continue;
		}


		// And the vertex cost.
		fTriScore += CacheAddTri ( pstCurTri->wIndex[0], pstCurTri->wIndex[1], pstCurTri->wIndex[2], TRUE );

		if ( fTriScore > fExpensiveCutoff )
		{
			// This tri is a lot more expensive than the average cost of the BestSoFar tris.
			// It's very unlikely to give us a good result.
			continue;
		}

		if ( fCurrentScore + fTriScore >= fBestSoFar )
		{
			// We've already gone more than the best score.
			//pwCurIndex += 3;
			continue;
		}

		// And bung it in the heap.
		// We -ve the score so that the first in the heap is the one with the _least_ score.
		NewHeap.Add ( &(ppstCurTris[i]), -fTriScore );
		//pwCurIndex += 3;
	}



	// Undo the "allowed" flags.
	if ( bNoMadJumps )
	{
		for ( int i = 0; i < 3; i++ )
		{
			ScoreVertex *psv = svVertex.Item(wPrevIndices[i]);
			for ( int j = 0; j < psv->stri.Size(); j++ )
			{
				(*(psv->stri.Item(j)))->bAllowed = FALSE;
			}
		}
	}

#ifdef _DEBUG
	for ( k = 0; k < iNumTris; k++ )
	{
		ASSERT ( !ppstCurTris[k]->bAllowed );
		ASSERT ( !ppstCurTris[k]->bUsed );
	}
#endif



	// Now extract from the heap, best score to worst.
	// This attempts to get early-outs very quickly and prevent too much recursion.
	//WORD *pwBest = NULL;
	//WORD *pwCur = NewHeap.FindFirst();
	ScoreTri **ppstBest = NULL;
	ScoreTri **ppstCur = NewHeap.FindFirst();

	//if ( pwCur == NULL )
	if ( ppstCur == NULL )
	{
		// Found nothing that was better.
		return fBestSoFar;
	}

	// Above this score, just don't bother.
	float fCutoffScore = fCutoffFactor * NewHeap.GetCurrentSort();

#ifdef _DEBUG
	float fPrevScore = 1.0f;
#endif

	int iTried = 0;
	//while ( pwCur != NULL )
	while ( ppstCur != NULL )
	{
		ScoreTri *pstCur = *ppstCur;
	
		float fThisTriNegScore = NewHeap.GetCurrentSort();
		NewHeap.RemoveFirst();

#ifdef _DEBUG
		// Check this heap actually works!
		ASSERT ( fThisTriNegScore <= fPrevScore );
		fPrevScore = fThisTriNegScore;
#endif


		if ( fThisTriNegScore < fCutoffScore )
		{
			// Reached the cutoff for this tri - don't bother continuing.
			break;
		}

		float fScore = fCurrentScore - fThisTriNegScore;

		if ( fScore >= fBestSoFar )
		{
			// We've already gone more than the best score.
			// The reast of the heap is bound to be greater, so don't bother continuing.
			break;
		}

		if ( bNoMadJumps )
		{
			iTried++;
			if ( iTried > iLookaheadCutoff )
			{
				// Tried enough tris - don't want to cause a combinatorial explosion.
				break;
			}
		}

		// Do the valencies.
#ifdef _DEBUG
		float fValenceScore = 0.0f;
#endif
#if IGNORE_VALENCE_FOR_LOOKAHEADS
		if ( bNoMadJumps )
		{
			// Only use the valence stuff on non-lookaheads.
			// To make the scores comparable, add the maximum valence boost all the time.
			fValenceScore = 3.0f * fValenceBoost[c_iMaxValenceBoost-1];
		}
		else
#endif
		{
			for ( j = 0; j < 3; j++ )
			{
				//int iValence = --(*(iValenceCounts.Item(pwCur[j])));
				int iValence = --(svVertex.Item(pstCur->wIndex[j])->iCurrentValence);
	#ifdef _DEBUG
				ASSERT ( iValence >= 0 );
				if ( iValence < c_iMaxValenceBoost )
				{
					fValenceScore += fValenceBoost[iValence];
				}
				else
				{
					fValenceScore += fValenceBoost[c_iMaxValenceBoost-1];
				}
	#endif
			}
		}

		// Add it to the cache.
		float fScoreTemp = CacheAddTri ( pstCur->wIndex[0], pstCur->wIndex[1], pstCur->wIndex[2] );

		ASSERT ( !pstCur->bUsed );
		pstCur->bUsed = TRUE;


#ifdef _DEBUG
		fScoreTemp += fValenceScore;
		ASSERT ( fabs ( fScoreTemp + fThisTriNegScore ) < 0.0001f );
#endif

		if ( iLookahead > 1 )
		{
			// Swap pwCur to the start of the list.
#if 0
			WORD wTemp0 = pwCur[0];
			WORD wTemp1 = pwCur[1];
			WORD wTemp2 = pwCur[2];
			pwCur[0] = pwIndices[0];
			pwCur[1] = pwIndices[1];
			pwCur[2] = pwIndices[2];
			//pwIndices[0] = wTemp0;
			//pwIndices[1] = wTemp1;
			//pwIndices[2] = wTemp2;
#else
			ScoreTri *pstTemp = *ppstCur;
			*ppstCur = *ppstCurTris;
			//*ppstCurTris = pstTemp;
#endif

			//ASSERT ( fScore < 1e9 );

			// And look ahead a bit more.
			//fScore = FindBestScoreLookahead ( iLookahead - 1, fScore, fBestSoFar, pwIndices + 3, iNumTris - 1, pwResult + 3 );
			float fNewScore = FindBestScoreLookahead ( iLookahead - 1, fScore, fBestSoFar, iNumTris - 1, pwResult + 3 );
			//ASSERT ( fNewScore < 1e9 );
			fScore = fNewScore;

#if 0
			// And swap it back.
			//wTemp0 = pwIndices[0];
			//wTemp1 = pwIndices[1];
			//wTemp2 = pwIndices[2];
			pwIndices[0] = pwCur[0];
			pwIndices[1] = pwCur[1];
			pwIndices[2] = pwCur[2];
			pwCur[0] = wTemp0;
			pwCur[1] = wTemp1;
			pwCur[2] = wTemp2;
#else
			//pstTemp = *ppstCurTris;
			*ppstCurTris = *ppstCur;
			*ppstCur = pstTemp;
#endif
		}
		//ASSERT ( fScore < 1e9 );
		if ( fScore < fBestSoFar )
		{
			fBestSoFar = fScore;
			//pwBest = pwCur;
			ppstBest = ppstCur;
		}

		CacheRemoveTri();
		ASSERT ( pstCur->bUsed );
		pstCur->bUsed = FALSE;

#if IGNORE_VALENCE_FOR_LOOKAHEADS
		if ( !bNoMadJumps )
#endif
		{
			// Restore the valencies.
			for ( j = 0; j < 3; j++ )
			{
				//++(*(iValenceCounts.Item(pwCur[j])));
				++(svVertex.Item(pstCur->wIndex[j])->iCurrentValence);
			}
		}

		//pwCur = NewHeap.FindFirst();
		ppstCur = NewHeap.FindFirst();
	}


	ASSERT ( ppstBest != NULL );
	{
		// Found a better solution.


		// Swap the best tri to the start of the list.
		float fNewBestScore = fBestSoFar;

		// Dump the tri to the result buffer.
		ScoreTri *pstBest = *ppstBest;
		//pwResult[0] = pwBest[0];
		//pwResult[1] = pwBest[1];
		//pwResult[2] = pwBest[2];
		pwResult[0] = pstBest->wIndex[0];
		pwResult[1] = pstBest->wIndex[1];
		pwResult[2] = pstBest->wIndex[2];
		if ( iCountdown > 1 )
		{
#if 0
			WORD wTemp[3];
			wTemp[0] = pwBest[0];
			wTemp[1] = pwBest[1];
			wTemp[2] = pwBest[2];
			pwBest[0] = pwIndices[0];
			pwBest[1] = pwIndices[1];
			pwBest[2] = pwIndices[2];		
#else
			ScoreTri *pTemp;
			pTemp = *ppstBest;
			*ppstBest = *ppstCurTris;
#endif

			//float fScore = CacheAddTri ( wTemp[0], wTemp[1], wTemp[2] );
			float fScore = CacheAddTri ( pstBest->wIndex[0], pstBest->wIndex[1], pstBest->wIndex[2] );
			ASSERT ( !pstBest->bUsed );
			pstBest->bUsed = TRUE;

			// And the valence.
			float fValenceScore = 0.0f;
#if IGNORE_VALENCE_FOR_LOOKAHEADS
			//if ( !bNoMadJumps )
#endif
			{
				for ( j = 0; j < 3; j++ )
				{
					//int iValence = --(*(iValenceCounts.Item(wTemp[j])));
					int iValence = --(svVertex.Item(pstBest->wIndex[j])->iCurrentValence);
					ASSERT ( iValence >= 0 );
					if ( iValence < c_iMaxValenceBoost )
					{
						fValenceScore += fValenceBoost[iValence];
					}
					else
					{
						fValenceScore += fValenceBoost[c_iMaxValenceBoost-1];
					}
				}
			}

			fScore += fValenceScore + fCurrentScore;

#ifdef DEBUG
			TRACE ( "Countdown %i\n", iCountdown );
#endif

			//fNewBestScore = FindBestScore ( iCountdown - 1, fScore, 1e10, pwIndices + 3, iNumTris - 1, pwResult + 3 );
			fNewBestScore = FindBestScore ( iCountdown - 1, fScore, 1e10, ppstCurTris + 1, iNumTris - 1, pwResult + 3 );

			//ASSERT ( fNewBestScore < 1e9 );

			CacheRemoveTri();
			ASSERT ( pstBest->bUsed );
			pstBest->bUsed = FALSE;

			// Restore the valencies.
#if IGNORE_VALENCE_FOR_LOOKAHEADS
			//if ( !bNoMadJumps )
#endif
			{
				for ( j = 0; j < 3; j++ )
				{
					//++(*(iValenceCounts.Item(wTemp[j])));
					++(svVertex.Item(pstBest->wIndex[j])->iCurrentValence);
				}
			}

			// And swap it back.
#if 0
			pwIndices[0] = pwBest[0];
			pwIndices[1] = pwBest[1];
			pwIndices[2] = pwBest[2];
			pwBest[0] = wTemp[0];
			pwBest[1] = wTemp[1];
			pwBest[2] = wTemp[2];
#else
			*ppstCurTris = *ppstBest;
			*ppstBest = pTemp;
#endif
		}

		return fNewBestScore;
	}
}

// Call this to reorder the tris in this trilist to get good vertex-cache coherency.
// *pwList is modified (but obviously not changed in size or memory location).
void OptimiseVertexCoherencyTriList ( WORD *pwList, int iHowManyTris )
{
	if ( iHowManyTris <= 2 )
	{
		// Silly.
		return;
	}

	DWORD	tm_Start	= timeGetTime	();
	if ( g_bUseFastButBadOptimise )
	{
		// Very cheap'n'cheerful.
		LOOKAHEAD = 1;
		iLookaheadCutoff = 4;
	}
	else
	{
		// Expensive, but better.
		LOOKAHEAD = 5;
		iLookaheadCutoff = 16;
	}


#ifdef _DEBUG
	// Find current score (probably rubbish).
	float fCurrentScore = GetNumVerticesLoadedTriList ( pwList, iHowManyTris );
#endif

	// First scan to find the biggest index.
	WORD *pwIndex = pwList;
	int iBiggestIndex = 0;
	int i;
	for ( i = 0; i < iHowManyTris * 3; i++ )
	{
		if ( iBiggestIndex < *pwIndex )
		{
			iBiggestIndex = *pwIndex;
		}
		pwIndex++;
	}

#if 0
	iValenceCounts.SizeTo(iBiggestIndex+1);
	for ( i = 0; i <= iBiggestIndex; i++ )
	{
		*(iValenceCounts.Item(i)) = 0;
	}


	// And set up the valencies.
	pwIndex = pwList;
	for ( i = 0; i < iHowManyTris * 3; i++ )
	{
		++(*(iValenceCounts.Item(*pwIndex)));
		pwIndex++;
	}
#else

	svVertex.SizeTo ( iBiggestIndex + 1 );
	for ( i = 0; i <= iBiggestIndex; i++ )
	{
		ScoreVertex *sv = svVertex.Item(i);
		sv->iCurrentValence = 0;
		sv->stri.SizeTo(0);
	}

	stTri.SizeTo ( iHowManyTris );
	pstTri.SizeTo ( iHowManyTris );
	WORD *pwCurTri = pwList;
	for ( i = 0; i < iHowManyTris; i++ )
	{
		ScoreTri *pstCur = stTri.Item(i);
		*(pstTri.Item(i)) = pstCur;
		pstCur->bUsed = FALSE;
		pstCur->bAllowed = FALSE;
		for ( int j = 0; j < 3; j++ )
		{
			pstCur->wIndex[j] = pwCurTri[j];
			svVertex.Item(pwCurTri[j])->iCurrentValence++;
			*(svVertex.Item(pwCurTri[j])->stri.AddItem()) = pstCur;
		}
		pwCurTri += 3;
	}

#endif

	WORD *pwNewIndex = new WORD[iHowManyTris*3];

	CacheInit ( iBiggestIndex + 1 );


#if 0
	// If you try to optimise from the very first tri, it takes ages to find the "best"
	// first tri - almost none of the early-outs work, because there is no cache history,
	// and indeed the whole concept of a "best" first tri is madness - you have to start
	// somewhere.
	// So I just pick the first one, and let it carry on from there. Once the cache has
	// been "seeded" the early-outs come thick and fast and it's a decent speed.
	pwNewIndex[0] = pwList[0];
	pwNewIndex[1] = pwList[1];
	pwNewIndex[2] = pwList[2];
	--(*(iValenceCounts.Item(pwList[0])));
	--(*(iValenceCounts.Item(pwList[1])));
	--(*(iValenceCounts.Item(pwList[2])));
	CacheAddTri ( pwList[0], pwList[1], pwList[2] );
	// No need to actually increase the score - everything is relative anyway.

	float fTemp = FindBestScore (	iHowManyTris - 1,
									0.0f,
									1e10,
									pwList + 3,
									iHowManyTris - 1,
									pwNewIndex + 3 );
#else
	float fTemp = FindBestScore (	iHowManyTris,
									0.0f,
									1e10,
									(pstTri.Item(0)),
									iHowManyTris,
									pwNewIndex );
#endif

	CacheBin();

#if 0
	iValenceCounts.SizeTo(0);
#else
	svVertex.SizeTo(0);
	stTri.SizeTo(0);
#endif


#ifdef _DEBUG
	// Find new score (hopefully better).
	float fNewScore = GetNumVerticesLoadedTriList ( pwNewIndex, iHowManyTris );
	fNewScore /= (float)iHowManyTris;		// The verts per tri score.
	// And if you definately use indices 0 to iBiggestIndex (which all this code does), then this is the max possible:
	float fMaxPossible = iBiggestIndex / (float)iHowManyTris;
#endif


	memcpy ( pwList, pwNewIndex, sizeof(WORD) * 3 * iHowManyTris );

	delete[] pwNewIndex;

	/*
	DWORD	tm_End	= timeGetTime	();
	DWORD	tm_		= tm_End - tm_Start;
	char	aaa		[256];
	sprintf	(aaa,"%d\n",tm_);
	OutputDebugString(aaa);
	*/
}
