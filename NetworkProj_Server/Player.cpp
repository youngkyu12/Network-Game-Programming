#include "Player.h"

Player::Player()
{

}
Player::~Player()
{

}

void Player::SetPosition(float x, float y, float z)
{
	Position = XMFLOAT3(x, y, z);
}

XMFLOAT3 Player::GetPosition()
{
	return Position;
}

XMFLOAT3 Player::GetVelocity()
{
	return Velocity;
}

XMFLOAT3 Player::GetLook()
{
	return Look;
}

XMFLOAT3 Player::GetUp()
{
	return Up;
}

XMFLOAT3 Player::GetRight()
{
	return Right;
}

uint16_t Player::GetHP()
{
	return HP;
}


void Player::Move(XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{
	if (bUpdateVelocity)
	{
		Velocity = Vector3::Add(Velocity, xmf3Shift);
	}
	else
	{
		Position = Vector3::Add(xmf3Shift, Position);
	}
}

void Player::Move(float x, float y, float z)
{
	Move(XMFLOAT3(x, y, z), false);
}

void Player::Rotate(float fPitch, float fYaw, float fRoll)
{
	if (fPitch != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&Right), XMConvertToRadians(fPitch));
		Look = Vector3::TransformNormal(Look, mtxRotate);
		Up = Vector3::TransformNormal(Up, mtxRotate);
	}
	if (fYaw != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&Up), XMConvertToRadians(fYaw));
		Look = Vector3::TransformNormal(Look, mtxRotate);
		Right = Vector3::TransformNormal(Right, mtxRotate);
	}
	if (fRoll != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&Look), XMConvertToRadians(fRoll));
		Up = Vector3::TransformNormal(Up, mtxRotate);
		Right = Vector3::TransformNormal(Right, mtxRotate);
	}

	Look = Vector3::Normalize(Look);
	Right = Vector3::Normalize(Vector3::CrossProduct(Up, Look));
	Up = Vector3::Normalize(Vector3::CrossProduct(Look, Right));
}

void Player::Update(float fTimeElapsed)
{
	Move(Velocity, false);

	XMFLOAT3 xmf3Deceleration = Vector3::Normalize(Vector3::ScalarProduct(Velocity, -1.0f));
	float fLength = Vector3::Length(Velocity);
	float fDeceleration = Friction * fTimeElapsed;
	if (fDeceleration > fLength) fDeceleration = fLength;
	Velocity = Vector3::Add(Velocity, xmf3Deceleration, fDeceleration);
}