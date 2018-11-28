////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "lightshaderclass.h"
#include "lightclass.h"
#include "rendertextureclass.h"


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(float, float, float);
	bool RenderToTexture();
	bool RenderScene();
	void ProcessCameraMovement(float, float, float);
	void ProcessCameraRotation(float, float, float);

private:
	bool Render(float, float);
	float xRotation = 0.0f;
	float yRotation = 0.0f;
	float zRotation = 0.0f;

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	ModelClass* m_floorModel;
	ModelClass* m_environmentModel;
	LightShaderClass* m_LightShader;
	LightShaderClass* m_LightShader2;
	LightClass* m_Light;
	RenderTextureClass* m_RenderTexture;
};

#endif