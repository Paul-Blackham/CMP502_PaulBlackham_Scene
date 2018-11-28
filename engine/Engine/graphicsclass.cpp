////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_LightShader = 0;
	m_LightShader2 = 0;
	m_Light = 0;
	m_RenderTexture = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;
	xRotation = 0.0f;
	yRotation = 0.0f;
	zRotation = 0.0f;


	// Create the Direct3D object.
	m_D3D = new D3DClass;
	if(!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	
	// Create the model object.
	m_Model = new ModelClass;
	if(!m_Model)
	{
		return false;
	}

	// Initialize the model object.
	result = m_Model->Initialize(m_D3D->GetDevice(), "../Engine/data/Cube.obj", L"../Engine/data/seafloor.dds");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Create the floor model object.
	m_floorModel = new ModelClass;
	if (!m_floorModel)
	{
		return false;
	}

	// Initialize the model object.
	result = m_floorModel->Initialize(m_D3D->GetDevice(), "../Engine/data/Floor.obj", L"../Engine/data/seafloor.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	

	// Create the environment model object.
	m_environmentModel = new ModelClass;
	if (!m_environmentModel)
	{
		return false;
	}

	// Initialize the model object.
	result = m_environmentModel->Initialize(m_D3D->GetDevice(), "../Engine/data/Environment.obj", L"../Engine/data/seafloor.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Create the light shader object.
	m_LightShader = new LightShaderClass;
	if(!m_LightShader)
	{
		return false;
	}

	// Initialize the light shader object.
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the light shader 2 object. ( new object for debug window render)
	m_LightShader2 = new LightShaderClass;
	if (!m_LightShader2)
	{
		return false;
	}

	// Initialize the light shader object.
	result = m_LightShader2->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the light object.
	m_Light = new LightClass;
	if(!m_Light)
	{
		return false;
	}

	// Initialize the light object.
	m_Light->SetAmbientColor(0.3f, 0.3f, 0.3f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(10.0f);

	// Create the render to texture object.
	m_RenderTexture = new RenderTextureClass;
	if (!m_RenderTexture)
	{
		return false;
	}

	// Initialize the render to texture object.
	result = m_RenderTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	if (!result)
	{
		return false;
	}

	return true;
}




void GraphicsClass::Shutdown()
{

	// Release the render to texture object.
	if (m_RenderTexture)
	{
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	// Release the light object.
	if(m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the light shader object.
	if(m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// Release the light shader 2 object.
	if (m_LightShader2)
	{
		m_LightShader2->Shutdown();
		delete m_LightShader2;
		m_LightShader2 = 0;
	}

	// Release the model object.
	if(m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the D3D object.
	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	return;
}


bool GraphicsClass::Frame(float zoom, float rotateX, float rotateY)
{
	bool result;
	static float rotation = 0.0f;
	static float delta =0.0f;


	// Update the rotation variable each frame.
	rotation += (float)D3DX_PI * 0.01f;
	if(rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	// Update the delta variable each frame. (keep this between 0 and 1)
	delta = 1.0f;
	
	// Render the graphics scene.
	result = Render(rotation, delta);
	if(!result)
	{
		return false;
	}

	ProcessCameraMovement(zoom, rotateX, rotateY);
	ProcessCameraRotation(zoom, rotateX, rotateY);

	return true;
}

void GraphicsClass::ProcessCameraMovement(float zoom, float rotateX, float rotateY) // accessed by system class
{
	// FIRST ZOOM

	D3DXVECTOR3 currentPosition = m_Camera->GetPosition();

	//normalize camera position
	float length = sqrt(currentPosition.x * currentPosition.x + currentPosition.y * currentPosition.y + currentPosition.z * currentPosition.z);
	//calculate distance camera will need to move in each axis to zoom towards origin - integrated into normalization
	float dist = length * zoom;

	float normalX = 0.0f;
	float normalY = 0.0f;
	float normalZ = 0.0f;

	if (dist != 0.0f) {
		normalX = currentPosition.x * dist;
		normalY = currentPosition.y * dist;
		normalZ = currentPosition.z * dist;
	}

	float positionX = currentPosition.x + normalX;
	float positionY = currentPosition.y + normalY;
	float positionZ = currentPosition.z + normalZ;

	/*if (positionX > 20.0f) {
		positionX = 20.0f;
	}

	if (positionX < -20.0f) {
		positionX = -20.0f;
	}

	if (positionY > 20.0f) {
		positionY = 20.0f;
	}
	if (positionY < -20.0f) {
		positionY = -20.0f;
	}

	if (positionZ > 20.0f) {
		positionZ = 20.0f;
	}
	if (positionZ < -20.0f) {
		positionZ = -20.0f;
	}*/

	xRotation += rotateX;
	yRotation += rotateY;
	//zRotation += rotateX;

	float xZunit2Dvec = sqrt(currentPosition.x * currentPosition.x + currentPosition.z * currentPosition.z);
	int xMult = 1;
	int yMult = 1;
	int zMult = 1;


	if (positionX < 0) {
		xMult = -1;
	}
	if (positionY < 0) {
		yMult = -1;
	}
	if (positionZ < 0) {
		zMult = -1;
	}

	float yUnit2Dvec = sqrt(xZunit2Dvec * xZunit2Dvec + positionY * positionY);

	m_Camera->SetPosition( sin(xRotation) * xZunit2Dvec, sin(yRotation) * yUnit2Dvec, cos(xRotation) * xZunit2Dvec * -1.0f);
}

void GraphicsClass::ProcessCameraRotation(float zoom, float rotateX, float rotateY){

	D3DXVECTOR3 newPosition = m_Camera->GetPosition();
	float toDegrees = 57.2958f;
	float toRadians = 0.0174533f;
	float rotAddYaw = 0.0f; // for different quadrents of rotation
	float rotAddPitch = 0.0f; // for different quadrents of rotation
	float xZunit2Dvec = sqrt(newPosition.x * newPosition.x + newPosition.z * newPosition.z);


	m_Camera->SetRotation(yRotation * toDegrees, -xRotation * toDegrees, 0.0f);


	//// Process Yaw (Y Rotation)
	//if (newPosition.z < 0.0f) {
	//	rotAddYaw = 0.0f;
	//}
	//else if (newPosition.z > 0.0f) {
	//	rotAddYaw = -180.0f;
	//}
	//
	//if (newPosition.z == 0.0f) {
	//	//m_Camera->SetRotation(0.0f, (90 * toRadians - atan(newPosition.z / newPosition.x)) * toDegrees, 0.0f); //atan(yUnit2Dvec / newPosition.y) * 57.2958f
	//}
	//else {
	//	m_Camera->SetRotation(0.0f, rotAddYaw + (atan(newPosition.x / newPosition.z) * toDegrees), 0.0f); //atan(yUnit2Dvec / newPosition.y) * 57.2958f
	//}

	//D3DXVECTOR3 newRotation = m_Camera->GetRotation();

	//// Process Pitch (local X Rotation)
	//if (newPosition.z < 0.0f) {
	//	xZunit2Dvec = -xZunit2Dvec;
	//	rotAddPitch = 0.0f;
	//}
	//else if (newPosition.z > 0.0f) {
	//	xZunit2Dvec = -xZunit2Dvec;
	//	rotAddPitch = -180.0f;
	//}

	//if (newPosition.y == 0.0f) {
	//	//m_Camera->SetRotation(0.0f, (90 * toRadians - atan(newPosition.z / newPosition.x)) * toDegrees, 0.0f); //atan(yUnit2Dvec / newPosition.y) * 57.2958f
	//}
	//else {
	//	m_Camera->SetRotation(-(rotAddPitch + (atan(newPosition.y / xZunit2Dvec) * toDegrees)), newRotation.y , 0.0f); //atan(yUnit2Dvec / newPosition.y) * 57.2958f
	//}

	return;
}


bool GraphicsClass::Render(float rotation, float deltavalue)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// Render the entire scene to the texture first.
	result = RenderToTexture();
	if (!result)
	{
		return false;
	}

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Render the scene as normal to the back buffer.
	result = RenderScene();
	if (!result)
	{
		return false;
	}

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}

bool GraphicsClass::RenderToTexture()
{
	bool result;


	// Set the render target to be the render to texture.
	m_RenderTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// Clear the render to texture.
	m_RenderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 1.0f, 1.0f);

	// Render the scene now and it will draw to the render to texture instead of the back buffer.
	result = RenderScene();
	if (!result)
	{
		return false;
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();

	return true;
}

bool GraphicsClass::RenderScene()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	bool result;
	static float rotation = 0.0f;


	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	// Update the rotation variable each frame.
	/*rotation += (float)D3DX_PI * 0.005f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}*/

	//D3DXMatrixRotationY(&worldMatrix, rotation);
	
	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->Render(m_D3D -> GetDeviceContext());

	D3DXMatrixTranslation(&worldMatrix, 0.0f, 0.0f, 0.0f);

	// Render the model using the light shader.
	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Light->GetAmbientColor(), 1.0f, m_Model->GetTexture(),
		m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	if (!result)
	{
		return false;
	}

	m_floorModel->Render(m_D3D->GetDeviceContext());

	D3DXMatrixTranslation(&worldMatrix, 0.0f, -5.0f, 0.0f);
	//
	// Render the model using the light shader.
	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Light->GetAmbientColor(), 1.0f, m_Model->GetTexture(),
		m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	if (!result)
	{
		return false;
	}

	m_environmentModel->Render(m_D3D->GetDeviceContext());

	D3DXMatrixTranslation(&worldMatrix, 0.0f, -5.0f, 0.0f);
	//
	// Render the model using the light shader.
	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Light->GetAmbientColor(), 1.0f, m_Model->GetTexture(),
		m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	if (!result)
	{
		return false;
	}

	return true;
}