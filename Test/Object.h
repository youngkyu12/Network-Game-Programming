#pragma once
#include "Mesh.h"
#include "Camera.h"

class CShader;

class CGameObject {
public:
	CGameObject(int nMeshes = 1);
	virtual ~CGameObject();

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void ReleaseUploadBuffers();

	//��� ���۸� �����Ѵ�.
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {}

	//��� ������ ������ �����Ѵ�.
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);

	virtual void ReleaseShaderVariables() {}

	void SetMesh(CMesh* pMesh);
	void SetMesh(int nIndex, CMesh* pMesh);
	virtual void SetShader(CShader* pShader);

	void SetChild(CGameObject* pChild, bool bReferenceUpdate = false);

	virtual void OnInitialize() {}
	virtual void Animate(float fTimeElapsed);
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent);
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	// ���� ��ü�� �̵��� ȸ���� ó���ϱ� ���� ����

	//���� ��ü�� ���� ��ȯ ��Ŀ��� ��ġ ���Ϳ� ����(x-��, y-��, z-��) ���͸� ��ȯ�Ѵ�.
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	//���� ��ü�� ��ġ�� �����Ѵ�.
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetScale(float x, float y, float z);
	void SetNumSubMesh(int nSubMeshes) { m_nSubMeshes = nSubMeshes; }

	//���� ��ü�� ���� x-��, y-��, z-�� �������� �̵��Ѵ�.
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);

	static CMeshLoadInfo* LoadMeshInfoFromFile(FILE* pInFile);

	static CGameObject* LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, FILE* pInFile);
	static CGameObject* LoadGeometryFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, char* pstrFileName);

	static void PrintFrameInfo(CGameObject* pGameObject, CGameObject* pParent);

	CGameObject* GetParent() { return(m_pParent); }
	void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	CGameObject* FindFrame(char* pstrFrameName);

	UINT GetMeshType() { return((m_pMesh) ? m_pMesh->GetType() : 0); }

	XMFLOAT4X4						m_xmf4x4Transform;
	XMFLOAT4X4						m_xmf4x4World;

	CGameObject* m_pParent = NULL;
	CGameObject* m_pChild = NULL;
	CGameObject* m_pSibling = NULL;

	char							m_pstrFrameName[64];

	CMesh* m_pMesh = NULL;
	int m_nSubMeshes = 0;

protected:

	//���� ��ü�� ���� ���� �޽��� �����ϴ� ��� ���� ��ü�� ������ �޽��鿡 ���� �����Ϳ� �� �����̴�.
	CMesh** m_ppMeshes = NULL;
	int m_nMeshes = 0;
		
	CShader* m_pShader = NULL;

private:
	int m_nReferences = 0;
};

class CRotatingObject : public CGameObject {
public:
	CRotatingObject(int nMeshes = 1);
	virtual ~CRotatingObject();


	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }
	virtual void Animate(float fTimeElapsed);

private:
	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;
};

class CTankObject : public CGameObject
{
public:
	CTankObject() {}
	virtual ~CTankObject() {}

protected:
	CGameObject* m_pTurretFrame = NULL;
	CGameObject* m_pCannonFrame = NULL;
	CGameObject* m_pGunFrame = NULL;

public:
	virtual void OnInitialize() {}
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);
};

class CM26Object : public CTankObject
{
public:
	CM26Object() {}
	virtual ~CM26Object() {}

	virtual void OnInitialize();
};


class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, 
		LPCTSTR pFileName, 
		int	nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color);
	virtual ~CHeightMapTerrain();

private:
	//������ ���� ������ ����� �̹����̴�.
	CHeightMapImage* m_pHeightMapImage;
	
	//���� ���� ���ο� ���� ũ���̴�.
	int	m_nWidth;
	int m_nLength;
		
	//������ ������ �� �� Ȯ���� ���ΰ��� ��Ÿ���� ������ �����̴�.
	XMFLOAT3 m_xmf3Scale;
public:
	//������ ���̸� ����ϴ� �Լ��̴�(���� ��ǥ��). ���� ���� ���̿� �������� y�� ���� ���̴�.
	float GetHeight(float x, float z) {
		return((m_pHeightMapImage->GetHeight(x / m_xmf3Scale.x, z / m_xmf3Scale.z)) * m_xmf3Scale.y);
	}
	//������ ���� ���͸� ����ϴ� �Լ��̴�(���� ��ǥ��). ���� ���� ���� ���͸� ����Ѵ�.
	XMFLOAT3 GetNormal(float x, float z) {
		return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z)));
	}
	int GetHeightMapWidth() { return(m_pHeightMapImage->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMapImage->GetHeightMapLength()); }
	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	//������ ũ��(����/����)�� ��ȯ�Ѵ�. ���� ���� ũ�⿡ �������� ���� ���̴�.
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float GetLength() { return(m_nLength * m_xmf3Scale.z); }
};
