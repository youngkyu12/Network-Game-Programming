#include "GameObject.h"

GameObject::GameObject()
{
}

GameObject::~GameObject()
{
}

void GameObject::UpdateBoundingBox()
{
	// 로컬 기준(센터=0, 방향=단위 쿼터니언, 크기=3,3,3)을 템플릿으로 사용
	const BoundingOrientedBox localBox(
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(10.0f, 5.0f, 2.0f),
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	BoundingOrientedBox worldBox;
	localBox.Transform(worldBox, XMLoadFloat4x4(&m_xmf4x4World));
	// 필요 시 정규화(안전용)
	XMStoreFloat4(&worldBox.Orientation,
		XMQuaternionNormalize(XMLoadFloat4(&worldBox.Orientation)));
	m_xmOOBB = worldBox;
}

void GameObject::SetBoundingBox(const XMFLOAT3& center, const XMFLOAT3& extents, const XMFLOAT4& orientation)
{
	m_xmOOBB = BoundingOrientedBox(center, extents, orientation);
}

void GameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}
