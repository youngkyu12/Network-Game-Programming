#include "Player.h"


Player::Player()
{

}
Player::~Player()
{

}

void Player::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}

void Player::SetLook(float x, float y, float z)
{
	XMFLOAT3 Look = XMFLOAT3(x, y, z);
	XMFLOAT3 Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	LookAt(Look, Up);
}


void Player::LookAt(XMFLOAT3& L, XMFLOAT3& U)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(GetPosition(), L, U);
	m_xmf4x4World._11 = xmf4x4View._11; m_xmf4x4World._12 = xmf4x4View._21; m_xmf4x4World._13 = xmf4x4View._31;
	m_xmf4x4World._21 = xmf4x4View._12; m_xmf4x4World._22 = xmf4x4View._22; m_xmf4x4World._23 = xmf4x4View._32;
	m_xmf4x4World._31 = xmf4x4View._13; m_xmf4x4World._32 = xmf4x4View._23; m_xmf4x4World._33 = xmf4x4View._33;
}

XMFLOAT3 Player::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

XMFLOAT3 Player::GetLook()
{
	XMFLOAT3 xmf3LookAt(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);
	xmf3LookAt = Vector3::Normalize(xmf3LookAt);
	return(xmf3LookAt);
}


uint16_t Player::GetHP()
{
	return HP;
}

uint8_t Player::GetFireFlag()
{
	return is_Firing;
}

uint8_t Player::GetMyState()
{
	return MyState;
}

void Player::SetMyState(uint8_t newState)
{
	MyState = newState;
}


void Player::CheckFireFlag()
{
	if (is_Firing == 1)
	{
		if (FireTimer.IsElapsed(100))
		{
			is_Firing = 0;
			FireTimer.Stop(); 
			cout << "1초 경과. 발사 상태 해제" << endl;
		}
	}
}

void Player::UpdateBoundingBox()
{
	// 로컬 기준(센터=0, 방향=단위 쿼터니언, 크기=3,3,3)을 템플릿으로 사용
	const BoundingOrientedBox localBox(
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(3.0f, 3.0f, 3.0f),
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	BoundingOrientedBox worldBox;
	localBox.Transform(worldBox, XMLoadFloat4x4(&m_xmf4x4World));

	// 필요 시 정규화(안전용)
	XMStoreFloat4(&worldBox.Orientation,
		XMQuaternionNormalize(XMLoadFloat4(&worldBox.Orientation)));

	m_xmOOBB = worldBox;
}

void Player::Move(XMFLOAT3& xmf3Shift)
{
	m_xmf4x4World._41 += xmf3Shift.x;
	m_xmf4x4World._42 += xmf3Shift.y;
	m_xmf4x4World._43 += xmf3Shift.z;
}

void Player::Fire()
{
	// 혹시 같은 패킷이 두번오면 타이머에 영향없게 retrun 처리
	if (is_Firing == 1)
	{
		return;
	}
	FireTimer.Start();
	is_Firing = 1;
	cout << "서버내 fireflag 1로 변경" << endl;
}

void Player::SetEmptyBoundingBox ()
{
	m_hasOOBB = false; // 논리적으로 비활성화
	// 필요 시 디버깅용으로만 유지(실제 충돌에 사용하지 않음)
	m_xmOOBB = BoundingOrientedBox ( XMFLOAT3 ( 0.0f , 0.0f , 0.0f ) ,
		XMFLOAT3 ( 0.0f , 0.0f , 0.0f ) ,
		XMFLOAT4 ( 0.0f , 0.0f , 0.0f , 1.0f ) );
}


void Player::SetBoundingBox ( const XMFLOAT3& center , const XMFLOAT3& extents , const XMFLOAT4& orientation )
{
	m_xmOOBB = BoundingOrientedBox ( center , extents , orientation );
	m_hasOOBB = true;
}


void Player::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationYawPitchRoll(fPitch, fYaw, fRoll);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

Timer::Timer()
{
	startTime = 0;
	isActive = false;
}

Timer::~Timer()
{
}
