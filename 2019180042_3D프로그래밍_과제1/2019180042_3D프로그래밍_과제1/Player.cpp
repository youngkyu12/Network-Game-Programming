#include "stdafx.h"
#include "Player.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CPlayer** CPlayer::s_ppPlayers = nullptr;
int CPlayer::s_nPlayers = 0;

CPlayer::CPlayer()
{
	PrevFire = false;
	s_ppPlayers = nullptr;
	s_nPlayers = 0;
}

CPlayer::~CPlayer()
{
}

void CPlayer::SetPosition(float x, float y, float z)
{
	m_xmf3Position = XMFLOAT3(x, y, z);

	CGameObject::SetPosition(x, y, z);
}

void CPlayer::SetCameraOffset(XMFLOAT3& xmf3CameraOffset)
{
	m_xmf3CameraOffset = xmf3CameraOffset;
	m_pCamera->SetLookAt(Vector3::Add(m_xmf3Position, m_xmf3CameraOffset), m_xmf3Position, m_xmf3Up);
	m_pCamera->GenerateViewMatrix();
}

void CPlayer::Rotate(float fPitch, float fYaw, float fRoll)
{
	m_pCamera->Rotate(fPitch, fYaw, fRoll);
	if (fPitch != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(fPitch));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, mtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, mtxRotate);
	}
	if (fYaw != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(fYaw));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, mtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, mtxRotate);
	}
	if (fRoll != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(fRoll));
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, mtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, mtxRotate);
	}

	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::Normalize(Vector3::CrossProduct(m_xmf3Up, m_xmf3Look));
	m_xmf3Up = Vector3::Normalize(Vector3::CrossProduct(m_xmf3Look, m_xmf3Right));
}

void CPlayer::LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, xmf3Up);
	m_xmf3Right = Vector3::Normalize(XMFLOAT3(xmf4x4View._11, xmf4x4View._21, xmf4x4View._31));
	m_xmf3Up = Vector3::Normalize(XMFLOAT3(xmf4x4View._12, xmf4x4View._22, xmf4x4View._32));
	m_xmf3Look = Vector3::Normalize(XMFLOAT3(xmf4x4View._13, xmf4x4View._23, xmf4x4View._33));
}

void CPlayer::SetLook(XMFLOAT3& xmf3Look)
{
	m_xmf3Look = xmf3Look;
	m_xmf3Right = Vector3::Normalize(Vector3::CrossProduct(m_xmf3Up, m_xmf3Look));
	m_xmf3Up = Vector3::Normalize(Vector3::CrossProduct(m_xmf3Look, m_xmf3Right));
}

void CPlayer::Move(XMFLOAT3& xmf3Shift)
{
	m_xmf3Position = Vector3::Add(xmf3Shift, m_xmf3Position);
}


void CPlayer::Update(float fTimeElapsed)
{
	// set position()
	// rotate() 카메라 플레이어 포함
	m_pCamera->Update(this, m_xmf3Position, fTimeElapsed);
	m_pCamera->GenerateViewMatrix();
}

void CPlayer::MoveUpdate(PlayerState player, float fTimeElapsed)
{
	// set position()
	// rotate() 카메라 플레이어 포함
	XMFLOAT3 xmf3shift;
	xmf3shift.x = player.pos_x;
	xmf3shift.y = player.pos_y;
	xmf3shift.z = player.pos_z;
	Move(xmf3shift);
	m_pCamera->Move(xmf3shift);
}

void CPlayer::Animate(float fElapsedTime)
{
	OnUpdateTransform();

	CGameObject::Animate(fElapsedTime);
}

void CPlayer::OnUpdateTransform()
{
	m_xmf4x4World._11 = m_xmf3Right.x; m_xmf4x4World._12 = m_xmf3Right.y; m_xmf4x4World._13 = m_xmf3Right.z;
	m_xmf4x4World._21 = m_xmf3Up.x; m_xmf4x4World._22 = m_xmf3Up.y; m_xmf4x4World._23 = m_xmf3Up.z;
	m_xmf4x4World._31 = m_xmf3Look.x; m_xmf4x4World._32 = m_xmf3Look.y; m_xmf4x4World._33 = m_xmf3Look.z;
	m_xmf4x4World._41 = m_xmf3Position.x; m_xmf4x4World._42 = m_xmf3Position.y; m_xmf4x4World._43 = m_xmf3Position.z;
	UpdateBoundingBox();
}

void CPlayer::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	CGameObject::Render(hDCFrameBuffer, pCamera);
}


/////////////////////////////////////////////////////////////////////////////////////////////
//
CTankPlayer::CTankPlayer()
{
	CCubeMesh* pBulletMesh = new CCubeMesh(1.0f, 4.0f, 1.0f);
	for (int i = 0; i < BULLETS; i++)
	{
		m_ppBullets[i] = new CBulletObject(m_fBulletEffectiveRange);
		m_ppBullets[i]->SetMesh(pBulletMesh);
		m_ppBullets[i]->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_ppBullets[i]->SetRotationSpeed(360.0f);
		m_ppBullets[i]->SetMovingSpeed(500.0f);
		m_ppBullets[i]->SetActive(false);
	}

	CWallMesh* pShieldMesh = new CWallMesh(20.0f, -40.0f, 20.0f, 20);
	m_pShieldObject = new CShieldObject();
	m_pShieldObject->SetMesh(pShieldMesh);
	m_pShieldObject->SetColor(RGB(0, 1, 0));
	m_pShieldObject->SetActive(false);
}

CTankPlayer::~CTankPlayer()
{

}

void CTankPlayer::Animate(float fElapsedTime)
{
	CPlayer::Animate(fElapsedTime);

	for (int i = 0; i < BULLETS; i++)
	{
		if (m_ppBullets[i]->m_bActive) m_ppBullets[i]->Animate(fElapsedTime);
	}

	if (m_pShieldObject->m_bActive)
	{
		// 남은 시간 감소
		m_fShieldRemainTime -= fElapsedTime;

		if (m_fShieldRemainTime <= 0.0f)
		{
			m_fShieldRemainTime = 3.0f;
			m_pShieldObject->SetActive(false); // 자동으로 쉴드 OFF
		}
		else
		{
			m_pShieldObject->Animate(fElapsedTime);
		}
	}
}

void CTankPlayer::OnUpdateTransform()
{
	CPlayer::OnUpdateTransform();

	m_xmf4x4World = Matrix4x4::Multiply(XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f), m_xmf4x4World);
}

void CTankPlayer::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	CPlayer::Render(hDCFrameBuffer, pCamera);

	if(m_pShieldObject->m_bActive)
		m_pShieldObject->Render(hDCFrameBuffer, pCamera);

	for (int i = 0; i < BULLETS; i++) if (m_ppBullets[i]->m_bActive) m_ppBullets[i]->Render(hDCFrameBuffer, pCamera);

}

void CTankPlayer::FireBullet()
{

	CBulletObject* pBulletObject = NULL;
	for (int i = 0; i < BULLETS; i++)
	{
		if (!m_ppBullets[i]->m_bActive)
		{
			pBulletObject = m_ppBullets[i];
			break;
		}
	}

	if (!pBulletObject) return;

	// 발사 위치/방향
	XMFLOAT3 origin = GetPosition();
	XMFLOAT3 dir = GetUp(); // 현재 코드에서 Up이 포신 방향
	XMVECTOR vOrigin = XMLoadFloat3(&origin);
	XMVECTOR vDir = XMVector3Normalize(XMLoadFloat3(&dir));

	float nearestDist = FLT_MAX;
	bool  hitFound = false;

	// 다른 플레이어와 OOBB 교차 검사
	if (s_ppPlayers)
	{
		for (int i = 0; i < s_nPlayers; ++i)
		{
			CPlayer* other = s_ppPlayers[i];
			if (!other || other == this) continue;
			if ( other->hp == DEAD_PLAYER ) continue;

			// OOBB가 업데이트 되어 있다고 가정(매 프레임 Animate에서 Transform 반영)
			float dist = 0.0f;
			if (other->m_xmOOBB.Intersects(vOrigin, vDir, dist))
			{
				if (dist >= 0.0f && dist < nearestDist)
				{
					nearestDist = dist;
					hitFound = true;
				}
			}
		}
	}

	// 충돌 없으면 기존 사거리 사용, 있으면 충돌 거리 사용
	float travelLimit = hitFound ? nearestDist : m_fBulletEffectiveRange;

	// 총알 초기화
	XMFLOAT3 firePos = Vector3::Add(origin, Vector3::ScalarProduct(dir, 6.0f, false));
	pBulletObject->m_xmf4x4World = m_xmf4x4World;
	pBulletObject->SetFirePosition(firePos);
	pBulletObject->SetMovingDirection(dir);
	pBulletObject->SetColor(RGB(255, 0, 0));
	pBulletObject->m_fMovingDistance = 0.0f;
	pBulletObject->m_bExploding = false;
	pBulletObject->SetTravelLimit(travelLimit, hitFound);
	pBulletObject->SetActive(true);
}
