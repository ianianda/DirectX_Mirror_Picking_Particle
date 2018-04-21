#pragma once

#include <string>
#include "Basics.h"
#include "Frame.h"
#include "EpParticleSystem.h"

#define C_INFINITY FLT_MAX
#define EPSILON 0.001

const D3DXCOLOR      WHITE(D3DCOLOR_XRGB(255, 255, 255));
const D3DXCOLOR      BLACK(D3DCOLOR_XRGB(0, 0, 0));

struct Vertex
{
	Vertex() {}
	Vertex(float x, float y, float z,
		float nx, float ny, float nz,
		float u, float v)
	{
		_x = x;  _y = y;  _z = z;
		_nx = nx; _ny = ny; _nz = nz;
		_u = u;  _v = v;
	}
	float _x, _y, _z;
	float _nx, _ny, _nz;
	float _u, _v;

	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
};

struct BoundingBox
{
	BoundingBox();

	bool isPointInside(D3DXVECTOR3& p);

	D3DXVECTOR3 _min;
	D3DXVECTOR3 _max;
};

struct BoundingSphere
{
	BoundingSphere();

	D3DXVECTOR3 _center;
	float       _radius;
};

struct Ray
{
	D3DXVECTOR3 _origin;
	D3DXVECTOR3 _direction;
};

/*
Game class
*/
class Game
{
public:
	Game();
	Game(HWND hwnd);
	~Game();
	int GameInit();
	int GameLoop();
	int GameShutdown();
	int Render();
	int LoadBitmapToSurface(char* PathName, LPDIRECT3DSURFACE9* ppSurface, LPDIRECT3DDEVICE9 pDevice);
	int InitDirect3DDevice(HWND hWndTarget, int Width, int Height, BOOL bWindowed, D3DFORMAT FullScreenFormat, LPDIRECT3D9 pD3D, LPDIRECT3DDEVICE9* ppDevice);
	VOID Cleanup();
	VOID SetupMatrices();

	void moveCamera(float, float, float);
	void rotateCameraX(float);
	void rotateCameraY(float);
	void rotateCameraZ(float);
	void moveObject(int, float, float, float);
	void rotateObjectX(int, float);
	void rotateObjectY(int, float);
	void rotateObjectZ(int, float);
	bool getCameraMove();
	bool getObj1Move();
	bool getObj2Move();
	void setCameraMove(bool);
	void setObj1Move(bool);
	void setObj2Move(bool);
	void SetLightingDirectional();
	void SetLightingAmbient();
	void SetLightingSpot();
	void SetLightingPoint();

	//Assignment 3 methods
	D3DMATERIAL9 InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p);
	bool Setup();
	void RenderMirror(D3DXPLANE plane, int streamOffset);
	void drawObject(D3DXMATRIX W, int objectIndex, int reflection);
	Ray CalcPickingRay(int x, int y);
	void TransformRay(Ray* ray, D3DXMATRIX* T);
	float* RaySphereIntTest(Ray* ray, BoundingSphere* sphere);
	bool SetupPicking();
	void selectObj(int index);
	bool selectCamera();
	void setCameraSelected();

	//Assignment 3 variables
	LPDIRECT3DDEVICE9 g_pDevice = 0;//graphics device
	IDirect3DTexture9* MirrorTex;
	D3DMATERIAL9 MirrorMtrl;
	IDirect3DVertexBuffer9* VB;
	D3DXVECTOR3 reflectedPosition;
	BoundingSphere* BSphere;
	ID3DXMesh** Sphere;
	int numOfObject;
	int numOfMeshFile;
	bool cameraSelected = true;
	bool* objSelected;

private:
	LPDIRECT3D9 g_pD3D = 0;//COM object
	HWND g_hWndMain;//handle to main window
	Frame f;
	int frame = 0;
	int display = 0;
	float fps = 0;
	ID3DXFont *font;
	bool countStart = false;
	bool surfaceToken = false;
	LPDIRECT3DSURFACE9 copySurface;
	LPDIRECT3DSURFACE9 pSurface = 0;
	D3DXMATRIX* matObj;
	D3DXMATRIX matView;
	D3DMATERIAL9 material;
	D3DLIGHT9 light;
	int DeviceHeight, DeviceWidth;

	bool CameraMove = true; // default move camera
	bool Obj1Move = false;
	bool Obj2Move = false;

	LPD3DXMESH*              g_pMesh = NULL; // Our mesh object in sysmem
	D3DMATERIAL9**           g_pMeshMaterials = NULL; // Materials for our mesh
	LPDIRECT3DTEXTURE9**     g_pMeshTextures = NULL; // Textures for our mesh
	DWORD*	                 g_dwNumMaterials = 0L;   // Number of mesh materials

	D3DXMATRIX g_ProjMat;
	D3DXMATRIX g_ViewMat;
	LPDIRECT3DTEXTURE9 g_Texture = NULL;
	EpParticleSystem *g_Firework = NULL;
	float g_LastTime = 0.0f;
	float g_Time = 0.0f;
};