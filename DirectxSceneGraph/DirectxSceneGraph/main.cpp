//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: terrainDriver.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Renders a terrain and allows you to walk around it. 
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"
#include "terrain.h"
#include "camera.h"
#include "fps.h"
#include "cube.h"
#include "SnowMan.h"
#include "SkyBox.h"

//
// Globals
//

IDirect3DDevice9* Device = 0; 

const int Width  = 800;
const int Height = 600;

//
// sky box
//
CSkyBox *		  SkyBox = nullptr;

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

Camera   TheCamera(Camera::LANDOBJECT);

//FPSCounter* FPS = 0;

//
// Framework Functions
//

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
	//
	// Create the terrain.
	//

	D3DXVECTOR3 lightDirection(0.0f, 1.0f, 0.0f);
	
	TheTerrain = new Terrain(Device, "Data/Terrain/coastMountain64.raw", 64, 64, 10, 0.5f);
	
	TheTerrain->genTexture(&lightDirection);

	//
	// sky box
	//
	SkyBox = new CSkyBox(Device);
	if (!SkyBox->InitSkyBox(60))
	{
		::MessageBox(NULL, _T("Initialize SkyBox failed!"), 0, 0);
	}

	//
	// Create the cube
	//
	Box = new Cube(Device);
	
	if (!LoadTexture("Data/crate.jpg", &BoxTexture))
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
			1000.0f);
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
		if( ::GetAsyncKeyState('W') & 0x8000f )
			TheCamera.walk(100.0f * timeDelta);

		if (::GetAsyncKeyState('S') & 0x8000f)
			TheCamera.walk(-100.0f * timeDelta);

		if (::GetAsyncKeyState('A') & 0x8000f)
			TheCamera.strafe(-100.0f * timeDelta);

		if (::GetAsyncKeyState('D') & 0x8000f)
			TheCamera.strafe(100.0f * timeDelta);
		
		if( ::GetAsyncKeyState(VK_UP) & 0x8000f )
			TheCamera.pitch(1.0f * timeDelta);
		
		if( ::GetAsyncKeyState(VK_DOWN) & 0x8000f )
			TheCamera.pitch(-1.0f * timeDelta);
		
		if( ::GetAsyncKeyState(VK_LEFT) & 0x8000f )
			TheCamera.yaw(-1.0f * timeDelta);
		
		if( ::GetAsyncKeyState(VK_RIGHT) & 0x8000f )
			TheCamera.yaw(1.0f * timeDelta);

		D3DXVECTOR3 pos;
		TheCamera.getPosition(&pos);
		float height = TheTerrain->getHeight( pos.x, pos.z ) * 0.2; // ½µµÍ¸ß¶È
		pos.y = height + 5.0f; // add height because we're standing up
		TheCamera.setPosition(&pos);

		D3DXMATRIX V;
		TheCamera.getViewMatrix(&V);
		Device->SetTransform(D3DTS_VIEW, &V);

		//
		// Draw the scene:
		//
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);
		Device->BeginScene();

		//
		// Draw the skybox
		//
		D3DXVECTOR3 cameraPosition;
		TheCamera.getPosition(&cameraPosition);
		D3DXMATRIX skyBoxTrasMatrix;
		D3DXMatrixTranslation(&skyBoxTrasMatrix, cameraPosition.x, cameraPosition.y, cameraPosition.z);
		if (SkyBox != nullptr)
		{
			Device->SetTransform(D3DTS_WORLD, &skyBoxTrasMatrix);
			SkyBox->Render();
		}

		//
		// Draw the terrain
		//
		D3DXMATRIX I;
		D3DXMatrixIdentity(&I);

		if (TheTerrain != nullptr)
			TheTerrain->draw(&I, false);

		//
		// Draw Snow Man
		//
		D3DXMATRIX snowMatrix;
		D3DXMatrixTranslation(&snowMatrix, 0.0, 7.0, 0.0);
		if (SnowManStatic != nullptr)
			SnowManStatic->Draw(snowMatrix);

		//
		//Draw Box
		//
		SelfRotateAngle += 0.00005 * D3DX_PI;
		if (SelfRotateAngle >= 2 * D3DX_PI)
			SelfRotateAngle = 0.0;
		D3DXMATRIX SelfRotateMatrix;
		D3DXMatrixRotationY(&SelfRotateMatrix, SelfRotateAngle);
		GlobalRotateAngle += 0.00005 * D3DX_PI;
		if (GlobalRotateAngle >= 2 * D3DX_PI)
			GlobalRotateAngle = 0.0;
		D3DXMATRIX GlobalRotateMatrix;
		D3DXMatrixRotationY(&GlobalRotateMatrix, GlobalRotateAngle);

		D3DXMATRIX ScallMatrix;
		D3DXMatrixScaling(&ScallMatrix, 5, 5, 5);
		D3DXMATRIX TransMatrix;
		D3DXMatrixTranslation(&TransMatrix, 0.0, 20.0, 50.0);
		D3DXMATRIX InputMatrix = SelfRotateMatrix * ScallMatrix * TransMatrix * GlobalRotateMatrix;

		if (Box != nullptr)
			Box->draw(&InputMatrix, &d3d::WHITE_MTRL, BoxTexture);
		
		//Snow Man
		D3DXMATRIX STransMatrix;
		D3DXMatrixTranslation(&STransMatrix, 0.0, 25.0, 50.0);
		InputMatrix = SelfRotateMatrix * STransMatrix * GlobalRotateMatrix;
		if (SnowManDynamic != nullptr)
			SnowManDynamic->Draw(InputMatrix);
		
		
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

