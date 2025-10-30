#include "stdafx.h"
#include "Mesh.h"
#include "GraphicsPipeline.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CPolygon::CPolygon(int nVertices)
{
	m_nVertices = nVertices;
	m_pVertices = new CVertex[nVertices];
}

CPolygon::~CPolygon()
{
	if (m_pVertices) delete[] m_pVertices;
}

void CPolygon::SetVertex(int nIndex, CVertex& vertex)
{
	if ((0 <= nIndex) && (nIndex < m_nVertices) && m_pVertices)
	{
		m_pVertices[nIndex] = vertex;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMesh::CMesh(int nPolygons)
{
	m_nPolygons = nPolygons;
	m_ppPolygons = new CPolygon * [nPolygons];
}

CMesh::~CMesh()
{
	if (m_ppPolygons)
	{
		for (int i = 0; i < m_nPolygons; i++) if (m_ppPolygons[i]) delete m_ppPolygons[i];
		delete[] m_ppPolygons;
	}
}

void CMesh::SetPolygon(int nIndex, CPolygon* pPolygon)
{
	if ((0 <= nIndex) && (nIndex < m_nPolygons)) m_ppPolygons[nIndex] = pPolygon;
}

void Draw2DLine(HDC hDCFrameBuffer, XMFLOAT3& f3PreviousProject, XMFLOAT3& f3CurrentProject)
{
	XMFLOAT3 f3Previous = CGraphicsPipeline::ScreenTransform(f3PreviousProject);
	XMFLOAT3 f3Current = CGraphicsPipeline::ScreenTransform(f3CurrentProject);
	::MoveToEx(hDCFrameBuffer, (long)f3Previous.x, (long)f3Previous.y, NULL);
	::LineTo(hDCFrameBuffer, (long)f3Current.x, (long)f3Current.y);
}

void CMesh::Render(HDC hDCFrameBuffer)
{
	XMFLOAT3 f3InitialProject, f3PreviousProject;
	bool bPreviousInside = false, bInitialInside = false, bCurrentInside = false, bIntersectInside = false;

	for (int j = 0; j < m_nPolygons; j++)
	{
		int nVertices = m_ppPolygons[j]->m_nVertices;
		CVertex* pVertices = m_ppPolygons[j]->m_pVertices;

		f3PreviousProject = f3InitialProject = CGraphicsPipeline::Project(pVertices[0].m_xmf3Position);
		bPreviousInside = bInitialInside = (-1.0f <= f3InitialProject.x) && (f3InitialProject.x <= 1.0f) && (-1.0f <= f3InitialProject.y) && (f3InitialProject.y <= 1.0f);
		for (int i = 1; i < nVertices; i++)
		{
			XMFLOAT3 f3CurrentProject = CGraphicsPipeline::Project(pVertices[i].m_xmf3Position);
			bCurrentInside = (-1.0f <= f3CurrentProject.x) && (f3CurrentProject.x <= 1.0f) && (-1.0f <= f3CurrentProject.y) && (f3CurrentProject.y <= 1.0f);
			if (((0.0f <= f3CurrentProject.z) && (f3CurrentProject.z <= 1.0f)) && ((bCurrentInside || bPreviousInside))) ::Draw2DLine(hDCFrameBuffer, f3PreviousProject, f3CurrentProject);
			f3PreviousProject = f3CurrentProject;
			bPreviousInside = bCurrentInside;
		}
		//if (((0.0f <= f3InitialProject.z) && (f3InitialProject.z <= 1.0f)) && ((bInitialInside || bPreviousInside))) ::Draw2DLine(hDCFrameBuffer, f3PreviousProject, f3InitialProject);
	}
}

BOOL CMesh::RayIntersectionByTriangle(XMVECTOR& xmRayOrigin, XMVECTOR& xmRayDirection, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, float* pfNearHitDistance)
{
	float fHitDistance;
	BOOL bIntersected = TriangleTests::Intersects(xmRayOrigin, xmRayDirection, v0, v1, v2, fHitDistance);
	if (bIntersected && (fHitDistance < *pfNearHitDistance)) *pfNearHitDistance = fHitDistance;

	return(bIntersected);
}

int CMesh::CheckRayIntersection(XMVECTOR& xmvPickRayOrigin, XMVECTOR& xmvPickRayDirection, float* pfNearHitDistance)
{
	int nIntersections = 0;
	bool bIntersected = m_xmOOBB.Intersects(xmvPickRayOrigin, xmvPickRayDirection, *pfNearHitDistance);
	if (bIntersected)
	{
		for (int i = 0; i < m_nPolygons; i++)
		{
			switch (m_ppPolygons[i]->m_nVertices)
			{
			case 3:
			{
				XMVECTOR v0 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[0].m_xmf3Position));
				XMVECTOR v1 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[1].m_xmf3Position));
				XMVECTOR v2 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[2].m_xmf3Position));
				BOOL bIntersected = RayIntersectionByTriangle(xmvPickRayOrigin, xmvPickRayDirection, v0, v1, v2, pfNearHitDistance);
				if (bIntersected) nIntersections++;
				break;
			}
			case 4:
			{
				XMVECTOR v0 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[0].m_xmf3Position));
				XMVECTOR v1 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[1].m_xmf3Position));
				XMVECTOR v2 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[2].m_xmf3Position));
				BOOL bIntersected = RayIntersectionByTriangle(xmvPickRayOrigin, xmvPickRayDirection, v0, v1, v2, pfNearHitDistance);
				if (bIntersected) nIntersections++;
				v0 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[0].m_xmf3Position));
				v1 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[2].m_xmf3Position));
				v2 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[3].m_xmf3Position));
				bIntersected = RayIntersectionByTriangle(xmvPickRayOrigin, xmvPickRayDirection, v0, v1, v2, pfNearHitDistance);
				if (bIntersected) nIntersections++;
				break;
			}
			}
		}
	}
	return(nIntersections);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CCubeMesh::CCubeMesh(float fWidth, float fHeight, float fDepth) : CMesh(6)
{
	float fHalfWidth = fWidth * 0.5f;
	float fHalfHeight = fHeight * 0.5f;
	float fHalfDepth = fDepth * 0.5f;

	CPolygon* pFrontFace = new CPolygon(4);
	pFrontFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	SetPolygon(0, pFrontFace);

	CPolygon* pTopFace = new CPolygon(4);
	pTopFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	pTopFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pTopFace->SetVertex(2, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pTopFace->SetVertex(3, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	SetPolygon(1, pTopFace);

	CPolygon* pBackFace = new CPolygon(4);
	pBackFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(2, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(3, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	SetPolygon(2, pBackFace);

	CPolygon* pBottomFace = new CPolygon(4);
	pBottomFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	pBottomFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	pBottomFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBottomFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	SetPolygon(3, pBottomFace);

	CPolygon* pLeftFace = new CPolygon(4);
	pLeftFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	pLeftFace->SetVertex(1, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	pLeftFace->SetVertex(2, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	pLeftFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	SetPolygon(4, pLeftFace);

	CPolygon* pRightFace = new CPolygon(4);
	pRightFace->SetVertex(0, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pRightFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pRightFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pRightFace->SetVertex(3, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	SetPolygon(5, pRightFace);

	m_xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//

void CMesh::CFrontMesh(meshXY* XY, int n, int &cnt ,float depth, float plus)
{
	CPolygon* pFrontFace = new CPolygon(n);
	for (int i = 0; i < n; ++i) {
		pFrontFace->SetVertex(i, CVertex(XY[i].x + plus, XY[i].y, -depth));
	}
	SetPolygon(cnt++, pFrontFace);
}

void CMesh::CBackMesh(meshXY* XY, int n, int& cnt, float depth, float plus)
{
	CPolygon* pBackFace = new CPolygon(n);
	for (int i = 1; i < n + 1; ++i) {
		pBackFace->SetVertex(i - 1, CVertex(XY[n - i].x + plus, XY[n - i].y, +depth));
	}
	SetPolygon(cnt++, pBackFace);
}
void CMesh::CSideMesh(meshXY* XY, int n, int& cnt, float depth, float plus)
{
	for (int i = 0; i < n; ++i) {
		CPolygon* pSideFace = new CPolygon(4);
		pSideFace->SetVertex(0, CVertex(XY[i].x + plus, XY[i].y, -depth));
		pSideFace->SetVertex(1, CVertex(XY[i].x + plus , XY[i].y, +depth));
		pSideFace->SetVertex(2, CVertex(XY[(i + 1) % n].x + plus, XY[(i + 1) % n].y, +depth));
		pSideFace->SetVertex(3, CVertex(XY[(i + 1) % n].x + plus, XY[(i + 1) % n].y, -depth));
		SetPolygon(cnt++, pSideFace);
	}
}

CWallMesh::CWallMesh(float fWidth, float fHeight, float fDepth, int nSubRects) : CMesh((4 * nSubRects * nSubRects) + 2)
{
	float fHalfWidth = fWidth * 0.5f;
	float fHalfHeight = fHeight * 0.5f;
	float fHalfDepth = fDepth * 0.5f;
	float fCellWidth = fWidth * (1.0f / nSubRects);
	float fCellHeight = fHeight * (1.0f / nSubRects);
	float fCellDepth = fDepth * (1.0f / nSubRects);

	int k = 0;
	CPolygon* pLeftFace;
	for (int i = 0; i < nSubRects; i++)
	{
		for (int j = 0; j < nSubRects; j++)
		{
			pLeftFace = new CPolygon(4);
			pLeftFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight + (i * fCellHeight), -fHalfDepth + (j * fCellDepth)));
			pLeftFace->SetVertex(1, CVertex(-fHalfWidth, -fHalfHeight + ((i + 1) * fCellHeight), -fHalfDepth + (j * fCellDepth)));
			pLeftFace->SetVertex(2, CVertex(-fHalfWidth, -fHalfHeight + ((i + 1) * fCellHeight), -fHalfDepth + ((j + 1) * fCellDepth)));
			pLeftFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight + (i * fCellHeight), -fHalfDepth + ((j + 1) * fCellDepth)));
			SetPolygon(k++, pLeftFace);
		}
	}

	CPolygon* pRightFace;
	for (int i = 0; i < nSubRects; i++)
	{
		for (int j = 0; j < nSubRects; j++)
		{
			pRightFace = new CPolygon(4);
			pRightFace->SetVertex(0, CVertex(+fHalfWidth, -fHalfHeight + (i * fCellHeight), -fHalfDepth + (j * fCellDepth)));
			pRightFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight + ((i + 1) * fCellHeight), -fHalfDepth + (j * fCellDepth)));
			pRightFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight + ((i + 1) * fCellHeight), -fHalfDepth + ((j + 1) * fCellDepth)));
			pRightFace->SetVertex(3, CVertex(+fHalfWidth, -fHalfHeight + (i * fCellHeight), -fHalfDepth + ((j + 1) * fCellDepth)));
			SetPolygon(k++, pRightFace);
		}
	}

	CPolygon* pTopFace;
	for (int i = 0; i < nSubRects; i++)
	{
		for (int j = 0; j < nSubRects; j++)
		{
			pTopFace = new CPolygon(4);
			pTopFace->SetVertex(0, CVertex(-fHalfWidth + (i * fCellWidth), +fHalfHeight, -fHalfDepth + (j * fCellDepth)));
			pTopFace->SetVertex(1, CVertex(-fHalfWidth + ((i + 1) * fCellWidth), +fHalfHeight, -fHalfDepth + (j * fCellDepth)));
			pTopFace->SetVertex(2, CVertex(-fHalfWidth + ((i + 1) * fCellWidth), +fHalfHeight, -fHalfDepth + ((j + 1) * fCellDepth)));
			pTopFace->SetVertex(3, CVertex(-fHalfWidth + (i * fCellWidth), +fHalfHeight, -fHalfDepth + ((j + 1) * fCellDepth)));
			SetPolygon(k++, pTopFace);
		}
	}

	CPolygon* pBottomFace;
	for (int i = 0; i < nSubRects; i++)
	{
		for (int j = 0; j < nSubRects; j++)
		{
			pBottomFace = new CPolygon(4);
			pBottomFace->SetVertex(0, CVertex(-fHalfWidth + (i * fCellWidth), -fHalfHeight, -fHalfDepth + (j * fCellDepth)));
			pBottomFace->SetVertex(1, CVertex(-fHalfWidth + ((i + 1) * fCellWidth), -fHalfHeight, -fHalfDepth + (j * fCellDepth)));
			pBottomFace->SetVertex(2, CVertex(-fHalfWidth + ((i + 1) * fCellWidth), -fHalfHeight, -fHalfDepth + ((j + 1) * fCellDepth)));
			pBottomFace->SetVertex(3, CVertex(-fHalfWidth + (i * fCellWidth), -fHalfHeight, -fHalfDepth + ((j + 1) * fCellDepth)));
			SetPolygon(k++, pBottomFace);
		}
	}

	CPolygon* pFrontFace = new CPolygon(4);
	pFrontFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	SetPolygon(k++, pFrontFace);

	CPolygon* pBackFace = new CPolygon(4);
	pBackFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(2, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(3, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	SetPolygon(k++, pBackFace);

	m_xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAirplaneMesh::CAirplaneMesh(float fWidth, float fHeight, float fDepth) : CMesh(18)
{
	int cnt = 0;
	float Objectcnt = 0.0f;
	float fHalfWidth = fWidth * 0.5f;
	float fHalfHeight = fHeight * 0.5f;
	float fHalfDepth = fDepth * 0.5f;

	CPolygon* pTopFace = new CPolygon(4);
	pTopFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, 0.0f));
	pTopFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, 0.0f));
	pTopFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, 0.0f));
	pTopFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, 0.0f));
	SetPolygon(0, pTopFace);

	CPolygon* pFrontFace = new CPolygon(4);
	pFrontFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	pFrontFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pFrontFace->SetVertex(2, CVertex(+fHalfWidth, +fHalfHeight, 0.0f));
	pFrontFace->SetVertex(3, CVertex(-fHalfWidth, +fHalfHeight, 0.0f));
	SetPolygon(1, pFrontFace);

	CPolygon* pBottomFace = new CPolygon(4);
	pBottomFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBottomFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBottomFace->SetVertex(2, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pBottomFace->SetVertex(3, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	SetPolygon(2, pBottomFace);

	CPolygon* pBackFace = new CPolygon(4);
	pBackFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight, 0.0f));
	pBackFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight, 0.0f));
	pBackFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	SetPolygon(3, pBackFace);

	CPolygon* pLeftFace = new CPolygon(4);
	pLeftFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	pLeftFace->SetVertex(1, CVertex(-fHalfWidth, +fHalfHeight, 0.0f));
	pLeftFace->SetVertex(2, CVertex(-fHalfWidth, -fHalfHeight, 0.0f));
	pLeftFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	SetPolygon(4, pLeftFace);

	CPolygon* pRightFace = new CPolygon(4);
	pRightFace->SetVertex(0, CVertex(+fHalfWidth, +fHalfHeight, 0.0f));
	pRightFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pRightFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pRightFace->SetVertex(3, CVertex(+fHalfWidth, -fHalfHeight, 0.0f));
	SetPolygon(5, pRightFace);

	//=========================================================================
	CPolygon* pTopFace1 = new CPolygon(4);
	pTopFace1->SetVertex(0, CVertex(-fHalfWidth * 0.5f, +fHalfHeight * 0.5f, -fHalfDepth * 0.5f));
	pTopFace1->SetVertex(1, CVertex(+fHalfWidth * 0.5f, +fHalfHeight * 0.5f, -fHalfDepth * 0.5f));
	pTopFace1->SetVertex(2, CVertex(+fHalfWidth * 0.5f, -fHalfHeight * 0.5f, -fHalfDepth * 0.5f));
	pTopFace1->SetVertex(3, CVertex(-fHalfWidth * 0.5f, -fHalfHeight * 0.5f, -fHalfDepth * 0.5f));
	SetPolygon(6, pTopFace1);

	CPolygon* pFrontFace1 = new CPolygon(4);
	pFrontFace1->SetVertex(0, CVertex(-fHalfWidth * 0.5f, +fHalfHeight * 0.5f, 0.0f));
	pFrontFace1->SetVertex(1, CVertex(+fHalfWidth * 0.5f, +fHalfHeight * 0.5f, 0.0f));
	pFrontFace1->SetVertex(2, CVertex(+fHalfWidth * 0.5f, +fHalfHeight * 0.5f, -fHalfDepth * 0.5f));
	pFrontFace1->SetVertex(3, CVertex(-fHalfWidth * 0.5f, +fHalfHeight * 0.5f, -fHalfDepth * 0.5f));
	SetPolygon(7, pFrontFace1);

	CPolygon* pBottomFace1 = new CPolygon(4);
	pBottomFace1->SetVertex(0, CVertex(-fHalfWidth * 0.5f, -fHalfHeight * 0.5f, 0.0f));
	pBottomFace1->SetVertex(1, CVertex(+fHalfWidth * 0.5f, -fHalfHeight * 0.5f, 0.0f));
	pBottomFace1->SetVertex(2, CVertex(+fHalfWidth * 0.5f, +fHalfHeight * 0.5f, 0.0f));
	pBottomFace1->SetVertex(3, CVertex(-fHalfWidth * 0.5f, +fHalfHeight * 0.5f, 0.0f));
	SetPolygon(8, pBottomFace1);

	CPolygon* pBackFace1 = new CPolygon(4);
	pBackFace1->SetVertex(0, CVertex(-fHalfWidth * 0.5f, -fHalfHeight * 0.5f, -fHalfDepth * 0.5f));
	pBackFace1->SetVertex(1, CVertex(+fHalfWidth * 0.5f, -fHalfHeight * 0.5f, -fHalfDepth * 0.5f));
	pBackFace1->SetVertex(2, CVertex(+fHalfWidth * 0.5f, -fHalfHeight * 0.5f, 0.0f));
	pBackFace1->SetVertex(3, CVertex(-fHalfWidth * 0.5f, -fHalfHeight * 0.5f, 0.0f));
	SetPolygon(9, pBackFace1);

	CPolygon* pLeftFace1 = new CPolygon(4);
	pLeftFace1->SetVertex(0, CVertex(-fHalfWidth * 0.5f, +fHalfHeight * 0.5f, 0.0f));
	pLeftFace1->SetVertex(1, CVertex(-fHalfWidth * 0.5f, +fHalfHeight * 0.5f, -fHalfDepth * 0.5f));
	pLeftFace1->SetVertex(2, CVertex(-fHalfWidth * 0.5f, -fHalfHeight * 0.5f, -fHalfDepth * 0.5f));
	pLeftFace1->SetVertex(3, CVertex(-fHalfWidth * 0.5f, -fHalfHeight * 0.5f, 0.0f));
	SetPolygon(10, pLeftFace1);

	CPolygon* pRightFace1 = new CPolygon(4);
	pRightFace1->SetVertex(0, CVertex(+fHalfWidth * 0.5f, +fHalfHeight * 0.5f, -fHalfDepth * 0.5f));
	pRightFace1->SetVertex(1, CVertex(+fHalfWidth * 0.5f, +fHalfHeight * 0.5f, 0.0f));
	pRightFace1->SetVertex(2, CVertex(+fHalfWidth * 0.5f, -fHalfHeight * 0.5f, 0.0f));
	pRightFace1->SetVertex(3, CVertex(+fHalfWidth * 0.5f, -fHalfHeight * 0.5f, -fHalfDepth * 0.5f));
	SetPolygon(11, pRightFace1);
	//========================================================================
	CPolygon* pFace3 = new CPolygon(4);
	pFace3->SetVertex(0, CVertex(-fHalfWidth * 0.125f, +fHalfHeight * 1.5f, -fHalfDepth * 0.375f));
	pFace3->SetVertex(1, CVertex(+fHalfWidth * 0.125f, +fHalfHeight * 1.5f, -fHalfDepth * 0.375f));
	pFace3->SetVertex(2, CVertex(+fHalfWidth * 0.125f, +fHalfHeight * 0.5f, -fHalfDepth * 0.375f));
	pFace3->SetVertex(3, CVertex(-fHalfWidth * 0.125f, +fHalfHeight * 0.5f, -fHalfDepth * 0.375f));
	SetPolygon(12, pFace3);

	CPolygon* pFace2 = new CPolygon(4);
	pFace2->SetVertex(0, CVertex(-fHalfWidth * 0.125f, +fHalfHeight * 1.5f, -fHalfDepth * 0.125f));
	pFace2->SetVertex(1, CVertex(+fHalfWidth * 0.125f, +fHalfHeight * 1.5f, -fHalfDepth * 0.125f));
	pFace2->SetVertex(2, CVertex(+fHalfWidth * 0.125f, +fHalfHeight * 1.5f, -fHalfDepth * 0.375f));
	pFace2->SetVertex(3, CVertex(-fHalfWidth * 0.125f, +fHalfHeight * 1.5f, -fHalfDepth * 0.375f));
	SetPolygon(13, pFace2);

	CPolygon* pFace4 = new CPolygon(4);
	pFace4->SetVertex(0, CVertex(-fHalfWidth * 0.125f, +fHalfHeight * 0.5f, -fHalfDepth * 0.125f));
	pFace4->SetVertex(1, CVertex(+fHalfWidth * 0.125f, +fHalfHeight * 0.5f, -fHalfDepth * 0.125f));
	pFace4->SetVertex(2, CVertex(+fHalfWidth * 0.125f, +fHalfHeight * 1.5f, -fHalfDepth * 0.125f));
	pFace4->SetVertex(3, CVertex(-fHalfWidth * 0.125f, +fHalfHeight * 1.5f, -fHalfDepth * 0.125f));
	SetPolygon(14, pFace4);

	CPolygon* pFace5 = new CPolygon(4);
	pFace5->SetVertex(0, CVertex(-fHalfWidth * 0.125f, +fHalfHeight * 0.5f, -fHalfDepth * 0.375f));
	pFace5->SetVertex(1, CVertex(+fHalfWidth * 0.125f, +fHalfHeight * 0.5f, -fHalfDepth * 0.375f));
	pFace5->SetVertex(2, CVertex(+fHalfWidth * 0.125f, +fHalfHeight * 0.5f, -fHalfDepth * 0.125f));
	pFace5->SetVertex(3, CVertex(-fHalfWidth * 0.125f, +fHalfHeight * 0.5f, -fHalfDepth * 0.125f));
	SetPolygon(15, pFace5);

	CPolygon* pFace6 = new CPolygon(4);
	pFace6->SetVertex(0, CVertex(-fHalfWidth * 0.125f, +fHalfHeight * 1.5f, -fHalfDepth * 0.125f));
	pFace6->SetVertex(1, CVertex(-fHalfWidth * 0.125f, +fHalfHeight * 1.5f, -fHalfDepth * 0.375f));
	pFace6->SetVertex(2, CVertex(-fHalfWidth * 0.125f, +fHalfHeight * 0.5f, -fHalfDepth * 0.375f));
	pFace6->SetVertex(3, CVertex(-fHalfWidth * 0.125f, +fHalfHeight * 0.5f, -fHalfDepth * 0.125f));
	SetPolygon(16, pFace6);

	CPolygon* pFace1 = new CPolygon(4);
	pFace1->SetVertex(0, CVertex(+fHalfWidth * 0.125f, +fHalfHeight * 1.5f, -fHalfDepth * 0.125f));
	pFace1->SetVertex(1, CVertex(+fHalfWidth * 0.125f, +fHalfHeight * 1.5f, -fHalfDepth * 0.125f));
	pFace1->SetVertex(2, CVertex(+fHalfWidth * 0.125f, +fHalfHeight * 0.5f, -fHalfDepth * 0.125f));
	pFace1->SetVertex(3, CVertex(+fHalfWidth * 0.125f, +fHalfHeight * 0.5f, -fHalfDepth * 0.125f));
	SetPolygon(17, pFace1);

	

	m_xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CAxisMesh::CAxisMesh(float fWidth, float fHeight, float fDepth) : CMesh(3)
{
	float fHalfWidth = fWidth * 0.5f;
	float fHalfHeight = fHeight * 0.5f;
	float fHalfDepth = fDepth * 0.5f;

	CPolygon* pxAxis = new CPolygon(2);
	pxAxis->SetVertex(0, CVertex(-fHalfWidth, 0.0f, 0.0f));
	pxAxis->SetVertex(1, CVertex(+fHalfWidth, 0.0f, 0.0f));
	SetPolygon(0, pxAxis);

	CPolygon* pyAxis = new CPolygon(2);
	pyAxis->SetVertex(0, CVertex(0.0f, -fHalfWidth, 0.0f));
	pyAxis->SetVertex(1, CVertex(0.0f, +fHalfWidth, 0.0f));
	SetPolygon(1, pyAxis);

	CPolygon* pzAxis = new CPolygon(2);
	pzAxis->SetVertex(0, CVertex(0.0f, 0.0f, -fHalfWidth));
	pzAxis->SetVertex(1, CVertex(0.0f, 0.0f, +fHalfWidth));
	SetPolygon(2, pzAxis);
}

void CAxisMesh::Render(HDC hDCFrameBuffer)
{
	XMFLOAT3 f3PreviousProject = CGraphicsPipeline::Project(m_ppPolygons[0]->m_pVertices[0].m_xmf3Position);
	XMFLOAT3 f3CurrentProject = CGraphicsPipeline::Project(m_ppPolygons[0]->m_pVertices[1].m_xmf3Position);
	HPEN hPen = ::CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
	HPEN hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);
	::Draw2DLine(hDCFrameBuffer, f3PreviousProject, f3CurrentProject);
	::SelectObject(hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);

	f3PreviousProject = CGraphicsPipeline::Project(m_ppPolygons[1]->m_pVertices[0].m_xmf3Position);
	f3CurrentProject = CGraphicsPipeline::Project(m_ppPolygons[1]->m_pVertices[1].m_xmf3Position);
	hPen = ::CreatePen(PS_SOLID, 0, RGB(0, 0, 255));
	hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);
	::Draw2DLine(hDCFrameBuffer, f3PreviousProject, f3CurrentProject);
	::SelectObject(hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);

	f3PreviousProject = CGraphicsPipeline::Project(m_ppPolygons[2]->m_pVertices[0].m_xmf3Position);
	f3CurrentProject = CGraphicsPipeline::Project(m_ppPolygons[2]->m_pVertices[1].m_xmf3Position);
	hPen = ::CreatePen(PS_SOLID, 0, RGB(0, 255, 0));
	hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);
	::Draw2DLine(hDCFrameBuffer, f3PreviousProject, f3CurrentProject);
	::SelectObject(hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);
}
