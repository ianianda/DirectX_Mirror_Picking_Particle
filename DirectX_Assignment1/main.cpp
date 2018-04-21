#define WIN32_LEAN_AND_MEAN

#include "Basics.h"
#include "Game.h"
#include "EpParticleSystem.h"
Game g;


long CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam) {
	short delta;

	switch (uMessage) {
	case WM_CREATE:
	{
		return 0;
	}
	case WM_PAINT:
	{
		ValidateRect(hWnd, NULL);//basically saying - yeah we took care of any paint msg without any overhead
		return 0;
	}
	case WM_CHAR: {
		if (wParam == VK_ESCAPE)
		{
			SendMessage(hWnd, WM_DESTROY, NULL, NULL);
		}
		return 0;
	}
	case WM_MOUSEWHEEL:
		delta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (delta > 0) {
			if (g.getCameraMove()) {
				g.rotateCameraX(-0.3f);
			}
			else if (g.getObj1Move()) {
				g.rotateObjectX(1, -0.3f);
			}
			else if (g.getObj2Move()) {
				g.rotateObjectX(2, -0.3f);
			}
		}
		else {
			if (g.getCameraMove()) {
				g.rotateCameraX(0.3f);
			}
			else if (g.getObj1Move()) {
				g.rotateObjectX(1, 0.3f);
			}
			else if (g.getObj2Move()) {
				g.rotateObjectX(2, 0.3f);
			}
		}
		break;
	case WM_KEYDOWN:
		switch (wParam) {
		case 0x30:
			// 0
			// move camera
			// set move camera bool
			g.setCameraSelected();
			break;
		case 0x31:
			// 1
			//g.moveobj(1)
			g.selectObj(0);
			break;
		case 0x32:
			// 2
			//g.moveobj(2)
			g.selectObj(1);
			break;
			// moves current selection
		case 0x33:
			// 3
			//g.moveobj(1)
			g.selectObj(2);
			break;
		case 0x34:
			// 4
			//g.moveobj(2)
			g.selectObj(3);
			break;
			// moves current selection
		case 0x57:
			// w // move up
			if (g.selectCamera())
			{
				g.moveCamera(0.0f, 1.0f, 0.0f);
			}
			else
			{
				for (int i = 0; i < g.numOfObject; i++)
				{
					if (g.objSelected[i])
					{
						g.moveObject(i, 0.0f, 1.0f, 0.0f);
						break;
					}
				}
			}
			break;
		case 0x41:
			// a // move left
			if (g.selectCamera()) {
				g.moveCamera(-1.0f, 0.0f, 0.0f);
			}
			else
			{
				for (int i = 0; i < g.numOfObject; i++)
				{
					if (g.objSelected[i])
					{
						g.moveObject(i, -1.0f, 0.0f, 0.0f);
						break;
					}
				}
			}
			break;
		case 0x53:
			// s // move down
			if (g.selectCamera()) {
				g.moveCamera(0.0f, -1.0f, 0.0f);
			}
			else
			{
				for (int i = 0; i < g.numOfObject; i++)
				{
					if (g.objSelected[i])
					{
						g.moveObject(i, 0.0f, -1.0f, 0.0f);
						break;
					}
				}
			}
			break;
		case 0x44:
			// d // move right
			if (g.selectCamera()) {
				g.moveCamera(1.0f, 0.0f, 0.0f);
			}
			else
			{
				for (int i = 0; i < g.numOfObject; i++)
				{
					if (g.objSelected[i])
					{
						g.moveObject(i, 1.0f, 0.0f, 0.0f);
						break;
					}
				}
			}
			break;
		case 0x58:
			// x // move farther
			if (g.selectCamera()) {
				g.moveCamera(0.0f, 0.0f, 1.0f);
			}
			else
			{
				for (int i = 0; i < g.numOfObject; i++)
				{
					if (g.objSelected[i])
					{
						g.moveObject(i, 0.0f, 0.0f, 1.0f);
						break;
					}
				}
			}
			break;
		case 0x5A:
			// z // move closer
			if (g.selectCamera()) {
				g.moveCamera(0.0f, 0.0f, -1.0f);
			}
			else
			{
				for (int i = 0; i < g.numOfObject; i++)
				{
					if (g.objSelected[i])
					{
						g.moveObject(i, 0.0f, 0.0f, -1.0f);
						break;
					}
				}
			}
			break;
		case 0x51:
			// q // rotate left
			if (g.selectCamera()) {
				g.rotateCameraY(-0.3f);
			}
			else
			{
				for (int i = 0; i < g.numOfObject; i++)
				{
					if (g.objSelected[i])
					{
						g.rotateObjectY(i, -0.3f);
						break;
					}
				}
			}
			break;
		case 0x45:
			// e // rotate right
			if (g.selectCamera()) {
				g.rotateCameraY(0.3f);
			}
			else
			{
				for (int i = 0; i < g.numOfObject; i++)
				{
					if (g.objSelected[i])
					{
						g.rotateObjectY(i, 0.3f);
						break;
					}
				}
			}
			break;
		case 0x50:
			// point lighting enabled
			// p
			g.SetLightingPoint();
			break;
		case 0x4F:
			// directional lighting enabled
			// o
			g.SetLightingDirectional();
			break;
		case 0x49:
			// spot lighting enabled
			// i
			g.SetLightingSpot();
			break;
		case 0x55:
			// ambient lighting enabled
			// u
			g.SetLightingAmbient();
			break;
		case 0x46:
			// f // rotate about z axis ccw
			if (g.selectCamera()) {
				g.rotateCameraZ(0.3f);
			}
			else
			{
				for (int i = 0; i < g.numOfObject; i++)
				{
					if (g.objSelected[i])
					{
						g.rotateObjectZ(i, 0.3f);
						break;
					}
				}
			}
			break;
		case 0x47:
			// g // rotate about z axis cw
			if (g.selectCamera()) {
				g.rotateCameraZ(-0.3f);
			}
			else
			{
				for (int i = 0; i < g.numOfObject; i++)
				{
					if (g.objSelected[i])
					{
						g.rotateObjectZ(i, -0.3f);
						break;
					}
				}
			}
			break;
		case 0x42:
			// c // rotate about x axis ccw
			if (g.selectCamera()) {
				g.rotateCameraX(0.3f);
			}
			else
			{
				for (int i = 0; i < g.numOfObject; i++)
				{
					if (g.objSelected[i])
					{
						g.rotateObjectX(i, 0.3f);
						break;
					}
				}
			}
			break;
		case 0x43:
			// v // rotate about x axis cw
			if (g.selectCamera()) {
				g.rotateCameraX(-0.3f);
			}
			else
			{
				for (int i = 0; i < g.numOfObject; i++)
				{
					if (g.objSelected[i])
					{
						g.rotateObjectX(i, -0.3f);
						break;
					}
				}
			}
			break;
		}

		return 0;
	case WM_LBUTTONDOWN:
	{
		// compute the ray in view space given the clicked screen point
		Ray ray = g.CalcPickingRay(LOWORD(lParam), HIWORD(lParam));

		// transform the ray to world space
		D3DXMATRIX view;
		g.g_pDevice->GetTransform(D3DTS_VIEW, &view);

		D3DXMATRIX viewInverse;
		D3DXMatrixInverse(&viewInverse, 0, &view);

		g.TransformRay(&ray, &viewInverse);

		// test for a hit
		float** results = new float*[g.numOfObject];
		for (int i = 0; i < g.numOfObject; i++)
		{
			results[i] = g.RaySphereIntTest(&ray, &(g.BSphere[i]));
		}
		float curMinPosS = C_INFINITY;
		float curMinPosD = C_INFINITY;
		float preMinPosS = C_INFINITY;
		float preMinPosD = C_INFINITY;
		int curSelected = -1;
		for (int i = 0; i < g.numOfObject; i++)
		{
			if (results[i][0] >= 0 && results[i][1] >= 0)
			{
				curMinPosD = results[i][0] < results[i][1] ? results[i][0] : results[i][1];
			}
			else if (results[i][0] >= 0 || results[i][1] >= 0)
			{
				curMinPosS = results[i][0] > 0 ? results[i][0] : results[i][1];
			}
			if (curMinPosD < preMinPosD)
			{
				preMinPosD = curMinPosD;
				curSelected = i;
			}
			else if (curMinPosS < preMinPosS && preMinPosD != C_INFINITY)
			{
				preMinPosS = curMinPosS;
				curSelected = i;
			}
		}
		if (curSelected != -1)
		{
			g.selectObj(curSelected);
		}
		return 0;
	}
	case WM_MOUSEMOVE:
		if (wParam == MK_LBUTTON) {
			// holding down the left button
		}
		return 0;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	default:
	{
		return DefWindowProc(hWnd, uMessage, wParam, lParam);
	}
	}
}

/*
WinMain the main function
	@param hInstance
	@param hPrevInstance
	@param pstrCmdLine
	@param iCmdShow
*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pstrCmdLine, int iCmdShow) {
	HWND hWnd;
	MSG msg;
	WNDCLASSEX wc;
	HWND g_hWndMain;//handle to main window

	static char strAppName[] = "First Windows App, Zen Style";

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_HAND);
	wc.hCursor = LoadCursor(NULL, IDC_CROSS);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = strAppName;

	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(NULL,
		strAppName,
		strAppName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		512, 512,
		NULL,
		NULL,
		hInstance,
		NULL);

	g = Game(hWnd);

	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	if (FAILED(g.GameInit())) {
		;//initialize Game
		SetError("Initialization Failed");
		g.GameShutdown();
		return E_FAIL;
	}

	while (TRUE) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			g.GameLoop();
		}
	}

	g.GameShutdown();// clean up the game
	return msg.wParam;
}