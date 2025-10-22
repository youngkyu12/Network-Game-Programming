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

	//상수 버퍼를 생성한다.
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {}

	//상수 버퍼의 내용을 갱신한다.
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

	// 게임 객체의 이동과 회전을 처리하기 위한 내용

	//게임 객체의 월드 변환 행렬에서 위치 벡터와 방향(x-축, y-축, z-축) 벡터를 반환한다.
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	//게임 객체의 위치를 설정한다.
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetScale(float x, float y, float z);
	void SetNumSubMesh(int nSubMeshes) { m_nSubMeshes = nSubMeshes; }

	//게임 객체를 로컬 x-축, y-축, z-축 방향으로 이동한다.
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

	//게임 객체는 여러 개의 메쉬를 포함하는 경우 게임 객체가 가지는 메쉬들에 대한 포인터와 그 개수이다.
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
	//지형의 높이 맵으로 사용할 이미지이다.
	CHeightMapImage* m_pHeightMapImage;
	
	//높이 맵의 가로와 세로 크기이다.
	int	m_nWidth;
	int m_nLength;
		
	//지형을 실제로 몇 배 확대할 것인가를 나타내는 스케일 벡터이다.
	XMFLOAT3 m_xmf3Scale;
public:
	//지형의 높이를 계산하는 함수이다(월드 좌표계). 높이 맵의 높이에 스케일의 y를 곱한 값이다.
	float GetHeight(float x, float z) {
		return((m_pHeightMapImage->GetHeight(x / m_xmf3Scale.x, z / m_xmf3Scale.z)) * m_xmf3Scale.y);
	}
	//지형의 법선 벡터를 계산하는 함수이다(월드 좌표계). 높이 맵의 법선 벡터를 사용한다.
	XMFLOAT3 GetNormal(float x, float z) {
		return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z)));
	}
	int GetHeightMapWidth() { return(m_pHeightMapImage->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMapImage->GetHeightMapLength()); }
	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	//지형의 크기(가로/세로)를 반환한다. 높이 맵의 크기에 스케일을 곱한 값이다.
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float GetLength() { return(m_nLength * m_xmf3Scale.z); }
};
