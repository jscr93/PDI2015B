// PDI2015B.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "PDI2015B.h"
#include "DXGIManager.h"
#include "math.h"
#include "CSDefault.h"
#include "CSConvolve.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
CDXGIManager g_Manager;
ID3D11Texture2D* g_pSource;
CCSDefault *g_pCSDefault;
CCSConvolve* g_pCSConvolve;			//Shader de convolucion

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PDI2015B, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PDI2015B));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PDI2015B));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_PDI2015B);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	IDXGIAdapter *pAdapter = g_Manager.EnumAdapters(hWnd);
	bool bResult = g_Manager.Initialize(hWnd, pAdapter, false);
	SAFE_RELEASE(pAdapter);

	if (!bResult)
		return FALSE;

	g_pCSDefault = new CCSDefault(&g_Manager);
	if (!g_pCSDefault->Initialize())
		return FALSE;

	g_pCSConvolve = new CCSConvolve(&g_Manager);
	if (!g_pCSConvolve->Initialize())
		return FALSE;

	g_pSource = g_Manager.LoadTexture("..\\Resources\\iss.bmp");




	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	static float s_fTime = 0;
	static float s_fScale = 1.0f;
	static float s_fTheta = 0;
	static int s_mnx, s_mny, s_fInterpolation = 0;
	switch (message)
	{
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case 'Z':
			s_fScale += 0.1;
			break;
		case 'X':
			s_fScale -= 0.1;
			break;
		case 'Q':
			s_fTheta += 0.1;
			break;
		case 'E':
			s_fTheta -= 0.1;
			break;
		case 'I':
			s_fInterpolation = ~s_fInterpolation;
			break;
		}
	}
	break;
	case WM_MOUSEMOVE:
	{
		s_mnx = LOWORD(lParam);
		s_mny = HIWORD(lParam);
		break;
	}
	break;
	case WM_CREATE:
		SetTimer(hWnd, 1, 20, NULL);
		return 0;
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			s_fTime += 0.2;
			InvalidateRect(hWnd, NULL, false);
		}
		break;
	case WM_SIZE:
		if (g_Manager.GetDevice())
			g_Manager.Resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
	{
		//tuberia 1: a) Transformacion Afin y B) luego una convolucion
		//Necesito crear todas las variables temporales que requiera
		//la tuberia.

		ID3D11Texture2D * pIntermedio = 0;
		D3D11_TEXTURE2D_DESC dtd;
		g_pSource->GetDesc(&dtd);
		dtd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS; //Buffers intermedios tener este bindflag
		g_Manager.GetDevice()->CreateTexture2D(&dtd, NULL, &pIntermedio);

		//Procesar
		g_pCSDefault->m_pInput = g_pSource;
		g_pCSDefault->m_pOutput = pIntermedio;
		MATRIX4D S = Scale(s_fScale, s_fScale, 1);
		MATRIX4D R = RotationZ(s_fTheta);
		MATRIX4D T = Translate(s_mnx, s_mny, 0);

		g_pCSDefault->m_Params.M = Inverse(S*R*T);
		g_pCSDefault->Configure();
		g_pCSDefault->Execute(); 

		g_pCSConvolve->m_pInput = pIntermedio;
		g_pCSConvolve->m_pOutput = g_Manager.GetBackBuffer();
		MATRIX4D Kernel = { 1,1,1,0,
							0,0,0,0,
							-1,-1,-1,0,
							0,0,0,0 };

		MATRIX4D KernelIdentity = { 0,0,0,0,
									0,1,0,0,
									0,0,0,0,
									0,0,0,0 }; // C=0 o cualquier valor

		MATRIX4D KernelInvert = { 0,0,0,0,
									0,-1,0,0,
									0,0,0,0,
									0,0,0,0 }; // C=1

		MATRIX4D KernelSoft = { 1/9.0f,1/9.0f,1/9.0f,0,
								1/9.0f,1/9.0f,1/9.0f,0,
								1/9.0f,1/9.0f,1/9.0f,0,
								0,0,0,0 };

		MATRIX4D KernelLaplace = { 0, -1/8.0f,  0, 0,
								-1/8.0f, 1/2.0f, -1/8.0f, 0,
								0, -1/8.0f, 0, 0,
								0, 0 , 0, 0};//C=0.5

		MATRIX4D KernelEmbossV = { -1, -1, -1, 0,
								  0, 0, 0, 0,
								  1, 1, 1, 1,
								  0, 0, 0, 0 };//C = 0.5

		float Strength = fabs(cos(s_fTime));
		float c = 1 - Strength;
		MATRIX4D KernelSharp = { 0, c , 0, 0,
								 c, 1, -c, 0,
								  0, -c, 0, 0,
								  0, 0, 0, 0 };//C = 0

		g_pCSConvolve->m_Params.Kernel = KernelSharp;
		g_pCSConvolve->m_Params.C = 0;
		g_pCSConvolve->Configure();
		g_pCSConvolve->Execute();

		//Liberar toda memoria intermedia al terminar de procesar
		SAFE_RELEASE(pIntermedio);
		g_Manager.GetSwapChain()->Present(1, 0);//T-1 sync
	}
	ValidateRect(hWnd, NULL);
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

