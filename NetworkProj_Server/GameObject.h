#pragma once
#include "pch.h"

class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

public:
	void SetPosition(float x, float y, float z);
	void SetBoundingBox(const XMFLOAT3& center, const XMFLOAT3& extents, const XMFLOAT4& orientation);
	void UpdateBoundingBox();
	const BoundingOrientedBox& GetBoundingBox() const { return m_xmOOBB; }

private:
	XMFLOAT4X4	m_xmf4x4World = Matrix4x4::Identity();
	BoundingOrientedBox m_xmOOBB = BoundingOrientedBox();
};

