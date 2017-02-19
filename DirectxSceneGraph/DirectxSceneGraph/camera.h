#ifndef __cameraH__
#define __cameraH__

#include <d3dx9.h>

class Camera
{
public:
	enum CameraType { LANDOBJECT, AIRCRAFT };

	Camera();
	Camera(CameraType cameraType);
	~Camera();

	void strafe(float units); // left/right
	void fly(float units);    // up/down
	void walk(float units);   // forward/backward
	void pitch(float angle); // rotate on right vector
	void yaw(float angle);   // rotate on up vector
	void roll(float angle);  // rotate on look vector
	void getViewMatrix(D3DXMATRIX* V); 
	void setCameraType(CameraType cameraType); 
	void getPosition(D3DXVECTOR3* pos); 
	void setPosition(D3DXVECTOR3* pos); 
	void getRight(D3DXVECTOR3* right);
	void getUp(D3DXVECTOR3* up);
	void setUp(D3DXVECTOR3 up);
	void getLook(D3DXVECTOR3* look);
	void setLook(D3DXVECTOR3 look);
	void saveViewMatrix()
	{
		_lastPos = _pos;
		_lastLook = _look;
		_lastUp = _up;
		_lastRight = _right;
	}
	void useLastViewMatrix()
	{
		_pos = _lastPos;
		_look = _lastLook;
		_up = _lastUp;
		_right = _lastRight;
	}
	void setBoundary(float x_min, float x_max, float z_min, float z_max);

private:
	CameraType  _cameraType;
	D3DXVECTOR3 _right;
	D3DXVECTOR3 _up;
	D3DXVECTOR3 _look;
	D3DXVECTOR3 _pos;

	D3DXVECTOR3 _lastRight;
	D3DXVECTOR3 _lastUp;
	D3DXVECTOR3 _lastLook;
	D3DXVECTOR3 _lastPos;

	float _minX;
	float _maxX;
	float _minZ;
	float _maxZ;

};
#endif // __cameraH__