#include "Game.h"




Game::Game() {

}

/*
Constructor for Game Class
*/
Game::Game(HWND hwnd) {
	g_hWndMain = hwnd;
	font = NULL;
	MirrorTex = 0;
	MirrorMtrl = InitMtrl(WHITE, WHITE, WHITE, BLACK, 2.0F);
	VB = 0;
	reflectedPosition = D3DXVECTOR3(0, 0, -3);
	numOfObject = 4;
	numOfMeshFile = 2;
	matObj = new D3DXMATRIX[numOfObject];
	BSphere = new BoundingSphere[numOfObject];
	Sphere = new ID3DXMesh*[numOfObject];
	g_pMesh = new LPD3DXMESH[numOfMeshFile];					// Our mesh object in sysmem
	g_pMeshMaterials = new D3DMATERIAL9*[numOfMeshFile];		// Materials for our mesh
	g_pMeshTextures = new LPDIRECT3DTEXTURE9*[numOfMeshFile];	// Textures for our mesh
	g_dwNumMaterials = new unsigned long[numOfMeshFile];		// Number of mesh materials
	objSelected = new bool[numOfObject];
}

/*
Destructor for Game Class
*/
Game::~Game() {
	//pSurface->Release();
	pSurface = 0;
}

/*
Initializer for the game
*/
int Game::GameInit() {
	HRESULT r = 0;//return values
	LPD3DXBUFFER* pD3DXMtrlBuffer = new LPD3DXBUFFER[numOfMeshFile];
	D3DSURFACE_DESC desc;

	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);//COM object
	if (g_pD3D == NULL) {
		SetError("Could not create IDirect3D9 object");
		return E_FAIL;
	}

	r = InitDirect3DDevice(g_hWndMain, 1920, 1080, FALSE, D3DFMT_X8R8G8B8, g_pD3D, &g_pDevice);
	if (FAILED(r)) {//FAILED is a macro that returns false if return value is a failure - safer than using value itself
		SetError("Initialization of the device failed");
		return E_FAIL;
	}

	// This is where I setup the surface
	r = this->g_pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurface);
	if (FAILED(r)) {
		SetError(_T("Couldn't get backbuffer"));
	}
	pSurface->GetDesc(&desc);

	r = this->g_pDevice->CreateOffscreenPlainSurface(desc.Width, desc.Height, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &this->copySurface, NULL);

	r = LoadBitmapToSurface("baboom.png", &pSurface, this->g_pDevice);
	if (FAILED(r))
		SetError(_T("Error loading bitmap"));

	r = D3DXLoadSurfaceFromSurface(this->copySurface, NULL, NULL, pSurface, NULL, NULL, D3DX_FILTER_TRIANGLE, 0);
	if (FAILED(r))
		SetError(_T("did not copy surface"));

	// Load the mesh from the specified file
	if (FAILED(D3DXLoadMeshFromX("tiger.x", D3DXMESH_SYSTEMMEM,
		g_pDevice, NULL,
		&pD3DXMtrlBuffer[0], NULL, &g_dwNumMaterials[0],
		&g_pMesh[0])))
	{
		// If model is not in current folder, try parent folder
		if (FAILED(D3DXLoadMeshFromX("..\\tiger.x", D3DXMESH_SYSTEMMEM,
			g_pDevice, NULL,
			&pD3DXMtrlBuffer[0], NULL, &g_dwNumMaterials[0],
			&g_pMesh[0])))
		{
			MessageBox(NULL, "Could not find tiger.x", "Meshes.exe", MB_OK);
			return E_FAIL;
		}
	}

	// Load the mesh from the specified file
	if (FAILED(D3DXLoadMeshFromX("Monkey.x", D3DXMESH_SYSTEMMEM,
		g_pDevice, NULL,
		&pD3DXMtrlBuffer[1], NULL, &g_dwNumMaterials[1],
		&g_pMesh[1])))
	{
		// If model is not in current folder, try parent folder
		if (FAILED(D3DXLoadMeshFromX("..\\EvilDrone.x", D3DXMESH_SYSTEMMEM,
			g_pDevice, NULL,
			&pD3DXMtrlBuffer[1], NULL, &g_dwNumMaterials[1],
			&g_pMesh[1])))
		{
			MessageBox(NULL, "Could not find EvilDrone.x", "Meshes.exe", MB_OK);
			return E_FAIL;
		}
	}

	// We need to extract the material properties and texture names from the 
	// pD3DXMtrlBuffer
	D3DXMATERIAL** d3dxMaterials = new D3DXMATERIAL*[numOfMeshFile];
	d3dxMaterials[0] = (D3DXMATERIAL*)pD3DXMtrlBuffer[0]->GetBufferPointer();
	d3dxMaterials[1] = (D3DXMATERIAL*)pD3DXMtrlBuffer[1]->GetBufferPointer();
	g_pMeshMaterials[0] = new D3DMATERIAL9[g_dwNumMaterials[0]];
	g_pMeshMaterials[1] = new D3DMATERIAL9[g_dwNumMaterials[1]];
	g_pMeshTextures[0] = new LPDIRECT3DTEXTURE9[g_dwNumMaterials[0]];
	g_pMeshTextures[1] = new LPDIRECT3DTEXTURE9[g_dwNumMaterials[1]];

	for (DWORD i = 0; i<g_dwNumMaterials[0]; i++)
	{
		// Copy the material
		g_pMeshMaterials[0][i] = d3dxMaterials[0][i].MatD3D;

		// Set the ambient color for the material (D3DX does not do this)
		g_pMeshMaterials[0][i].Ambient = g_pMeshMaterials[0][i].Diffuse;

		g_pMeshTextures[0][i] = NULL;
		if (d3dxMaterials[0][i].pTextureFilename != NULL &&
			lstrlen(d3dxMaterials[0][i].pTextureFilename) > 0)
		{
			// Create the texture
			if (FAILED(D3DXCreateTextureFromFile(g_pDevice,
				d3dxMaterials[0][i].pTextureFilename,
				&g_pMeshTextures[0][i])))
			{
				// If texture is not in current folder, try parent folder
				const TCHAR* strPrefix = TEXT("..\\");
				const int lenPrefix = lstrlen(strPrefix);
				TCHAR strTexture[MAX_PATH];
				lstrcpyn(strTexture, strPrefix, MAX_PATH);
				lstrcpyn(strTexture + lenPrefix, d3dxMaterials[0][i].pTextureFilename, MAX_PATH - lenPrefix);
				// If texture is not in current folder, try parent folder
				if (FAILED(D3DXCreateTextureFromFile(g_pDevice,
					strTexture,
					&g_pMeshTextures[0][i])))
				{
					MessageBox(NULL, "Could not find texture map", "Meshes.exe", MB_OK);
				}
			}
		}
	}

	//Create normals for the tiger
	g_pMesh[0]->CloneMeshFVF(D3DXMESH_MANAGED, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, g_pDevice, &g_pMesh[0]);
	if (FAILED(D3DXComputeNormals(g_pMesh[0], NULL))) {
		return E_FAIL;
	}

	for (DWORD i = 0; i < g_dwNumMaterials[1]; i++)
	{
		// Copy the material
		g_pMeshMaterials[1][i] = d3dxMaterials[1][i].MatD3D;

		// Set the ambient color for the material (D3DX does not do this)
		g_pMeshMaterials[1][i].Ambient = g_pMeshMaterials[1][i].Diffuse;

		g_pMeshTextures[1][i] = NULL;
		if (d3dxMaterials[1][i].pTextureFilename != NULL &&
			lstrlen(d3dxMaterials[1][i].pTextureFilename) > 0)
		{
			// Create the texture
			if (FAILED(D3DXCreateTextureFromFile(g_pDevice,
				d3dxMaterials[1][i].pTextureFilename,
				&g_pMeshTextures[1][i])))
			{
				// If texture is not in current folder, try parent folder
				const TCHAR* strPrefix = TEXT("..\\");
				const int lenPrefix = lstrlen(strPrefix);
				TCHAR strTexture[MAX_PATH];
				lstrcpyn(strTexture, strPrefix, MAX_PATH);
				lstrcpyn(strTexture + lenPrefix, d3dxMaterials[1][i].pTextureFilename, MAX_PATH - lenPrefix);
				// If texture is not in current folder, try parent folder
				if (FAILED(D3DXCreateTextureFromFile(g_pDevice,
					strTexture,
					&g_pMeshTextures[1][i])))
				{
					MessageBox(NULL, "Could not find texture map", "Meshes.exe", MB_OK);
				}
			}
		}
	}

	// for loop to load in the objects
	Setup();
	SetupMatrices();
	D3DXMatrixTranslation(&matObj[0], 0, 0, -7.5f);
	D3DXMatrixTranslation(&matObj[1], 7.5f, 0, 2.5f);
	D3DXMatrixTranslation(&matObj[2], 0, 7.5f, 2.5f);
	D3DXMatrixTranslation(&matObj[3], -7.5f, 0, 2.5f);
	SetupPicking();

	// Done with the material buffer
	pD3DXMtrlBuffer[0]->Release();
	pD3DXMtrlBuffer[1]->Release();
	g_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	return S_OK;
}

/*
Looping function for game to keep rendering and calculating FPS
*/
int Game::GameLoop() {
	
	if (countStart == false) {
		f.StartCounter();
		countStart = true;
	}
	frame++;

	if (f.GetCounter() >= 1000) {
		display = frame;
		frame = 0;
		f.ResetCounter();
	}

	Render();

	if (GetAsyncKeyState(VK_ESCAPE))
		PostQuitMessage(0);

	return S_OK;
}

/*
Shuts the game down
*/
int Game::GameShutdown() {
	//release resources. First display adapter because COM object created it, then COM object
	if (g_pDevice)
		g_pDevice->Release();

	if (g_pD3D)
		g_pD3D->Release();

	return S_OK;
}

/*
Method for rendering the screen and image 
*/
int Game::Render() {
	HRESULT r;
	LPDIRECT3DSURFACE9 pBackSurf = 0;
	if (!g_pDevice) {
		SetError("Cannot render because there is no device");
		return E_FAIL;
	}
	g_Time = GetTickCount() * 0.001f;
	float interval = g_Time - g_LastTime;
	g_LastTime = g_Time;

	g_Firework->Update(interval);
	//clear the display arera with colour black, ignore stencil buffer
	g_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_ARGB(255, 0, 0, 25), 1.0f, 0);

	//get pointer to backbuffer
	r = g_pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackSurf);
	if (FAILED(r)) {
		SetError("Couldn't get backbuffer");
	}

	if (surfaceToken == false) {
		surfaceToken = true;
		r = LoadBitmapToSurface("baboon.bmp",
			&pSurface, g_pDevice);
		if (FAILED(r)) {
			SetError("could not load bitmap surface");
		}
		D3DSURFACE_DESC surfaceD;
		pBackSurf->GetDesc(&surfaceD);

		r = g_pDevice->CreateOffscreenPlainSurface(surfaceD.Width, surfaceD.Height, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &copySurface, NULL);
		r = D3DXLoadSurfaceFromSurface(copySurface, NULL, NULL, pSurface, NULL, NULL, D3DX_FILTER_TRIANGLE, 0);
		pSurface = copySurface;

		if (FAILED(r))
			SetError("did not copy surface");
	}
	else {
		r = g_pDevice->UpdateSurface(pSurface, NULL, pBackSurf, NULL);

		if (r != D3D_OK) {
		}
	}

	pBackSurf->Release();//release lock
	pBackSurf = 0;

	RECT rect = { 0, 0, 200, 200 };

	if (font == NULL)
	{
		HRESULT hr = D3DXCreateFont(g_pDevice, 40, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY, FF_DONTCARE, "Arial", &font);
	}


	g_pDevice->BeginScene();
	
	//g_pDevice->SetTransform(D3DTS_WORLD, &matObj1);
	//// Meshes are divided into subsets, one for each material. Render them in
	//// a loop
	//for (DWORD i = 0; i<g_dwNumMaterials; i++)
	//{
	//	// Set the material and texture for this subset
	//	g_pDevice->SetMaterial(&g_pMeshMaterials[i]);
	//	g_pDevice->SetTexture(0, g_pMeshTextures[i]);

	//	// Draw the mesh subset
	//	g_pMesh->DrawSubset(i);
	//}
	//g_pDevice->SetTransform(D3DTS_WORLD, &matObj2);
	//for (DWORD i = 0; i<g_dwNumMaterials2; i++)
	//{
	//	// Set the material and texture for this subset
	//	g_pDevice->SetMaterial(&g_pMeshMaterials2[i]);
	//	g_pDevice->SetTexture(0, g_pMeshTextures2[i]);

	//	// Draw the mesh subset
	//	g_pMesh2->DrawSubset(i);
	//}

	font->DrawTextA(NULL, std::to_string(display).c_str(), -1, &rect, DT_LEFT, D3DCOLOR_XRGB(255, 255, 255));

	drawObject(matObj[0], 0, 0);
	drawObject(matObj[1], 1, 0);
	drawObject(matObj[2], 0, 0);
	drawObject(matObj[3], 1, 0);

	/*D3DXVECTOR3 position(0, 3.0f, -20.f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &position, &target, &up);
	g_pDevice->SetTransform(D3DTS_VIEW, &V);*/
	D3DXMATRIX I;
	D3DXMatrixIdentity(&I);
	g_pDevice->SetTransform(D3DTS_WORLD, &I);
	g_pDevice->SetStreamSource(0, VB, 0, sizeof(Vertex));

	// draw the mirror
	g_pDevice->SetMaterial(&MirrorMtrl);
	g_pDevice->SetTexture(0, MirrorTex);
	g_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

	g_pDevice->SetMaterial(&MirrorMtrl);
	g_pDevice->SetTexture(0, MirrorTex);
	g_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 6, 2);

	g_pDevice->SetMaterial(&MirrorMtrl);
	g_pDevice->SetTexture(0, MirrorTex);
	g_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 12, 2);

	g_pDevice->SetMaterial(&MirrorMtrl);
	g_pDevice->SetTexture(0, MirrorTex);
	g_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 18, 2);

	g_pDevice->SetMaterial(&MirrorMtrl);
	g_pDevice->SetTexture(0, MirrorTex);
	g_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 24, 2);

	g_pDevice->SetMaterial(&MirrorMtrl);
	g_pDevice->SetTexture(0, MirrorTex);
	g_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 30, 2);

	D3DXPLANE plane(0.0f, 0.0f, 1.0f, 0.0f); // front xy plane
	int streamOffset = 0;
	RenderMirror(plane, streamOffset);
	plane = D3DXPLANE(0.0f, 1.0f, 0.0f, 0.0f); // bottom yz plane
	streamOffset += 6;
	RenderMirror(plane, streamOffset);
	plane = D3DXPLANE(0.0f, 6.0f, 0.0f, 0.0f); // top yz plane
	streamOffset += 6;
	RenderMirror(plane, streamOffset);
	plane = D3DXPLANE(0.0f, 0.0f, 6.0f, 0.0f); // back xy plane
	streamOffset += 6;
	RenderMirror(plane, streamOffset);
	plane = D3DXPLANE(3.5f, 0.0f, 0.0f, 0.0f); // right xz plane
	streamOffset += 6;
	RenderMirror(plane, streamOffset);
	plane = D3DXPLANE(-3.5f, 0.0f, 0.0f, 0.0f); // left xz plane
	streamOffset += 6;
	RenderMirror(plane, streamOffset);

	for (int i = 0; i < numOfObject; i++)
	{
		BSphere[i]._center = D3DXVECTOR3(matObj[i]._41, matObj[i]._42, matObj[i]._43);
	}
	g_Firework->Render();
	g_pDevice->EndScene();

	g_pDevice->SetTransform(D3DTS_VIEW, &matView);

	// Render the bounding sphere with alpha blending so we can see 
	// through it.
	//g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	//g_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//g_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//D3DMATERIAL9 blue = InitMtrl(WHITE, WHITE, WHITE, BLACK, 2.0f);
	//blue.Diffuse.a = 0.25f; // 25% opacity
	//g_pDevice->SetMaterial(&blue);
	//g_pDevice->SetTransform(D3DTS_WORLD, &matObj1);
	//Sphere->DrawSubset(0);

	g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

	// Present the backbuffer contents to the display
	g_pDevice->Present(NULL, NULL, NULL, NULL);

	return S_OK;
}

/*
Loads the bitmap to the surface
	@param PathName
	@param ppSurface
	@param pDevice
*/
int Game::LoadBitmapToSurface(char* PathName, LPDIRECT3DSURFACE9* ppSurface, LPDIRECT3DDEVICE9 pDevice) {
	HRESULT r;
	HBITMAP hBitmap;
	BITMAP Bitmap;

	hBitmap = (HBITMAP)LoadImage(NULL, PathName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	if (hBitmap == NULL) {
		SetError("Unable to load bitmap");
		return E_FAIL;
	}

	GetObject(hBitmap, sizeof(BITMAP), &Bitmap);
	DeleteObject(hBitmap);//we only needed it for the header info to create a D3D surface

	//create surface for bitmap
	r = pDevice->CreateOffscreenPlainSurface(Bitmap.bmWidth, Bitmap.bmHeight, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, ppSurface, NULL);

	if (FAILED(r)) {
		SetError("Unable to create surface for bitmap load");
		return E_FAIL;
	}
	//load bitmap onto surface
	r = D3DXLoadSurfaceFromFile(*ppSurface, NULL, NULL, PathName, NULL, D3DX_DEFAULT, 0, NULL);
	if (FAILED(r)) {
		SetError("Unable to laod file to surface");
		return E_FAIL;
	}
	return S_OK;
}


int Game::InitDirect3DDevice(HWND hWndTarget, int Width, int Height, BOOL bWindowed, D3DFORMAT FullScreenFormat, LPDIRECT3D9 pD3D, LPDIRECT3DDEVICE9* ppDevice) {
	D3DPRESENT_PARAMETERS d3dpp;//rendering info
	D3DDISPLAYMODE d3ddm;//current display mode info
	HRESULT r = 0;

	if (*ppDevice)
		(*ppDevice)->Release();

	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
	r = pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
	if (FAILED(r)) {
		SetError("Could not get display adapter information");
		return E_FAIL;
	}

	d3dpp.BackBufferWidth = Width;
	d3dpp.BackBufferHeight = Height;
	d3dpp.BackBufferFormat = bWindowed ? d3ddm.Format : FullScreenFormat;
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;//D3DSWAPEFFECT_COPY;
	d3dpp.hDeviceWindow = hWndTarget;
	d3dpp.Windowed = bWindowed;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.FullScreen_RefreshRateInHz = 0;//default refresh rate
	d3dpp.PresentationInterval = bWindowed ? 0 : D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	r = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWndTarget, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, ppDevice);
	if (FAILED(r)) {
		SetError("Could not create the render device");
		return E_FAIL;
	}

	this->DeviceHeight = Height;
	this->DeviceWidth = Width;

	// Turn on the zbuffer
	g_pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	// Turn on ambient lighting 
	g_pDevice->SetRenderState(D3DRS_AMBIENT, 0xffffffff);

	//	g_DeviceHeight = Height;
	//	g_DeviceWidth = Width;

	//	g_SavedPresParams = d3dpp;
	if (D3DXCreateTextureFromFile(g_pDevice, "snow.tga", &g_Texture) != D3D_OK)
		return false;

	D3DVECTOR position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	//粒子系统的位置
	D3DVECTOR range = D3DXVECTOR3(4.0f, 4.0f, 4.0f);	//长宽高范围
	D3DVECTOR accel = D3DXVECTOR3(0.0f, -0.0002f, 0.0f);	//加速度
	D3DVECTOR emiPosMin = D3DXVECTOR3(-0.05f, -2.0f, -0.05f);	//发射位置的范围
	D3DVECTOR emiPosMax = D3DXVECTOR3(0.05f, -2.0f, 0.05f);
	D3DXVECTOR3 veloMin = D3DXVECTOR3(-0.006f, 0.02f, -0.006f);	//粒子初始速度范围
	D3DXVECTOR3 veloMax = D3DXVECTOR3(0.006f, 0.03f, 0.006f);
	D3DCOLORVALUE colorMin = { 0.0f, 0.0f, 0.0f, 0.0f };		//粒子颜色范围
	D3DCOLORVALUE colorMax = { 1.0f, 1.0f, 1.0f, 0.0f };
	float psizeMin = 0.3f;	//粒子大小范围
	float psizeMax = 0.5f;
	int maxCount = 3000;	//最大粒子数量
	int emiCount = 10;	//每次发射数量
	float emiInterval = 0.01f; //发射间隔时间
	g_Firework = new EpParticleSystem(position, range, accel, emiPosMin, emiPosMax,
		veloMin, veloMax, colorMin, colorMax, psizeMin, psizeMax, maxCount, emiCount, emiInterval,
		g_pDevice, g_Texture);
	//初始化粒子系统
	g_Firework->Initialize();

	g_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	g_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);

	//计算投影矩阵
	D3DXMatrixPerspectiveFovLH(&g_ProjMat, 45.0f, 4.0f / 3, 0.1f, 1000.0f);
	g_pDevice->SetTransform(D3DTS_PROJECTION, &g_ProjMat);

	//计算视图矩阵
	D3DXVECTOR3 cameraPos(0.0f, 0.0f, -3.0f);
	D3DXVECTOR3 lookAtPos(0.0f, -1.0f, 0.0f);
	D3DXVECTOR3 upDir(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&g_ViewMat, &cameraPos, &lookAtPos, &upDir);
	g_pDevice->SetTransform(D3DTS_VIEW, &g_ViewMat);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Game::Cleanup()
{
	for (int i = 0; i < numOfMeshFile; i++)
	{
		if (g_pMeshMaterials[i] != NULL)
		{
			delete[] g_pMeshMaterials[i];
		}
		if (g_pMeshTextures[i] != NULL)
		{
			for (DWORD j = 0; j < g_dwNumMaterials[0]; j++)
			{
				if (g_pMeshTextures[i][j])
					g_pMeshTextures[i][j]->Release();
			}
			delete[] g_pMeshTextures[i];
		}
		if (g_pMesh[i] != NULL)
		{
			g_pMesh[i]->Release();
		}
	}

	if (g_pD3D != NULL)
		g_pD3D->Release();
}

bool Game::Setup()
{
	g_pDevice->CreateVertexBuffer(
		36 * sizeof(Vertex),
		0, // usage
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&VB,
		0);

	Vertex* v = 0;
	VB->Lock(0, 0, (void**)&v, 0);

	// mirror
	v[0] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[1] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[2] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[3] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[4] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[5] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// bottom mirror
	v[6] = Vertex(-2.5f, 0.0f, 5.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f);
	v[7] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[8] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

	v[9] = Vertex(-2.5f, 0.0f, 5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[10] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[11] = Vertex(2.5f, 0.0f, 5.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
	
	// top mirror
	v[12] = Vertex(2.5f, 5.0f, 5.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
	v[13] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[14] = Vertex(-2.5f, 5.0f, 5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);

	v[15] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[16] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[17] = Vertex(-2.5f, 5.0f, 5.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f);
						  
	// back mirror
	v[18] = Vertex(2.5f, 0.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
	v[19] = Vertex(2.5f, 5.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[20] = Vertex(-2.5f, 0.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);

	v[21] = Vertex(2.5f, 5.0f, 5.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[22] = Vertex(-2.5f, 5.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[23] = Vertex(-2.5f, 0.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);

	// right side mirror
	v[24] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
	v[25] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[26] = Vertex(2.5f, 0.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);

	v[27] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[28] = Vertex(2.5f, 5.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[29] = Vertex(2.5f, 0.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);

	// left side mirror
	v[30] = Vertex(-2.5f, 0.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[31] = Vertex(-2.5f, 5.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[32] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[33] = Vertex(-2.5f, 0.0f, 5.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[34] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[35] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	VB->Unlock();

	//
	// Load Textures, set filters.
	//
	D3DXCreateTextureFromFile(g_pDevice, "earth.bmp", &MirrorTex);

	return false;
}

/*
Sets up the matrix for the camera.
*/
void Game::SetupMatrices()
{
	// For our world matrix, we will just leave it as the identity
	D3DXMATRIXA16 matWorld;
	D3DXMatrixRotationY(&matWorld, 50.0f);
	g_pDevice->SetTransform(D3DTS_WORLD, &matWorld); // Takes whatever is in the matrix, and sets it to the world
				// ** This is how we will move objects and rotate later **

	// Set up our view matrix. A view matrix can be defined given an eye point,
	// a point to lookat, and a direction for which way is up. Here, we set the
	// eye five units back along the z-axis and up three units, look at the 
	// origin, and define "up" to be in the y-direction.
	D3DXVECTOR3 vEyePt(0.0f, 3.0f, -15.0f);
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
	g_pDevice->SetTransform(D3DTS_VIEW, &matView);

	// For the projection matrix, we set up a perspective transform (which
	// transforms geometry from 3D view space to 2D viewport space, with
	// a perspective divide making objects smaller in the distance). To build
	// a perpsective transform, we need the field of view (1/4 pi is common),
	// the aspect ratio, and the near and far clipping planes (which define at
	// what distances geometry should be no longer be rendered).
	D3DXMATRIXA16 matProj;
	float screenaspect = (float)DeviceWidth / (float)DeviceHeight;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, screenaspect, 1.0f, 500.0f);
	g_pDevice->SetTransform(D3DTS_PROJECTION, &matProj);
}

D3DMATERIAL9 Game::InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p)
{
	D3DMATERIAL9 mtrl;
	mtrl.Ambient = a;
	mtrl.Diffuse = d;
	mtrl.Specular = s;
	mtrl.Emissive = e;
	mtrl.Power = p;
	return mtrl;
}

void Game::RenderMirror(D3DXPLANE plane, int streamOffset)
{
	g_pDevice->Clear(0, 0, D3DCLEAR_STENCIL, D3DCOLOR_ARGB(255, 0, 0, 55), 1.0f, 0);
	//
	// Draw Mirror quad to stencil buffer ONLY.  In this way
	// only the stencil bits that correspond to the mirror will
	// be on.  Therefore, the reflected teapot can only be rendered
	// where the stencil bits are turned on, and thus on the mirror 
	// only.
	//
	g_pDevice->SetRenderState(D3DRS_STENCILENABLE, true);
	g_pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	g_pDevice->SetRenderState(D3DRS_STENCILREF, 0x1);
	g_pDevice->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
	g_pDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
	g_pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	g_pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
	g_pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);

	// disable writes to the depth and back buffers
	g_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);
	g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	g_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
	g_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// draw the mirror to the stencil buffer
	g_pDevice->SetStreamSource(0, VB, 0, sizeof(Vertex));
	g_pDevice->SetFVF(Vertex::FVF);
	g_pDevice->SetMaterial(&MirrorMtrl);
	g_pDevice->SetTexture(0, MirrorTex);
	D3DXMATRIX I;
	D3DXMatrixIdentity(&I);
	g_pDevice->SetTransform(D3DTS_WORLD, &I);
	g_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, streamOffset, 2);

	// re-enable depth writes
	g_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);

	// only draw reflected teapot to the pixels where the mirror
	// was drawn to.
	g_pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	g_pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);

	// position reflection
	D3DXMATRIX R;
	//D3DXPLANE plane(0.0f, 0.0f, 1.0f, 0.0f); // xy plane
	D3DXMatrixReflect(&R, &plane);

	D3DXMATRIX netM;
	D3DXMatrixIdentity(&netM);

	for (int i = 0; i < numOfObject; i++) {
		// copy object's net trans to netM
		netM = matObj[i];

		// translate object to match reflection plane's level
		if (plane.a != 0) {
			if (plane.a >= 0)
			{
				netM._41 += 1 - (plane.a);
			}
			else
			{
				netM._41 += -1 - (plane.a);
			}
		}
		if (plane.b != 0) {
			if (plane.b >= 0) {
				netM._42 += 1 - (plane.b);
			}
			else
			{
				netM._42 += -1 - (plane.b);
			}
		}
		if (plane.c != 0) {
			if (plane.c >= 0) {
				netM._43 += 1 - (plane.c);
			}
			else
			{
				netM._43 += -1 - (plane.c);
			}
		}

		// reflect netM on given plane
		netM = netM * R;

		// translate object back to original level
		if (plane.a != 0) {
			if (plane.a >= 0)
			{
				netM._41 -= 1 - (plane.a);
			}
			else
			{
				netM._41 -= -1 - (plane.a);
			}
		}
		if (plane.b != 0) {
			if (plane.b >= 0) {
				netM._42 += 1 - (plane.b);
			}
			else
			{
				netM._42 += -1 - (plane.b);
			}
		}
		if (plane.c != 0) {
			if (plane.c >= 0) {
				netM._43 += 1 - (plane.c);
			}
			else
			{
				netM._43 += -1 - (plane.c);
			}
		}

		// clear depth buffer and blend the reflected object with the mirror
		g_pDevice->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
		g_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
		g_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

		if (plane.c == 1 && netM._43 > 0.0f)
		{
			drawObject(netM, i, 1);
		}
		else if (plane.b == 1 && netM._42 > 0.0f)
		{
			drawObject(netM, i, 1);
		}
		else if (plane.b == 6 && netM._42 < 5.0f)
		{
			drawObject(netM, i, 1);
		}
		else if (plane.c == 6 && netM._43 < 5.0f)
		{
			drawObject(netM, i, 1);
		}
		else if (plane.a == -3.5 && netM._41 > -2.5f)
		{
			drawObject(netM, i, 1);
		}
		else if (plane.a == 3.5 && netM._41 < 2.5f)
		{
			drawObject(netM, i, 1);
		}
	}
	
	// Restore render states.
	g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	g_pDevice->SetRenderState(D3DRS_STENCILENABLE, false);
	g_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void Game::drawObject(D3DXMATRIX W, int objectIndex, int reflection)
{
	// Finally, draw the reflected object
	objectIndex %= 2;
	g_pDevice->SetTransform(D3DTS_WORLD, &W);
	if (reflection)
	{
		g_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	}
	// Meshes are divided into subsets, one for each material. Render them in
	// a loop
	for (DWORD i = 0; i < g_dwNumMaterials[0]; i++)
	{
		// Set the material and texture for this subset
		g_pDevice->SetMaterial(&g_pMeshMaterials[objectIndex][i]);
		g_pDevice->SetTexture(0, g_pMeshTextures[objectIndex][i]);
		// Draw the mesh subset					 
		g_pMesh[objectIndex]->DrawSubset(i);
	}
	if (reflection)
	{
		g_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	}
}

Ray Game::CalcPickingRay(int x, int y)
{
	float px = 0.0f;
	float py = 0.0f;

	D3DVIEWPORT9 vp;
	g_pDevice->GetViewport(&vp);

	D3DXMATRIX proj;
	g_pDevice->GetTransform(D3DTS_PROJECTION, &proj);

	px = (((2.0f*x) / vp.Width) - 1.0f) / proj(0, 0);
	py = (((-2.0f*y) / vp.Height) + 1.0f) / proj(1, 1);

	Ray ray;
	ray._origin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	ray._direction = D3DXVECTOR3(px, py, 1.0f);

	return ray;
}

void Game::TransformRay(Ray* ray, D3DXMATRIX* T)
{
	// transform the ray's origin, w = 1.
	D3DXVec3TransformCoord(
		&ray->_origin,
		&ray->_origin,
		T);

	// transform the ray's direction, w = 0.
	D3DXVec3TransformNormal(
		&ray->_direction,
		&ray->_direction,
		T);

	// normalize the direction
	D3DXVec3Normalize(&ray->_direction, &ray->_direction);
}

float* Game::RaySphereIntTest(Ray* ray, BoundingSphere* sphere)
{
	float* result = new float[2];
	D3DXVECTOR3 v = ray->_origin - sphere->_center;

	float b = 2.0f * D3DXVec3Dot(&ray->_direction, &v);
	float c = D3DXVec3Dot(&v, &v) - (sphere->_radius * sphere->_radius);

	// find the discriminant
	float discriminant = (b * b) - (4.0f * c);

	// test for imaginary number
	if (discriminant < 0.0f)
	{
		result[0] = -1;
		result[1] = -1;
		return result;
	}

	discriminant = sqrtf(discriminant);

	result[0] = (-b + discriminant) / 2.0f;
	result[1] = (-b - discriminant) / 2.0f;

	return result;
}

bool Game::SetupPicking()
{
	for (int i = 0; i < numOfObject; i++)
	{
		BYTE* v = 0;
		g_pMesh[i % 2]->LockVertexBuffer(0, (void**)&v);

		D3DXComputeBoundingSphere(
			(D3DXVECTOR3*)v,
			g_pMesh[i % 2]->GetNumVertices(),
			D3DXGetFVFVertexSize(g_pMesh[i % 2]->GetFVF()),
			&BSphere[i]._center,
			&BSphere[i]._radius);
		g_pMesh[i % 2]->UnlockVertexBuffer();
		D3DXCreateSphere(g_pDevice, BSphere[i]._radius, 20, 20, &Sphere[i], 0);
	}
	return true;
}

BoundingBox::BoundingBox()
{
	// infinite small 
	_min.x = C_INFINITY;
	_min.y = C_INFINITY;
	_min.z = C_INFINITY;

	_max.x = -C_INFINITY;
	_max.y = -C_INFINITY;
	_max.z = -C_INFINITY;
}

bool BoundingBox::isPointInside(D3DXVECTOR3& p)
{
	if (p.x >= _min.x && p.y >= _min.y && p.z >= _min.z &&
		p.x <= _max.x && p.y <= _max.y && p.z <= _max.z)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BoundingSphere::BoundingSphere()
{
	_radius = 0.0f;
}

/*
Move the camera based on x, y, z
*/
void Game::moveCamera(float x, float y, float z) {
	D3DXMATRIX newM;
	D3DXMatrixTranslation(&newM, x, y, z);
	D3DXMatrixMultiply(&matView, &matView, &newM);
}
/*
Rotate the camera about x.
*/
void Game::rotateCameraX(float r) {
	// rotate around y
	D3DXMATRIX newM;
	D3DXMatrixRotationX(&newM, r);
	D3DXMatrixMultiply(&matView, &matView, &newM);
}
/*
Rotate the camera about y.
*/
void Game::rotateCameraY(float r) {
	// rotate around y
	D3DXMATRIX newM;
	D3DXMatrixRotationY(&newM, r);
	D3DXMatrixMultiply(&matView, &matView, &newM);
}
/*
Rotate the camera about z.
*/
void Game::rotateCameraZ(float r) {
	// rotate around y
	D3DXMATRIX newM;
	D3DXMatrixRotationZ(&newM, r);
	D3DXMatrixMultiply(&matView, &matView, &newM);
}
/*
Move the object based on the object number and x, y, z.
*/
void Game::moveObject(int objNum, float x, float y, float z) {
	D3DXMATRIX newM;
	D3DXMatrixTranslation(&newM, x, y, z);
	D3DXMatrixMultiply(&(matObj[objNum]), &(matObj[objNum]), &newM);
}
/*
Rotate the object about x.
*/
void Game::rotateObjectX(int objNum, float r) {
	// rotate around y
	D3DXMATRIX newM, matRot, backM;
	float x, y, z;
	D3DXMatrixRotationX(&matRot, r);
	x = matObj[objNum]._41;
	y = matObj[objNum]._42;
	z = matObj[objNum]._43;
	D3DXMatrixTranslation(&newM, -x, -y, -z);
	D3DXMatrixMultiply(&newM, &newM, &matRot);
	D3DXMatrixTranslation(&backM, x, y, z);
	D3DXMatrixMultiply(&newM, &newM, &backM);
	D3DXMatrixMultiply(&(matObj[objNum]), &(matObj[objNum]), &newM);
}
/*
Rotate the camera about y.
*/
void Game::rotateObjectY(int objNum, float r) {
	// rotate around y
	D3DXMATRIX newM, matRot, backM;
	float x, y, z;
	D3DXMatrixRotationY(&matRot, r);
	x = matObj[objNum]._41;
	y = matObj[objNum]._42;
	z = matObj[objNum]._43;
	D3DXMatrixTranslation(&newM, -x, -y, -z);
	D3DXMatrixMultiply(&newM, &newM, &matRot);
	D3DXMatrixTranslation(&backM, x, y, z);
	D3DXMatrixMultiply(&newM, &newM, &backM);
	D3DXMatrixMultiply(&(matObj[objNum]), &(matObj[objNum]), &newM);
}
/*
Rotate the camera about z.
*/
void Game::rotateObjectZ(int objNum, float r) {
	// rotate around y
	D3DXMATRIX newM, matRot, backM;
	float x, y, z;
	D3DXMatrixRotationZ(&matRot, r);
	x = matObj[objNum]._41;
	y = matObj[objNum]._42;
	z = matObj[objNum]._43;
	D3DXMatrixTranslation(&newM, -x, -y, -z);
	D3DXMatrixMultiply(&newM, &newM, &matRot);
	D3DXMatrixTranslation(&backM, x, y, z);
	D3DXMatrixMultiply(&newM, &newM, &backM);
	D3DXMatrixMultiply(&(matObj[objNum]), &(matObj[objNum]), &newM);
}
/*
Gets if the camera is selected.
*/
bool Game::getCameraMove() {
	return CameraMove;
}

/*
Gets if the camera is selected.
*/
bool Game::selectCamera()
{
	return cameraSelected;
}
/*
Either select's or deselect's the camera.
*/
void Game::setCameraSelected()
{
	cameraSelected = true;
	for (int i = 0; i < numOfObject; i++)
	{
		objSelected[i] = false;
	}
}

/*
Either select's or deselect's the first object.
*/
void Game::selectObj(int index)
{
	cameraSelected = false;
	for (int i = 0; i < numOfObject; i++)
	{
		if (i == index) {
			objSelected[i] = true;
		}
		else
		{
			objSelected[i] = false;
		}
	}
}
/*
Gets if object 1 is selected.
*/
bool Game::getObj1Move() {
	return Obj1Move;
}
/*
Gets if object 2 is selected.
*/
bool Game::getObj2Move() {
	return Obj2Move;
}
/*
Either select's or deselect's the camera.
*/
void Game::setCameraMove(bool b) {
	CameraMove = b;
	Obj1Move = false;
	Obj2Move = false;
}
/*
Either select's or deselect's the first object.
*/
void Game::setObj1Move(bool b) {
	Obj1Move = b;
	CameraMove = false;
	Obj2Move = false;
}
/*
Either select's or deselect's the second object.
*/
void Game::setObj2Move(bool b) {
	Obj2Move = b;
	CameraMove = false;
	Obj1Move = false;
}
/*
Sets the lighting to directional.
*/
void Game::SetLightingDirectional() {
	ZeroMemory(&material, sizeof(D3DMATERIAL9));
	material.Diffuse.r = 1.0f;
	material.Diffuse.g = 1.0f;
	material.Diffuse.b = 1.0f;
	material.Diffuse.a = 1.0f;

	material.Ambient.r = 1.0f;
	material.Ambient.g = 1.0f;
	material.Ambient.b = 1.0f;
	material.Ambient.a = 1.0f;

	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	D3DXVECTOR3 vecDir;
	vecDir = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vecDir);
	g_pDevice->SetMaterial(&material);
	g_pDevice->SetLight(0, &light);
	g_pDevice->LightEnable(0, TRUE);
	g_pDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(10, 10, 10));
}
/*
Sets the lighting to ambient.
*/
void Game::SetLightingAmbient() {
	g_pDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(255, 255, 255));
}
/*
Sets the lighting to spot.
*/
void Game::SetLightingSpot() {
	ZeroMemory(&material, sizeof(D3DMATERIAL9));
	material.Diffuse.r = 1.0f;
	material.Diffuse.g = 1.0f;
	material.Diffuse.b = 1.0f;
	material.Diffuse.a = 1.0f;

	material.Ambient.r = 1.0f;
	material.Ambient.g = 1.0f;
	material.Ambient.b = 1.0f;
	material.Ambient.a = 1.0f;

	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_SPOT;
	light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	light.Position = D3DXVECTOR3(0.0f, 0.0f, -10.0f);
	light.Direction = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	light.Range = 100.0f;    // a range of 100
	light.Attenuation0 = 0.0f;    // no constant inverse attenuation
	light.Attenuation1 = 0.125f;    // only .125 inverse attenuation
	light.Attenuation2 = 0.0f;    // no square inverse attenuation
	light.Phi - D3DXToRadian(40.0f);
	light.Theta = D3DXToRadian(20.0f);
	light.Falloff = 1.0f;

	g_pDevice->SetMaterial(&material);
	g_pDevice->SetLight(0, &light);
	g_pDevice->LightEnable(0, TRUE);
	g_pDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(10, 10, 10));
}
/*
Sets the lighting to point.
*/
void Game::SetLightingPoint() {
	ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_POINT;
	light.Diffuse = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	light.Position = D3DXVECTOR3(0.0f, 5.0f, 0.0f);
	light.Range = 100.0f;
	light.Attenuation0 = 0.0f;
	light.Attenuation1 = 0.125f;
	light.Attenuation2 = 0.0f;

	ZeroMemory(&material, sizeof(D3DMATERIAL9));
	material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	material.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	g_pDevice->SetMaterial(&material);
	g_pDevice->SetLight(0, &light);
	g_pDevice->LightEnable(0, TRUE);
	g_pDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(10, 10, 10));
}