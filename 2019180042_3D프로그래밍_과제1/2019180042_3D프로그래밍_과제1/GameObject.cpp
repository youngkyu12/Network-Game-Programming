#include "stdafx.h"
#include "GameObject.h"
#include "GraphicsPipeline.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
inline float RandF(float fMin, float fMax)
{
	return(fMin + ((float)rand() / (float)RAND_MAX) * (fMax - fMin));
}

XMVECTOR RandomUnitVectorOnSphere()
{
	XMVECTOR xmvOne = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR xmvZero = XMVectorZero();

	while (true)
	{
		XMVECTOR v = XMVectorSet(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), 0.0f);
		if (!XMVector3Greater(XMVector3LengthSq(v), xmvOne)) return(XMVector3Normalize(v));
	}
}


CGameObject::~CGameObject(void)
{
	if (m_pMesh) m_pMesh->Release();
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}

void CGameObject::SetPosition(XMFLOAT3& xmf3Position)
{
	m_xmf4x4World._41 = xmf3Position.x;
	m_xmf4x4World._42 = xmf3Position.y;
	m_xmf4x4World._43 = xmf3Position.z;
}

XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

XMFLOAT3 CGameObject::GetLook()
{
	XMFLOAT3 xmf3LookAt(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);
	xmf3LookAt = Vector3::Normalize(xmf3LookAt);
	return(xmf3LookAt);
}

XMFLOAT3 CGameObject::GetUp()
{
	XMFLOAT3 xmf3Up(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);
	xmf3Up = Vector3::Normalize(xmf3Up);
	return(xmf3Up);
}

XMFLOAT3 CGameObject::GetRight()
{
	XMFLOAT3 xmf3Right(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
	xmf3Right = Vector3::Normalize(xmf3Right);
	return(xmf3Right);
}

void CGameObject::SetRotationTransform(XMFLOAT4X4* pmxf4x4Transform)
{
	m_xmf4x4World._11 = pmxf4x4Transform->_11; m_xmf4x4World._12 = pmxf4x4Transform->_12; m_xmf4x4World._13 = pmxf4x4Transform->_13;
	m_xmf4x4World._21 = pmxf4x4Transform->_21; m_xmf4x4World._22 = pmxf4x4Transform->_22; m_xmf4x4World._23 = pmxf4x4Transform->_23;
	m_xmf4x4World._31 = pmxf4x4Transform->_31; m_xmf4x4World._32 = pmxf4x4Transform->_32; m_xmf4x4World._33 = pmxf4x4Transform->_33;
}



void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Right, fDistance));

	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Up, fDistance));

	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3LookAt = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3LookAt, fDistance));

	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationYawPitchRoll(fPitch, fYaw, fRoll);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Rotate(XMFLOAT3& xmf3RotationAxis, float fAngle)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationAxis(xmf3RotationAxis, fAngle);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Move(float x, float y, float z)
{
	SetPosition(m_xmf4x4World._41 + x, m_xmf4x4World._42 + y, m_xmf4x4World._43 + z);
}

void CGameObject::LookTo(XMFLOAT3& xmf3LookTo, XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookToLH(GetPosition(), xmf3LookTo, xmf3Up);
	m_xmf4x4World._11 = xmf4x4View._11; m_xmf4x4World._12 = xmf4x4View._21; m_xmf4x4World._13 = xmf4x4View._31;
	m_xmf4x4World._21 = xmf4x4View._12; m_xmf4x4World._22 = xmf4x4View._22; m_xmf4x4World._23 = xmf4x4View._32;
	m_xmf4x4World._31 = xmf4x4View._13; m_xmf4x4World._32 = xmf4x4View._23; m_xmf4x4World._33 = xmf4x4View._33;
}

void CGameObject::LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(GetPosition(), xmf3LookAt, xmf3Up);
	m_xmf4x4World._11 = xmf4x4View._11; m_xmf4x4World._12 = xmf4x4View._21; m_xmf4x4World._13 = xmf4x4View._31;
	m_xmf4x4World._21 = xmf4x4View._12; m_xmf4x4World._22 = xmf4x4View._22; m_xmf4x4World._23 = xmf4x4View._32;
	m_xmf4x4World._31 = xmf4x4View._13; m_xmf4x4World._32 = xmf4x4View._23; m_xmf4x4World._33 = xmf4x4View._33;
}


void CGameObject::UpdateBoundingBox()
{
	if (m_pMesh)
	{
		m_pMesh->m_xmOOBB.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
		XMStoreFloat4(&m_xmOOBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBB.Orientation)));
	}
}

void CGameObject::Animate(float fElapsedTime)
{
	if (m_fRotationSpeed != 0.0f) Rotate(m_xmf3RotationAxis, m_fRotationSpeed * fElapsedTime);
	if (m_fMovingSpeed != 0.0f) {
		//Move(m_xmf3MovingDirection, m_fMovingSpeed * fElapsedTime);
	}

	UpdateBoundingBox();
}

void CGameObject::Render(HDC hDCFrameBuffer, XMFLOAT4X4* pxmf4x4World, CMesh* pMesh)
{
	if (pMesh)
	{
		
		CGraphicsPipeline::SetWorldTransform(pxmf4x4World);
		
		HPEN hPen = ::CreatePen(PS_SOLID, 0, m_dwColor);
		HPEN hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);
		
		pMesh->Render(hDCFrameBuffer);
		::SelectObject ( hDCFrameBuffer , hOldPen );
		::DeleteObject ( hPen );
		
	}
}

void CGameObject::RenderFilled(HDC hDCFrameBuffer, XMFLOAT4X4* pxmf4x4World, CMesh* pMesh, COLORREF fillColor)
{
	if (pMesh)
	{

		CGraphicsPipeline::SetWorldTransform(pxmf4x4World);
		pMesh->RenderFilled(hDCFrameBuffer, fillColor);

	}
}

void CGameObject::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	if (pCamera->IsInFrustum(m_xmOOBB)) CGameObject::Render(hDCFrameBuffer, &m_xmf4x4World, m_pMesh);
}

void CGameObject::RenderFilled(HDC hDCFrameBuffer, CCamera* pCamera, COLORREF fillColor)
{
	if (pCamera->IsInFrustum(m_xmOOBB)) CGameObject::RenderFilled(hDCFrameBuffer, &m_xmf4x4World, m_pMesh, fillColor);
}

void CGameObject::GenerateRayForPicking(XMVECTOR& xmvPickPosition, XMMATRIX& xmmtxView, XMVECTOR& xmvPickRayOrigin, XMVECTOR& xmvPickRayDirection)
{
	XMMATRIX xmmtxToModel = XMMatrixInverse(NULL, XMLoadFloat4x4(&m_xmf4x4World) * xmmtxView);

	XMFLOAT3 xmf3CameraOrigin(0.0f, 0.0f, 0.0f);
	xmvPickRayOrigin = XMVector3TransformCoord(XMLoadFloat3(&xmf3CameraOrigin), xmmtxToModel);
	xmvPickRayDirection = XMVector3TransformCoord(xmvPickPosition, xmmtxToModel);
	xmvPickRayDirection = XMVector3Normalize(xmvPickRayDirection - xmvPickRayOrigin);
}

int CGameObject::PickObjectByRayIntersection(XMVECTOR& xmvPickPosition, XMMATRIX& xmmtxView, float* pfHitDistance)
{
	int nIntersected = 0;
	if (m_pMesh)
	{
		XMVECTOR xmvPickRayOrigin, xmvPickRayDirection;
		GenerateRayForPicking(xmvPickPosition, xmmtxView, xmvPickRayOrigin, xmvPickRayDirection);
		nIntersected = m_pMesh->CheckRayIntersection(xmvPickRayOrigin, xmvPickRayDirection, pfHitDistance);
	}
	return(nIntersected);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CWallsObject::CWallsObject()
{
}

CWallsObject::~CWallsObject()
{
}

void CWallsObject::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	CGameObject::Render(hDCFrameBuffer, &m_xmf4x4World, m_pMesh);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
XMFLOAT3 CExplosiveObject::m_pxmf3SphereVectors[EXPLOSION_DEBRISES];
CMesh* CExplosiveObject::m_pExplosionMesh = NULL;

CExplosiveObject::CExplosiveObject()
{
}

CExplosiveObject::~CExplosiveObject()
{
}

void CExplosiveObject::PrepareExplosion()
{
	for (int i = 0; i < EXPLOSION_DEBRISES; i++) XMStoreFloat3(&m_pxmf3SphereVectors[i], ::RandomUnitVectorOnSphere());

	m_pExplosionMesh = new CCubeMesh(0.5f, 0.5f, 0.5f);
}

void CExplosiveObject::Animate(float fElapsedTime)
{
	if (m_bBlowingUp)
	{
		m_fElapsedTimes += fElapsedTime;
		if (m_fElapsedTimes <= m_fDuration)
		{
			XMFLOAT3 xmf3Position = GetPosition();
			for (int i = 0; i < EXPLOSION_DEBRISES; i++)
			{
				m_pxmf4x4Transforms[i] = Matrix4x4::Identity();
				m_pxmf4x4Transforms[i]._41 = xmf3Position.x + m_pxmf3SphereVectors[i].x * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i]._42 = xmf3Position.y + m_pxmf3SphereVectors[i].y * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i]._43 = xmf3Position.z + m_pxmf3SphereVectors[i].z * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i] = Matrix4x4::Multiply(Matrix4x4::RotationAxis(m_pxmf3SphereVectors[i], m_fExplosionRotation * m_fElapsedTimes), m_pxmf4x4Transforms[i]);
			}
		}
		else
		{
			m_bBlowingUp = false;
			m_fElapsedTimes = 0.0f;
			die = true;
		}
	}
	else
	{
		CGameObject::Animate(fElapsedTime);
	}
}

void CExplosiveObject::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	if (die == false) {
		if (m_bBlowingUp)
		{
			for (int i = 0; i < EXPLOSION_DEBRISES; i++)
			{
				CGameObject::Render(hDCFrameBuffer, &m_pxmf4x4Transforms[i], m_pExplosionMesh);
			}
		}
		else
		{
			CGameObject::Render(hDCFrameBuffer, pCamera);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMesh* CBulletObject::s_pExplosionMesh = NULL;
XMFLOAT3 CBulletObject::s_pxmf3SphereVectors[EXPLOSION_DEBRISES];

CBulletObject::CBulletObject(float fEffectiveRange)
{
	m_fBulletEffectiveRange = fEffectiveRange;
	s_pExplosionMesh = NULL;
	s_pxmf3SphereVectors[EXPLOSION_DEBRISES];
}

CBulletObject::~CBulletObject()
{
}

void CBulletObject::SetFirePosition(XMFLOAT3 xmf3FirePosition)
{
	m_xmf3FirePosition = xmf3FirePosition;
	SetPosition(xmf3FirePosition);
}

void CBulletObject::Reset()
{
	m_pLockedObject = NULL;
	m_fElapsedTimeAfterFire = 0;
	m_fMovingDistance = 0;
	m_fRotationAngle = 0.0f;
	m_bHasTarget = false;
	m_bExploding = false;
	m_fExplosionElapsed = 0.0f;
	m_fTravelLimit = m_fBulletEffectiveRange;
	m_bActive = false;
}

void CBulletObject::PrepareExplosion()
{
	// 한 번만 준비
	if (!s_pExplosionMesh)
	{
		for (int i = 0; i < EXPLOSION_DEBRISES; i++)
			XMStoreFloat3(&s_pxmf3SphereVectors[i], ::RandomUnitVectorOnSphere());
		s_pExplosionMesh = new CCubeMesh(0.5f, 0.5f, 0.5f);
	}
}

void CBulletObject::StartExplosion()
{
	// 파편 폭발 시작
	m_bExploding = true;              // 기존 깜박임 플래그 유지 가능(사용 안 해도 됨)
	m_fExplosionElapsed = 0.0f;

	m_bBlowingUp = true;              // 파편 애니메이션 활성화
	m_fElapsedTimes = 0.0f;
	// 이동 중지
	m_fMovingDistance = 0.0f;
}

void CBulletObject::Animate(float fElapsedTime)
{
	// 파편 폭발 중이면 파편 애니메이션(ExplosiveObject와 동일 로직)
	if (m_bBlowingUp)
	{
		m_fElapsedTimes += fElapsedTime;
		if (m_fElapsedTimes <= m_fDuration)
		{
			XMFLOAT3 xmf3Position = GetPosition();
			for (int i = 0; i < EXPLOSION_DEBRISES; i++)
			{
				m_pxmf4x4Transforms[i] = Matrix4x4::Identity();
				m_pxmf4x4Transforms[i]._41 = xmf3Position.x + s_pxmf3SphereVectors[i].x * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i]._42 = xmf3Position.y + s_pxmf3SphereVectors[i].y * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i]._43 = xmf3Position.z + s_pxmf3SphereVectors[i].z * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i] = Matrix4x4::Multiply(
					Matrix4x4::RotationAxis(s_pxmf3SphereVectors[i], m_fExplosionRotation * m_fElapsedTimes),
					m_pxmf4x4Transforms[i]);
			}
		}
		else
		{
			// 파편 애니메이션 끝
			m_bBlowingUp = false;
			m_fElapsedTimes = 0.0f;
			die = true;
			Reset(); // 재사용 가능 상태로
		}
		return;
	}
	// 기존 비행/충돌 이동
	m_fElapsedTimeAfterFire += fElapsedTime;
	float fStep = m_fMovingSpeed * fElapsedTime;

	XMFLOAT3 cur = GetPosition();
	XMFLOAT3 move = Vector3::ScalarProduct(m_xmf3MovingDirection, fStep, false);
	SetPosition(Vector3::Add(cur, move));
	m_fMovingDistance += fStep;

	// 회전 효과
	m_fRotationAngle += m_fRotationSpeed * fElapsedTime;
	if (m_fRotationAngle > 360.0f) m_fRotationAngle -= 360.0f;
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationYawPitchRoll(0.0f, XMConvertToRadians(m_fRotationAngle), 0.0f);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);

	UpdateBoundingBox();

	// 이동 완료 충돌 위치 도달 시 파편 폭발 시작
	if (m_fMovingDistance >= m_fTravelLimit && m_bHasTarget)
	{
		StartExplosion();
	}
}

// 폭발 중에는 파편들을 렌더, 아니면 기존 메시 렌더
void CBulletObject::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	if (m_bBlowingUp)
	{
		if (!s_pExplosionMesh) PrepareExplosion();
		for (int i = 0; i < EXPLOSION_DEBRISES; i++)
		{
			CGameObject::Render(hDCFrameBuffer, &m_pxmf4x4Transforms[i], s_pExplosionMesh);
		}
		return;
	}
	// 기존 렌더 경로
	CGameObject::Render(hDCFrameBuffer, pCamera);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
void CAxisObject::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	CGraphicsPipeline::SetWorldTransform(&m_xmf4x4World);

	m_pMesh->Render(hDCFrameBuffer);
}

void CShieldObject::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{ 
	CGraphicsPipeline::SetWorldTransform ( &m_xmf4x4World );
	m_pMesh->RenderFilled(hDCFrameBuffer, RGB(77, 255, 200));
}
