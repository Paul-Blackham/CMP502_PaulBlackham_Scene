////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <fstream>

#include "textureclass.h"
using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
class ModelClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
	    D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct FaceType
	{
		int a, b, c;
		int at, bt, ct;
		int an, bn, cn;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*, char*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

	ID3D11ShaderResourceView* GetTexture();


private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);


	bool LoadModelTxt(char*);
	bool ReadObjVertCount(char*);
	bool ReadObjVertData(char*);
	void ReleaseModel();

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();


private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount, m_textureCount, m_normalCount, m_faceCount;
	bool isObj = false;
	bool isTxt = false;
	ModelType* m_model;
	FaceType* m_face;
	TextureClass* m_Texture;
};

#endif