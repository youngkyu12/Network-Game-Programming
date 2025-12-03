#include "stdafx.h"
#include "Scene.h"
#include "GraphicsPipeline.h"
#include <numeric> // 파일 상단에 추가

CScene::CScene() {};
CScene::CScene(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
}

CScene::~CScene()
{
}

void CScene::BuildObjects()
{
	CBulletObject::PrepareExplosion();

	float fHalfWidth = 200.0f, fHalfHeight = 200.0f, fHalfDepth = 200.0f;
	CWallMesh* pWallCubeMesh = new CWallMesh(fHalfWidth * 2.0f, fHalfHeight * 2.0f, fHalfDepth * 2.0f, 20);

	m_pWallsObject = new CWallsObject();
	m_pWallsObject->SetPosition(0.0f, 199.0f, 0.0f);
	m_pWallsObject->SetMesh(pWallCubeMesh);
	m_pWallsObject->SetColor(RGB(0, 0, 0));
	m_pWallsObject->m_pxmf4WallPlanes[0] = XMFLOAT4(+1.0f, 0.0f, 0.0f, fHalfWidth);
	m_pWallsObject->m_pxmf4WallPlanes[1] = XMFLOAT4(-1.0f, 0.0f, 0.0f, fHalfWidth);
	m_pWallsObject->m_pxmf4WallPlanes[2] = XMFLOAT4(0.0f, +1.0f, 0.0f, fHalfHeight);
	m_pWallsObject->m_pxmf4WallPlanes[3] = XMFLOAT4(0.0f, -1.0f, 0.0f, fHalfHeight);
	m_pWallsObject->m_pxmf4WallPlanes[4] = XMFLOAT4(0.0f, 0.0f, +1.0f, fHalfDepth);
	m_pWallsObject->m_pxmf4WallPlanes[5] = XMFLOAT4(0.0f, 0.0f, -1.0f, fHalfDepth);
	m_pWallsObject->m_xmOOBBPlayerMoveCheck = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth * 0.05f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	fHalfWidth = 10.0f, fHalfHeight = 5.0f, fHalfDepth = 2.0f;
	CCubeMesh* pbCubeMesh = new CCubeMesh(fHalfWidth * 2.0f, fHalfHeight * 2.0f, fHalfDepth * 2.0f);

	m_nObjects = 16;
	m_ppObjects = new CGameObject * [m_nObjects];

	int cols = 4;            // 가로로 4개
	int rows = 4;            // 세로로 4개
	float spacingX = 40.0f;  // X축 간격
	float spacingZ = 40.0f;  // Z축 간격

	for (int i = 0; i < m_nObjects; ++i) {
		int row = i / cols;
		int col = i % cols;     

		// 가운데 기준으로 좌우/앞뒤로 벌어지게 배치
		float x = (col - (cols - 1) / 2.0f) * spacingX;
		float z = (row - (rows - 1) / 2.0f) * spacingZ;
		m_ppObjects[i] = new CGameObject();
		m_ppObjects[i]->SetPosition(x, 2.0f, z);
		m_ppObjects[i]->SetMesh(pbCubeMesh);
		m_ppObjects[i]->SetColor(RGB(0, 0, 0));
	}


#ifdef _WITH_DRAW_AXIS
	m_pWorldAxis = new CGameObject();
	CAxisMesh* pAxisMesh = new CAxisMesh(0.5f, 0.5f, 0.5f);
	m_pWorldAxis->SetMesh(pAxisMesh);
#endif
}

void CScene::ReleaseObjects()
{
	if (CExplosiveObject::m_pExplosionMesh) CExplosiveObject::m_pExplosionMesh->Release();

	for (int i = 0; i < m_nObjects; i++) if (m_ppObjects[i]) delete m_ppObjects[i];
	if (m_ppObjects) delete[] m_ppObjects;


#ifdef _WITH_DRAW_AXIS
	if (m_pWorldAxis) delete m_pWorldAxis;
#endif
}



CGameObject* CScene::PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera)
{
	XMFLOAT3 xmf3PickPosition;
	xmf3PickPosition.x = (((2.0f * xClient) / (float)pCamera->m_Viewport.m_nWidth) - 1) / pCamera->m_xmf4x4PerspectiveProject._11;
	xmf3PickPosition.y = -(((2.0f * yClient) / (float)pCamera->m_Viewport.m_nHeight) - 1) / pCamera->m_xmf4x4PerspectiveProject._22;
	xmf3PickPosition.z = 1.0f;

	XMVECTOR xmvPickPosition = XMLoadFloat3(&xmf3PickPosition);
	XMMATRIX xmmtxView = XMLoadFloat4x4(&pCamera->m_xmf4x4View);

	int nIntersected = 0;
	float fNearestHitDistance = FLT_MAX;
	CGameObject* pNearestObject = NULL;
	for (int i = 0; i < m_nObjects; i++)
	{
		if (m_ppObjects[i]->m_objectcode != 'b') {
			float fHitDistance = FLT_MAX;
			nIntersected = m_ppObjects[i]->PickObjectByRayIntersection(xmvPickPosition, xmmtxView, &fHitDistance);
			if ((nIntersected > 0) && (fHitDistance < fNearestHitDistance))
			{
				fNearestHitDistance = fHitDistance;
				pNearestObject = m_ppObjects[i];
			}
		}
	}
	return(pNearestObject);
}



void CScene::Animate(float fElapsedTime)
{
	m_pWallsObject->Animate(fElapsedTime);
	// 바운딩 박스 업데이트
	for (int i = 0; i < m_nObjects; i++) {
		m_ppObjects[i]->Animate(fElapsedTime);
	}
}

void CScene::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	CGraphicsPipeline::SetViewport(&pCamera->m_Viewport);
	CGraphicsPipeline::SetViewPerspectiveProjectTransform(&pCamera->m_xmf4x4ViewPerspectiveProject);
	m_pWallsObject->Render(hDCFrameBuffer, pCamera);

	// 1) 객체를 뷰공간 Z(원거리 먼저)로 정렬
	std::vector<int> order(m_nObjects);
	std::iota(order.begin(), order.end(), 0);

	XMMATRIX V = XMLoadFloat4x4(&pCamera->m_xmf4x4View);
	std::sort(order.begin(), order.end(), [&](int a, int b)
		{
			XMFLOAT3 pa = m_ppObjects[a]->GetPosition();
			XMFLOAT3 pb = m_ppObjects[b]->GetPosition();
			XMVECTOR va = XMVector3TransformCoord(XMLoadFloat3(&pa), V);
			XMVECTOR vb = XMVector3TransformCoord(XMLoadFloat3(&pb), V);
			float za = XMVectorGetZ(va);
			float zb = XMVectorGetZ(vb);
			return za > zb; // 원거리(더 큰 z) 먼저
		});

	// 2) 정렬된 순서로 채움 + 와이어 프레임
	for (int idx : order) {
		// 면 채움(필요 시 반투명 alpha 인자 추가)
		m_ppObjects[idx]->RenderFilled(hDCFrameBuffer, pCamera, RGB(0, 255, 0));
		// 외곽선
		m_ppObjects[idx]->Render(hDCFrameBuffer, pCamera);
	}

	//UI
#ifdef _WITH_DRAW_AXIS
	CGraphicsPipeline::SetViewOrthographicProjectTransform(&pCamera->m_xmf4x4ViewOrthographicProject);
	m_pWorldAxis->SetRotationTransform(&m_pPlayer->m_xmf4x4World);
	m_pWorldAxis->Render(hDCFrameBuffer, pCamera);
#endif
}



