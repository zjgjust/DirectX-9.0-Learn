//////////////////////////////////////////////////////////////////////////////////////////////////
//  Snowman Scene Graph
//  Author zjg
//  Date   2017-2-11          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"
#include "terrain.h"
#include "camera.h"
#include "fps.h"
#include "cube.h"
#include "SnowMan.h"
#include "SkyBox.h"
#include "pSystem.h"

#include <cstdlib>
#include <ctime>

#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0) 
#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)
//
// Globals
//

IDirect3DDevice9* Device = 0; 

const int Width  = 1024;
const int Height = 768;

//
// Mouse
//
bool IsMouseDown = false;
POINT LastMousePoint = { 0,0 };
POINT CurrentMousePoint = { 0,0 };

//
// sky box
//
CSkyBox *		  SkyBox = nullptr;

//
// snow
//
using namespace psys;
PSystem *Sno = nullptr;

//
//cube
//
Cube*              Box = nullptr;
IDirect3DTexture9* BoxTexture = nullptr;
float			   SelfRotateAngle = 0;
float			   GlobalRotateAngle = 0;

//
//terrain
//
Terrain* TheTerrain = nullptr;

//
// snow man
//
SnowMan* SnowManStatic = nullptr;
SnowMan* SnowManDynamic = nullptr;

//
// camera
//
Camera   TheCamera(Camera::LANDOBJECT);
typedef struct Boundary
{
	float minX;
	float maxX;
	float minZ;
	float maxZ;
} Boundary;
Boundary CameraBoundary;
bool IsFollowing = false;

//FPSCounter* FPS = 0;

//
// Framework Functions
//

void MouseDragFunc()
{
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(NULL, &mousePos);

	CurrentMousePoint.x = mousePos.x;
	CurrentMousePoint.y = mousePos.y;

	if (IsMouseDown)
	{
		int xDiff = CurrentMousePoint.x - LastMousePoint.x;
		int yDiff = CurrentMousePoint.y - LastMousePoint.y;

		if (xDiff != 0)
		{
			TheCamera.yaw(xDiff * -0.001);
		}

		if (yDiff != 0)
		{
			TheCamera.pitch(yDiff * 0.001);
		}
	}

	LastMousePoint.x = CurrentMousePoint.x;
	LastMousePoint.y = CurrentMousePoint.y;
}

bool LoadTexture(std::string fileName,IDirect3DTexture9 **tex)
{
	HRESULT hr = 0;

	hr = D3DXCreateTextureFromFile(
		Device,
		fileName.c_str(),
		tex);

	if (FAILED(hr))
		return false;

	return true;
}

bool Setup()
{
	// seed random number generator
	srand((unsigned int)time(0));

	//
	// Create Snow System.
	//
	d3d::BoundingBox boundingBox;
	boundingBox._min = D3DXVECTOR3(-70.0f, 0.0f, -70.0f);
	boundingBox._max = D3DXVECTOR3(70.0f, 70.0f, 70.0f);
	Sno = new Snow(&boundingBox, 5000);
	Sno->init(Device, "Data/Snow/snowflake.dds");
	//
	// Create the terrain.
	//
	D3DXVECTOR3 lightDirection(0.0f, 1.0f, 0.0f);
	TheTerrain = new Terrain(Device, "Data/Terrain/coastMountain64.raw", 64, 64, 10, 0.5f);
	TheTerrain->genTexture(&lightDirection);
	TheTerrain->getBoundarys(CameraBoundary.minX, CameraBoundary.maxX, CameraBoundary.minZ, CameraBoundary.maxZ);
	TheCamera.setBoundary(CameraBoundary.minX, CameraBoundary.maxX, CameraBoundary.minZ, CameraBoundary.maxZ);

	//
	// sky box
	//

	SkyBox = new CSkyBox(Device);
	if (!SkyBox->InitSkyBox(200))
	{
		::MessageBox(NULL, _T("Initialize SkyBox failed!"), 0, 0);
	}

	//
	// Create the cube
	//

	Box = new Cube(Device);
	if (!LoadTexture("Data/Box/crate.jpg", &BoxTexture))
	{
		::MessageBox(0, "Cube texture error.", 0, 0);
		::PostQuitMessage(0);
	}

	//
	// Create the snow mans
	//

	SnowManStatic = new SnowMan(Device);
	SnowManDynamic = new SnowMan(Device);

	//
	// Set texture filters.
	//

	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	//
	// Light
	//

 	D3DLIGHT9 light;
 	::ZeroMemory(&light, sizeof(light));
 	light.Type = D3DLIGHT_DIRECTIONAL;
 	light.Ambient = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
 	light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
 	light.Specular = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
 	light.Direction = D3DXVECTOR3(5.0f, -20.0f, 0.0f);
 	Device->SetLight(0, &light);
 	Device->LightEnable(0, true);
	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, false);

	//
	// Set projection matrix.
	//

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
			&proj,
			D3DX_PI * 0.25f, // 45 - degree
			(float)Width / (float)Height,
			1.0f,
			10000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	return true;
}

void Cleanup()
{
	d3d::Delete<Terrain*>(TheTerrain);
	
	//d3d::Delete<FPSCounter*>(FPS);
}

bool Display(float timeDelta)
{
	//
	// Update the scene:
	//

	if( Device )
	{

		if(KEYDOWN('Q'))
		{
			if (!IsFollowing)
			{
				IsFollowing = true;
				TheCamera.saveViewMatrix();
			}
		}

		if (KEYDOWN('E'))
		{
			if (IsFollowing)
			{
				IsFollowing = false;
				TheCamera.useLastViewMatrix();
			}
		}

		if (!IsFollowing)
		{

			if (::GetAsyncKeyState('W') & 0x8000f)
				TheCamera.walk(100.0f * timeDelta);

			if (::GetAsyncKeyState('S') & 0x8000f)
				TheCamera.walk(-100.0f * timeDelta);

			if (::GetAsyncKeyState('A') & 0x8000f)
				TheCamera.strafe(-100.0f * timeDelta);

			if (::GetAsyncKeyState('D') & 0x8000f)
				TheCamera.strafe(100.0f * timeDelta);

			if (::GetAsyncKeyState(VK_UP) & 0x8000f)
				TheCamera.pitch(1.0f * timeDelta);

			if (::GetAsyncKeyState(VK_DOWN) & 0x8000f)
				TheCamera.pitch(-1.0f * timeDelta);

			if (::GetAsyncKeyState(VK_LEFT) & 0x8000f)
				TheCamera.yaw(-1.0f * timeDelta);

			if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f)
				TheCamera.yaw(1.0f * timeDelta);

			if (::GetAsyncKeyState(VK_LBUTTON))
				IsMouseDown = true;
			else
				IsMouseDown = false;

			MouseDragFunc();
			D3DXVECTOR3 pos;
			TheCamera.getPosition(&pos);
			float height = TheTerrain->getHeight(pos.x, pos.z) * 0.2; // 降低高度
			pos.y = height + 5.0f; // add height because we're standing up
			TheCamera.setPosition(&pos);

		}

		//terrain matrix
		D3DXMATRIX I;
		D3DXMatrixIdentity(&I);

		//snowman static
		D3DXMATRIX snowMatrix;
		D3DXMatrixTranslation(&snowMatrix, 0.0, 7.0, 0.0);

		//box matrix
		SelfRotateAngle += /*0.001*/timeDelta * D3DX_PI / 2;
		
		if (SelfRotateAngle >= 2 * D3DX_PI) SelfRotateAngle = 0.0;
		
		D3DXMATRIX SelfRotateMatrix;
		
		D3DXMatrixRotationY(&SelfRotateMatrix, SelfRotateAngle);
		
		GlobalRotateAngle += /*0.001*/timeDelta * D3DX_PI / 2;
		
		if (GlobalRotateAngle >= 2 * D3DX_PI) GlobalRotateAngle = 0.0;

		D3DXMATRIX GlobalRotateMatrix;
		
		D3DXMatrixRotationY(&GlobalRotateMatrix, GlobalRotateAngle);
		
		D3DXMATRIX ScallMatrix;
		
		D3DXMatrixScaling(&ScallMatrix, 5, 5, 5);
		
		D3DXMATRIX TransMatrix;
		
		D3DXMatrixTranslation(&TransMatrix, 0.0, 20.0, 50.0);
		
		D3DXMATRIX BoxMatrix = SelfRotateMatrix * ScallMatrix * TransMatrix * GlobalRotateMatrix;

		if (IsFollowing)
		{
			D3DXVECTOR3 position(0.0, 0.0, 0.0);
			D3DXMATRIX tempMatrix = TransMatrix * GlobalRotateMatrix;
			D3DXVec3TransformCoord(&position, &position,&tempMatrix);
			D3DXVECTOR3 look = D3DXVECTOR3(0, 0, 0) - position;
			D3DXVECTOR3 up(0.0, 1.0, 0.0);
			TheCamera.setPosition(&position);
			TheCamera.setLook(look);
			TheCamera.setUp(up);
		}

		D3DXMATRIX V;
		TheCamera.getViewMatrix(&V);
		Device->SetTransform(D3DTS_VIEW, &V);

		//sky box matrix
		D3DXVECTOR3 cameraPosition;
		TheCamera.getPosition(&cameraPosition);
		D3DXMATRIX skyBoxTrasMatrix;
		D3DXMatrixTranslation(&skyBoxTrasMatrix, cameraPosition.x, cameraPosition.y, cameraPosition.z);

		//Snow Man
		D3DXMATRIX DynamicSnowManTransMatrix;
		D3DXMatrixTranslation(&DynamicSnowManTransMatrix, 0.0, 25.0, 50.0);
		DynamicSnowManTransMatrix = SelfRotateMatrix * DynamicSnowManTransMatrix * GlobalRotateMatrix;

		//更新雪花数据
		Sno->update(timeDelta);
		
		//
		// Draw the scene:
		//
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);
		Device->BeginScene();

		if (SkyBox != nullptr)
		{
			Device->SetTransform(D3DTS_WORLD, &skyBoxTrasMatrix);
			SkyBox->Render();
		}

		if (TheTerrain != nullptr)
			TheTerrain->draw(&I, false);

		if (SnowManStatic != nullptr)
			SnowManStatic->Draw(snowMatrix);

		if (Box != nullptr)
			Box->draw(&BoxMatrix, &d3d::WHITE_MTRL, BoxTexture);
		
		if (SnowManDynamic != nullptr)
			SnowManDynamic->Draw(DynamicSnowManTransMatrix);
		
		
		// order important, render snow last.
		Device->SetTransform(D3DTS_WORLD, &I);
		Sno->render();
		

		//if( FPS )
		//	FPS->render(0xffffffff, timeDelta);

		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	}
	return true;
}

//
// WndProc
//
LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
		
	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE )
			::DestroyWindow(hwnd);
		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

//
// WinMain
//
int WINAPI WinMain(HINSTANCE hinstance,
				   HINSTANCE prevInstance, 
				   PSTR cmdLine,
				   int showCmd)
{
	if(!d3d::InitD3D(hinstance,
		Width, Height, true, D3DDEVTYPE_HAL, &Device))
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}
		
	if(!Setup())
	{
		::MessageBox(0, "Setup() - FAILED", 0, 0);
		return 0;
	}

	d3d::EnterMsgLoop( Display );

	Cleanup();

	Device->Release();

	return 0;
}
