// PDI2015B.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "PDI2015B.h"
#include "DXGIManager.h"
#include "math.h"
#include "CSDefault.h"
#include "CSConvolve.h"
#include "CSALU.h"
#include "..\\Video\\AtWareVideoCapture.h"
#include "VideoProcessor.h"
#include "Frame.h"
#include "CSMetaCanvas.h"
#include "CSFusion.h"
#include "gif.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
CDXGIManager g_Manager;
ID3D11Texture2D* g_pSource;
ID3D11Texture2D* g_pStaticVideoImage;
ID3D11Texture2D* g_pMetaCanvas;
CCSDefault *g_pCSDefault;
CCSConvolve* g_pCSConvolve;						//Shader de convolucion
CCSALU* g_pCSALU;
IAtWareVideoCapture* g_pIVC;						//Interface Video Capture
CVideoProcessor g_VP;								//The video processor
CCSMetaCanvas* g_pCSMC;
CCSFusion* g_pCSFusion;
bool g_ExistsStaticVideoImage;
bool g_ExistsMetaCanvas;
GifWriter* g_gifWriter;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	VideoHost(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

CDXGIManager::PIXEL alpha(CDXGIManager::PIXEL);

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
	//wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.hbrBackground = 0;
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_PDI2015B);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	ATOM a = RegisterClassEx(&wcex);
	wcex.lpfnWndProc = VideoHost;
	wcex.lpszClassName = L"VideoHost";
	wcex.lpszMenuName = NULL;
	RegisterClassEx(&wcex);

	return a;
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

	g_pCSALU = new CCSALU(&g_Manager);
	if (!g_pCSALU->Initialize())
		return FALSE;

	g_pCSMC = new CCSMetaCanvas(&g_Manager);
	if (!g_pCSMC->Initialize())
		return FALSE;

	g_pCSFusion = new CCSFusion(&g_Manager);
	if (!g_pCSFusion->Initialize())
		return FALSE;

	g_gifWriter = new GifWriter;

	g_pSource = g_Manager.LoadTexture("..\\Resources\\iss.bmp", -1, alpha);
	g_pStaticVideoImage = g_pSource;

	g_ExistsStaticVideoImage = false;
	g_ExistsMetaCanvas = false;

	printf("hola mundo");
	wprintf(L"hola mundo");
	

	HWND hWndVH = CreateWindowEx(WS_EX_TOOLWINDOW, L"VideoHost", L"Vista Previa",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInst, NULL);
	g_pIVC = CreateAtWareVideoCapture();
	g_pIVC->Initialize(hWndVH);
	g_pIVC->EnumAndChooseCaptureDevice();
	g_pIVC->BuildStreamGraph();
	g_pIVC->SetCallBack(&g_VP, 1);
	AM_MEDIA_TYPE mt;
	g_pIVC->GetMediaType(&mt);
	g_VP.m_mt = mt;
	g_pIVC->Start();
	g_pIVC->ShowPreviewWindow(true);


	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	ShowWindow(hWndVH, nCmdShow);
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
	static int ALU_op = 0;
	static int s_fInterpolation = 0;
	static int s_mnx, s_mny = -1;
	static int s_prev_mnx, s_prev_mny = -1;
	static int brush_size = 10;
	static int style = 0;
	static bool canvasresised = false;
	static int gifFrameNumber = 0;
	
	static int click = 0;
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
		case 'U':
			if(ALU_op<12)
				ALU_op++;
			break;
		case 'D':
			if (ALU_op>0)
				ALU_op--;
			break;
		case 'M':
			g_ExistsStaticVideoImage = false;
			break;
		case 'P':
			if(brush_size<100)
				brush_size++;
			break;
		case 'L':
			if(brush_size>1)
				brush_size--;
			break;
		case '1':
			style = 1;
			break;
		case '0':
			style = 0;
			break;
		case 'N':
			g_ExistsMetaCanvas = false;
			break;
		}
	}
	break;
	case WM_LBUTTONDOWN:
		click = 1;
		break;
	case WM_RBUTTONDOWN:
		click = 2;
		break;
	case WM_RBUTTONUP:
	case WM_LBUTTONUP:
		click = 0;
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
		if (CFrame* pullFrame = g_VP.Pull())
		{
			g_pSource = g_Manager.LoadTexture(pullFrame);
			CFrame* frame = g_Manager.LoadTextureBack(g_pSource);


			D3D11_TEXTURE2D_DESC dtd;
			//Initializes textures
			g_Manager.GetBackBuffer()->GetDesc(&dtd);
			dtd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
			
			g_pCSALU->m_pInput_1 = g_pSource;
			g_pCSALU->m_pOutput = g_Manager.GetBackBuffer();
			g_pCSALU->Configure((ALU_OPERATION)ALU_NEG);
			g_pCSALU->Execute();

			UINT frameSize = frame->m_sx*frame->m_sy;
			uint8_t* uint8_tFrame;
			uint8_tFrame = new uint8_t[frameSize*4];
			for (int j = 0; j < frame->m_sy; j++)
			{
				for (int i = 0; i < frame->m_sx; i++)
				{
					CFrame::PIXEL Color = frame->GetPixel(i, j);
					uint8_tFrame[frame->m_sx*j + (i*4)] = Color.r;
					uint8_tFrame[frame->m_sx*j + (i*4)+1] = Color.g;
					uint8_tFrame[frame->m_sx*j + (i*4)+2] = Color.b;
					uint8_tFrame[frame->m_sx*j + (i*4)+3] = Color.a;
				}
			}

			GifWriter gifWriter;
			if (gifFrameNumber == 0)
			{
				GifBegin(&gifWriter, "..\\Resources\\myGif.gif", frame->m_sx, frame->m_sy, 1000);
			}
			while (gifFrameNumber < 100)
			{
				GifWriteFrame(&gifWriter, uint8_tFrame, frame->m_sx, frame->m_sy, 100);
				gifFrameNumber++;
			}
			if (gifFrameNumber == 100)
			{
				GifEnd(&gifWriter);
			}
		}

		/*if (CFrame* pullFrame = g_VP.Pull())
		{

			if (!g_ExistsMetaCanvas)
			{
				//This function creates a white texture2d of the same size as the pulled video frame.
				//It does not delete the frame
				g_pMetaCanvas = g_Manager.LoadWhiteTextureOfSize(pullFrame);
				g_ExistsMetaCanvas = true;
			}

			//Creates a texture2d from the pulled video frame and then deletes the frame
			g_pSource = g_Manager.LoadTexture(pullFrame);

			//Creates an static image for the style 0
			if (!g_ExistsStaticVideoImage) 
			{
				g_pStaticVideoImage = g_pSource;
				g_ExistsStaticVideoImage = true;
			}
		}

		if (s_prev_mnx == -1)
		{
			s_prev_mnx = s_mnx;
			s_prev_mny = s_mny;
		}

		//If it doesn't exist a metacanvas yet, do not do anything
		if (!g_pMetaCanvas)
			return 0;

		//Pipeline textures
		ID3D11Texture2D* pFusionOut;
		ID3D11Texture2D* pMetaCanvasOut;
		ID3D11Texture2D* pImageModifiedOut;
		ID3D11Texture2D* pImageModified = NULL;

		D3D11_TEXTURE2D_DESC dtd;

		//Initializes textures
		g_Manager.GetBackBuffer()->GetDesc(&dtd);
		dtd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		g_Manager.GetDevice()->CreateTexture2D(&dtd, NULL, &pMetaCanvasOut);
		g_Manager.GetDevice()->CreateTexture2D(&dtd, NULL, &pFusionOut);

		 
		//Style 0: Static Video Image
		if (style == 0)
		{
			pImageModified = g_pStaticVideoImage;
		}
		//Style 2: Negative Image
		else if (style == 1)
		{
			g_Manager.GetDevice()->CreateTexture2D(&dtd, NULL, &pImageModifiedOut);
			g_pCSALU->m_pInput_1 = g_pSource;
			g_pCSALU->m_pOutput = pImageModifiedOut;
			g_pCSALU->Configure((ALU_OPERATION)ALU_NEG);
			g_pCSALU->Execute();
			pImageModified = pImageModifiedOut;
		}

		g_pCSFusion->m_pInput_1 = g_pSource;
		g_pCSFusion->m_pInput_2 = pImageModified;
		g_pCSFusion->m_pInput_3 = g_pMetaCanvas;
		g_pCSFusion->m_pOutput = pFusionOut;
		g_pCSFusion->Configure();
		g_pCSFusion->Execute();

		g_pCSMC->m_pInput_1 = pFusionOut;
		g_pCSMC->m_pInput_2 = g_pMetaCanvas;
		g_pCSMC->m_pOutput_1 = g_Manager.GetBackBuffer();
		g_pCSMC->m_pOutput_2 = pMetaCanvasOut;
		g_pCSMC->m_Params.cursor_posX = s_mnx;
		g_pCSMC->m_Params.cursor_posY = s_mny;
		g_pCSMC->m_Params.cursor_prev_posX = s_prev_mnx;
		g_pCSMC->m_Params.cursor_prev_posY = s_prev_mny;
		g_pCSMC->m_Params.brush_size = brush_size;
		int dx = s_prev_mnx - s_mnx;
		int dy = s_prev_mny - s_mny;
		if (dx != 0)
		{
			g_pCSMC->m_Params.m = (float)dy / (float)dx;
		}
		else
		{
			g_pCSMC->m_Params.m = 0;
		}
		g_pCSMC->m_Params.click = click;
		g_pCSMC->Configure();
		g_pCSMC->Execute();


		g_pCSALU->m_pInput_1 = pMetaCanvasOut;
		g_pCSALU->m_pOutput = g_pMetaCanvas;
		g_pCSALU->Configure((ALU_OPERATION)ALU_COPY);
		g_pCSALU->Execute();*/

		



		//Procesar
		
		/*g_pCSConvolve->m_pInput = g_pSource;
		g_pCSConvolve->m_pOutput = pDefaultOut;
		MATRIX4D S = Scale(s_fScale, s_fScale, 1);
		MATRIX4D R = RotationZ(s_fTheta);
		MATRIX4D T = Translate(s_mnx, s_mny, 0);

		g_pCSConvolve->m_Params.Kernel = g_pCSConvolve->getKernelLaplace();
		g_pCSConvolve->m_Params.C = 0.5;
		g_pCSConvolve->Configure();
		g_pCSConvolve->Execute();*/

		/*g_pCSDefault->m_pInput = g_pSource;
		g_pCSDefault->m_pOutput = pDefaultOut;
		MATRIX4D S = Scale(s_fScale, s_fScale, 1);
		MATRIX4D R = RotationZ(s_fTheta);
		MATRIX4D T = Translate(s_mnx, s_mny, 0);

		g_pCSDefault->m_Params.M = Inverse(S*R*T);
		g_pCSDefault->Configure();
		g_pCSDefault->Execute(); */

		/*g_pIC->m_pInput_1 = g_pSource;
		g_pIC->m_pInput_2 = g_pStaticVideoImage;
		g_pIC->m_pOutput = pDefaultOut;
		g_pIC->Configure();
		g_pIC->Execute();*/

		/*#pragma region Convolve
		g_pCSConvolve->m_pInput = g_pSource;
		g_pCSConvolve->m_pOutput = pConvolveOut;
		g_pCSConvolve->m_Params.Kernel = g_pCSConvolve->getKernelSharp(s_fTime);
		g_pCSConvolve->m_Params.C = 0;
		g_pCSConvolve->Configure();
		g_pCSConvolve->Execute();
		#pragma endregion

		//ALU_Thresholds
		g_pCSALU->m_pInput_1 = pDefaultOut;
		g_pCSALU->m_pInput_2 = pConvolveOut;
		//g_pCSALU->m_Params.m_Threshold = { 0,0,0,0 };
		g_pCSALU->m_Params.Threshold = 0.4;
		g_pCSALU->m_pOutput = g_Manager.GetBackBuffer();
		g_pCSALU->Configure((ALU_OPERATION)ALU_op);
		g_pCSALU->Execute();*/

		s_prev_mnx = s_mnx;
		s_prev_mny = s_mny;

		//Liberar toda memoria intermedia al terminar de procesar
		//SAFE_RELEASE(pFusionOut);
		//SAFE_RELEASE(pMetaCanvasOut);
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

CDXGIManager::PIXEL alpha(CDXGIManager::PIXEL p)
{
	CDXGIManager::PIXEL pAlpha = p;
	pAlpha.a = p.b;
	return pAlpha;
}

LRESULT WINAPI VideoHost(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		return 0;
	case WM_SIZE:
	{
		RECT rc;
		GetClientRect(hWnd, &rc);
		g_pIVC->SetPreviewWindowPosition(&rc);
	}
	break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}