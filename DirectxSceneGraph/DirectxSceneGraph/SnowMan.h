#pragma once
#include <d3dx9.h>
#include "d3dUtility.h"
class SnowMan {
public:
	SnowMan(IDirect3DDevice9 *device);

	~SnowMan();

	bool Init();

	void CleanUp();

	void Draw(const D3DXMATRIX _world);

private:
	ID3DXMesh *_objects[7];
	D3DXMATRIX  _worldTranslations[7];
	D3DMATERIAL9 _materials[7];
	
	IDirect3DDevice9 *_device;
	IDirect3DTexture9 *_tex;
	IDirect3DVertexBuffer9 *_vbo;
	IDirect3DIndexBuffer9 *_ibo;
};