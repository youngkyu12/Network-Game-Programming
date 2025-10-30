#pragma once
/////////////////////////////////////////////////////////////////////////////////////////////////////
//


class CVertex
{
public:
	CVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CVertex(float x, float y, float z) { m_xmf3Position = XMFLOAT3(x, y, z); }
	~CVertex() {}

	XMFLOAT3					m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
};

class CPolygon
{
public:
	CPolygon() {}
	CPolygon(int nVertices);
	~CPolygon();

	int							m_nVertices = 0;
	CVertex* m_pVertices = NULL;

	void SetVertex(int nIndex, CVertex& vertex);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct meshXY {
	float x;
	float y;
};
class CMesh
{
public:
	CMesh() {}
	CMesh(int nPolygons);
	virtual ~CMesh();

private:
	int							m_nReferences = 1;

public:
	void AddRef() { m_nReferences++; }
	void Release() { m_nReferences--; if (m_nReferences <= 0) delete this; }

protected:
	int							m_nPolygons = 0;
	CPolygon** m_ppPolygons = NULL;

public:
	BoundingOrientedBox			m_xmOOBB = BoundingOrientedBox();
public:
	void CFrontMesh(meshXY* mesh, int nVertex, int& nPolygon, float depth, float add_X);
	void CBackMesh(meshXY* mesh, int nVertex, int& nPolygon, float depth, float add_X);
	void CSideMesh(meshXY* mesh, int nVertex, int& nPolygon, float depth, float add_X);
public:
	void SetPolygon(int nIndex, CPolygon* pPolygon);

	virtual void Render(HDC hDCFrameBuffer);

	BOOL RayIntersectionByTriangle(XMVECTOR& xmRayOrigin, XMVECTOR& xmRayDirection, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, float* pfNearHitDistance);
	int CheckRayIntersection(XMVECTOR& xmvPickRayOrigin, XMVECTOR& xmvPickRayDirection, float* pfNearHitDistance);
};

class CCubeMesh : public CMesh
{
public:
	CCubeMesh(float fWidth = 4.0f, float fHeight = 4.0f, float fDepth = 4.0f);
	virtual ~CCubeMesh() {}
};

class CStartMesh : public CMesh
{
public:
	CStartMesh(float fWidth = 1.0f, float fHeight = 1.0f, float fDepth = 1.0f);
	virtual ~CStartMesh() {}
private:
};

class CNameMesh : public CMesh
{
public:
	CNameMesh(float fWidth = 4.0f, float fHeight = 4.0f, float fDepth = 4.0f);
	virtual ~CNameMesh() {}
};

class CTutorialMesh : public CMesh
{
public:
	CTutorialMesh(float fWidth = 4.0f, float fHeight = 4.0f, float fDepth = 4.0f);
	virtual ~CTutorialMesh() {}
};

class CLevel_1Mesh : public CMesh
{
public:
	CLevel_1Mesh(float fWidth = 4.0f, float fHeight = 4.0f, float fDepth = 4.0f);
	virtual ~CLevel_1Mesh() {}
};

class CLevel_2Mesh : public CMesh
{
public:
	CLevel_2Mesh(float fWidth = 4.0f, float fHeight = 4.0f, float fDepth = 4.0f);
	virtual ~CLevel_2Mesh() {}
};

class CstartMesh : public CMesh
{
public:
	CstartMesh(float fWidth = 4.0f, float fHeight = 4.0f, float fDepth = 4.0f);
	virtual ~CstartMesh() {}
};

class CEndMesh : public CMesh
{
public:
	CEndMesh(float fWidth = 4.0f, float fHeight = 4.0f, float fDepth = 4.0f);
	virtual ~CEndMesh() {}
};

class CYouwinMesh : public CMesh
{
public:
	CYouwinMesh(float fWidth = 4.0f, float fHeight = 4.0f, float fDepth = 4.0f);
	virtual ~CYouwinMesh() {}
};

class CRailMesh : public CMesh
{
public:
	CRailMesh(float fWidth = 4.0f, float fHeight = 4.0f, float fDepth = 4.0f);
	virtual ~CRailMesh() {}
};

class CRailCurveMesh : public CMesh
{
public:
	CRailCurveMesh(float fWidth = 4.0f, float fHeight = 4.0f, float fDepth = 4.0f);
	virtual ~CRailCurveMesh() {}
};

class CTrainMesh : public CMesh
{
public:
	CTrainMesh(float fWidth = 4.0f, float fHeight = 4.0f, float fDepth = 4.0f);
	virtual ~CTrainMesh() {}
};


class CWallMesh : public CMesh
{
public:
	CWallMesh(float fWidth = 4.0f, float fHeight = 4.0f, float fDepth = 4.0f, int nSubRects = 20);
	virtual ~CWallMesh() {}
};

class CAirplaneMesh : public CMesh
{
public:
	CAirplaneMesh(float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 4.0f);
	virtual ~CAirplaneMesh() {}
};

class CAxisMesh : public CMesh
{
public:
	CAxisMesh(float fWidth = 4.0f, float fHeight = 4.0f, float fDepth = 4.0f);
	virtual ~CAxisMesh() {}

	virtual void Render(HDC hDCFrameBuffer);
};
