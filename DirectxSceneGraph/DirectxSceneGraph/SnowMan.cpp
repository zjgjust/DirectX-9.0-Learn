#include "SnowMan.h"

SnowMan::SnowMan(IDirect3DDevice9 *device)
{
	_device = device;
	if (!Init())
	{
		::MessageBox(0, "Snow init error.", 0, 0);
		::PostQuitMessage(0);
	}
}

SnowMan::~SnowMan()
{
	CleanUp();
}

bool SnowMan::Init()
{
	//
	// the meshs
	//

	// body
	D3DXCreateSphere(
		_device,
		3.0f, // radius
		30,   // slices
		30,   // stacks
		&_objects[0],
		0);

	//head
	D3DXCreateSphere(
		_device,
		2.0f,
		30,
		30,
		&_objects[1],
		0);

	//eye
	D3DXCreateSphere(
		_device,
		0.3f,
		20,
		20,
		&_objects[2],
		0);

	D3DXCreateSphere(
		_device,
		0.3f,
		20,
		20,
		&_objects[3],
		0);
	
	//nose
	D3DXCreateSphere(
		_device,
		0.4f,
		20,
		20,
		&_objects[4],
		0);

	//
	// setup the materials
	//
	_materials[0] = d3d::WHITE_MTRL;
	_materials[1] = d3d::WHITE_MTRL;
	_materials[2] = d3d::BLACK_MTRL;
	_materials[3] = d3d::BLACK_MTRL;
	_materials[4] = d3d::RED_MTRL;
	//
	// setup the world translations
	//
	D3DXMatrixTranslation(&_worldTranslations[0], 0.0f, 3.0f, 0.0f);
	D3DXMatrixTranslation(&_worldTranslations[1], 0.0f, 7.0f, 0.0f);
	D3DXMatrixTranslation(&_worldTranslations[2], -0.8f, 8.0f, -1.5f);
	D3DXMatrixTranslation(&_worldTranslations[3], 0.8f, 8.0f, -1.5f);
	D3DXMatrixTranslation(&_worldTranslations[4], 0.0f, 7.0f, -2.0f);
	return true;
}

void SnowMan::CleanUp()
{
	for (int i = 0; i < 5; i++)
		d3d::Release<ID3DXMesh*>(_objects[i]);
}

void SnowMan::Draw(const D3DXMATRIX _world)
{
	for (int i = 0; i < 5; i++)
	{
		_device->SetMaterial(&_materials[i]);
		D3DXMATRIX matrix = _worldTranslations[i] * _world;
		_device->SetTransform(D3DTS_WORLD, &matrix);
		_objects[i]->DrawSubset(0);
	}
	_device->SetTexture(0, 0);
}


