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
	virtual void ReleaseShaderVariables() {}

	void SetMesh(CMesh* pMesh);
	void SetMesh(int nIndex, CMesh* pMesh);
	virtual void SetShader(CShader* pShader);

	virtual void Animate(float fTimeElapsed);
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

	//���� ��ü�� ���� x-��, y-��, z-�� �������� �̵��Ѵ�.
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);


protected:
	XMFLOAT4X4 m_xmf4x4World;

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

class CPlayerObject
{
private:
	int								m_nReferences = 0;

public:
	void AddRef();
	void Release();

public:
	CPlayerObject();
	virtual ~CPlayerObject();

public:
	char							m_pstrFrameName[64];

	int m_nSubMeshes;
	CMesh* m_pMesh = NULL;
	CShader* m_pShader = NULL;

	XMFLOAT4X4						m_xmf4x4Transform;
	XMFLOAT4X4						m_xmf4x4World;

	CPlayerObject* m_pParent = NULL;
	CPlayerObject* m_pChild = NULL;
	CPlayerObject* m_pSibling = NULL;

	void SetMesh(CMesh* pMesh);
	void SetShader(CShader* pShader);

	void SetChild(CPlayerObject* pChild, bool bReferenceUpdate = false);
	void SetSubMeshes(int nSubMeshes);

	virtual void BuildMaterials(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {}

	virtual void OnInitialize() {}
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);

	virtual void OnPrepareRender() {}
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);

	virtual void ReleaseUploadBuffers();

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetScale(float x, float y, float z);

	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
	void Rotate(XMFLOAT4* pxmf4Quaternion);

	CPlayerObject* GetParent() { return(m_pParent); }
	void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	CPlayerObject* FindFrame(char* pstrFrameName);

	UINT GetMeshType() { return((m_pMesh) ? m_pMesh->GetType() : 0); }

public:
	static void LoadMaterialsInfoFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile);
	static CMeshLoadInfo* LoadMeshInfoFromFile(FILE* pInFile);

	static CPlayerObject* LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, FILE* pInFile);
	static CPlayerObject* LoadGeometryFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, char* pstrFileName);

	static void PrintFrameInfo(CPlayerObject* pGameObject, CPlayerObject* pParent);
};