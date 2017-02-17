#pragma once
/*
 * 天空盒类，负责天空盒的创建
 */

#include <d3dx9.h>
#include "d3dUtility.h"

class CSkyBox
{
public:
    CSkyBox(IDirect3DDevice9* pDevice);
    ~CSkyBox(void);

    bool InitSkyBox(int scale);

    void Render();

    bool SetTexture(const TCHAR* textureFile, int flag);

private:

	struct TVertex
	{
		TVertex() {}
		TVertex(float x, float y, float z,
			float u, float v):_x(x),_y(y),_z(z),_u(u),_v(v)
		{}
		float _x, _y, _z;
		float _u, _v; // 纹理坐标

		static const DWORD FVF = D3DFVF_XYZ | D3DFVF_TEX1;
	};

    IDirect3DDevice9*       _d3dDevice;
    IDirect3DVertexBuffer9* _vb;
    IDirect3DIndexBuffer9*  _ib;
    IDirect3DTexture9*      _texture[5];
};


