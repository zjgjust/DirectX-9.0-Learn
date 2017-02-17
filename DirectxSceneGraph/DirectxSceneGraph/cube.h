//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: cube.h
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Provides an interface to create and render a cube.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __cubeH__
#define __cubeH__

#include <d3dx9.h>
#include <string>

class Cube
{
public:
	Cube(IDirect3DDevice9* device);
	
	~Cube();

	bool draw(D3DXMATRIX* world = nullptr, const D3DMATERIAL9* mtrl = nullptr, IDirect3DTexture9* tex = nullptr);
private:

	struct Vertex
	{
		Vertex() {}
		Vertex(
			float x, float y, float z,
			float nx, float ny, float nz,
			float u, float v)
		{
			_x = x;  _y = y;  _z = z;
			_nx = nx; _ny = ny; _nz = nz;
			_u = u;  _v = v;
		}
		float _x, _y, _z;
		float _nx, _ny, _nz;
		float _u, _v; // texture coordinates
		static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
	};
	IDirect3DDevice9*       _device;
	IDirect3DTexture9*		_tex;
	IDirect3DVertexBuffer9* _vb;
	IDirect3DIndexBuffer9*  _ib;
};
#endif //__cubeH__