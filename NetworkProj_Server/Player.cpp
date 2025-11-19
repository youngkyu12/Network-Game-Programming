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

void Player::SetLook(float x, float y, float z)
{
	Look = XMFLOAT3(x, y, z);
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


void Player::Move(XMFLOAT3& xmf3Shift)
{

	Position = Vector3::Add(xmf3Shift, Position);
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