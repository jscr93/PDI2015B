#include "stdafx.h"
#include "DXGIManager.h"
#include <d3dcompiler.h>
#include <fstream>
using namespace std;

CDXGIManager::CDXGIManager()
{
	m_pDevice = 0;
	m_pContext = 0;
	m_pSwapChain = 0;
	m_pBackBuffer = 0;

}


CDXGIManager::~CDXGIManager()
{
}

void CDXGIManager::Unitialize(void) {
	if (m_pSwapChain)
		m_pSwapChain->SetFullscreenState(false, 0);
	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pBackBuffer);
}


IDXGIAdapter* CDXGIManager::EnumAdapters(HWND hWnd) {
	IDXGIFactory* pFactory = NULL;
	HRESULT hr = CreateDXGIFactory(IID_IDXGIFactory, (void**)&pFactory);
	if (FAILED(hr)) {
		MessageBox(hWnd, L"Drop your computer and buy another...", L"Fatal Error", MB_ICONERROR);
		return NULL;
	}
	int iAdapter = 0;
	IDXGIAdapter* pAdapter = 0;
	while (1) {
		DXGI_ADAPTER_DESC dad;
		if (FAILED(pFactory->EnumAdapters(iAdapter, &pAdapter))) {
			break;
		}
		TCHAR szMessage[1024];
		pAdapter->GetDesc(&dad);
		wsprintf(szMessage, L"Description:%s\r\nVideo Memory:%d MB\r\nShared Memory:%d MB\r\nSystem Memory%d MB"
			, dad.Description
			, dad.DedicatedVideoMemory/(1024*1024)
			, dad.SharedSystemMemory/(1024*1024)
			, dad.DedicatedSystemMemory/(1024*1024));
		switch (MessageBox(hWnd, szMessage, L"Do you wish to use this device?",
			MB_YESNOCANCEL | MB_ICONQUESTION)) {
		case IDYES:
			SAFE_RELEASE(pFactory);
			return pAdapter;
		case IDNO:
			break;
		case IDCANCEL:
			SAFE_RELEASE(pFactory);
			SAFE_RELEASE(pAdapter);
			return NULL;
		}
		iAdapter++;
	}
	return NULL;
}

bool CDXGIManager::Initialize(
	HWND hWnd,
	IDXGIAdapter* pAdapter,
	bool bFullScreen)
{
	RECT rc;
	GetClientRect(hWnd, &rc);
	DXGI_SWAP_CHAIN_DESC dscd;
	memset(&dscd, 0, sizeof(DXGI_SWAP_CHAIN_DESC));
	dscd.BufferCount = 2;
	dscd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dscd.BufferDesc.Width = rc.right;
	dscd.BufferDesc.Height = rc.bottom;
	dscd.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
	dscd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
	dscd.BufferDesc.RefreshRate.Numerator = 0;
	dscd.BufferDesc.RefreshRate.Denominator = 0;
	dscd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT 
					| DXGI_USAGE_UNORDERED_ACCESS;
	dscd.OutputWindow = hWnd;
	dscd.SampleDesc.Count = 1;
	dscd.SampleDesc.Quality = 0;
	dscd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	dscd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	dscd.Windowed = !bFullScreen;
	D3D_FEATURE_LEVEL Feature = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL DetectedFeature;
	if (pAdapter) 
	{
		//Try to use the adapter
		HRESULT hr =
		D3D11CreateDeviceAndSwapChain(pAdapter, D3D_DRIVER_TYPE_UNKNOWN,
			NULL, 0, &Feature, 1, D3D11_SDK_VERSION, &dscd, &m_pSwapChain,
			&m_pDevice, &DetectedFeature, &m_pContext);
		if (FAILED(hr))
			return false;
		Resize(rc.right, rc.bottom);
		return true;
	}
	else
	{
		//Try to use CPU
		HRESULT hr =
			D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_WARP, 0, 0, &Feature,
				1, D3D11_SDK_VERSION, &dscd, &m_pSwapChain, &m_pDevice, &DetectedFeature, &m_pContext);
		if (FAILED(hr))
			return false;
		Resize(rc.right, rc.bottom);
		return true;
	}
	return false;
}

ID3D11ComputeShader* CDXGIManager::CompileCS(wchar_t* pszFileName, char* pszEntryPoint)
{
	ID3D10Blob* pIL = NULL; //Bytecode del swhader en DXIL
	ID3D10Blob* pErrors = NULL; //Error log buffer
	ID3D11ComputeShader* pCS = NULL; //The complete shader!!!

#ifdef _DEBUG
	DWORD dwOption = D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_ENABLE_STRICTNESS
		| D3DCOMPILE_DEBUG;
#else
	DWORD dwOption = D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_ENABLE_STRICTNESS
#endif

	HRESULT hr =
		D3DCompileFromFile(pszFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE,
			pszEntryPoint, "cs_5_0", dwOption, 0, &pIL, &pErrors);

	if (FAILED(hr)) 
	{
		if (pErrors)
			MessageBoxA(NULL, (char*)pErrors->GetBufferPointer(), "Errores", MB_ICONERROR);
		SAFE_RELEASE(pErrors);
		SAFE_RELEASE(pIL);
		return NULL;
	}
	//Crear el compute shader en codigo nativo
	hr = m_pDevice->CreateComputeShader(pIL->GetBufferPointer(),
		pIL->GetBufferSize(), NULL, &pCS);
	if (FAILED(hr)) {
		SAFE_RELEASE(pIL);
		SAFE_RELEASE(pErrors);
		return NULL;
	}
	SAFE_RELEASE(pErrors);
	SAFE_RELEASE(pIL);
	return pCS;
}

ID3D11Texture2D* CDXGIManager::LoadTexture(
	char* pszFileName,
	int nMipMapLevels, unsigned long(*pPixel)(unsigned long))
{
	struct PIXEL
	{
		unsigned char r, g, b, a;
	};

	fstream in;
	in.open(pszFileName, ios::in | ios::binary);
	if (!in.is_open())
		return NULL;
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
	memset(&bfh, 0, sizeof(bfh));
	memset(&bih, 0, sizeof(bih));
	//1
	in.read((char*)&bfh.bfType, 2); //Se leen los primeros dos bytes

									//int x = 'DCBA'; Aritmetica ASCII
	if ('MB' != bfh.bfType)
		return NULL;
	in.read((char*)&bfh.bfSize, sizeof(bfh) - 2);
	//2 Informacion de encabeado de imagen
	in.read((char*)&bih.biSize, 4);
	if (sizeof(BITMAPINFOHEADER) != bih.biSize)
		return NULL;
	in.read((char*)&bih.biWidth, sizeof(bih) - 4);
	//3 Carga de la informacion de imagen
	unsigned long RowLength = 4*((bih.biBitCount*bih.biWidth + 31)/32);
		//7px x 24bit/px = 168bits + 31 bits => 199 bits /32 => 6*4 -> 24
		//Otro
		//18 a 24 bpp
		//18*24	= 432 + 31 => 463 /32 => 14*4 -> 56

	//4 Crear recurso de andamiaje para CPU->GPU
	//Los recursos de andamiaje están en espacio de CPU
	D3D11_TEXTURE2D_DESC dtd;
	ID3D11Texture2D* pStaging = 0;
	memset(&dtd, 0, sizeof(dtd));
	dtd.ArraySize = 1;
	dtd.BindFlags = 0;
	dtd.CPUAccessFlags = D3D11_CPU_ACCESS_READ 
					   | D3D11_CPU_ACCESS_WRITE;

	dtd.Usage = D3D11_USAGE_STAGING;
	dtd.Height = bih.biHeight;
	dtd.Width = bih.biWidth;
	dtd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dtd.MipLevels = 1;
	dtd.SampleDesc.Count = 1;

	HRESULT hr =
		m_pDevice->CreateTexture2D(&dtd, NULL, &pStaging);
	if (FAILED(hr))
	{
		*(int*)0 = 0;
		return NULL;
	}

	switch (bih.biBitCount)
	{
		case 1:
			{
				RGBQUAD palette[2];
				int nColors = 0 == bih.biClrUsed ? 2 : bih.biClrUsed;
				in.read((char *)palette, nColors * sizeof(RGBQUAD));
				unsigned char * pRow = new unsigned char[RowLength];

				D3D11_MAPPED_SUBRESOURCE ms;
				m_pContext->Map(pStaging, 0, D3D11_MAP_READ_WRITE, 0, &ms);

				for (int j = bih.biHeight - 1; j >= 0; j--)
				{
					PIXEL * pLine = (PIXEL *)((char *)ms.pData + (j * ms.RowPitch));
					in.read((char *)pRow, RowLength);
					int pixelNumberInRow = 0;

					for (int byteNumberInRow = 0; byteNumberInRow < RowLength; byteNumberInRow++)
					{
						unsigned char* readByte = &pRow[byteNumberInRow];
						RGBQUAD c;
						for (int pixelNumberInByte = 7; pixelNumberInByte >= 0; pixelNumberInByte--)
						{
							if (pixelNumberInRow >= bih.biWidth)
								break;
							c = palette[(*readByte >> pixelNumberInByte) & 0x01]; 
							PIXEL color;
							color.r = c.rgbRed;
							color.g = c.rgbGreen;
							color.b = c.rgbBlue;
							color.a = 0xFF;
							pLine[pixelNumberInRow] = color;
							pixelNumberInRow++;
						}
					}
				}
				m_pContext->Unmap(pStaging, 0);
				delete[] pRow;
			}
			break;
		case 4:
			{
				RGBQUAD palette[16];
				int nColors = 0 == bih.biClrUsed ? 16 : bih.biClrUsed;
				in.read((char *)palette, nColors*sizeof(RGBQUAD));
				unsigned char * pRow = new unsigned char[RowLength];

				D3D11_MAPPED_SUBRESOURCE ms;
				m_pContext->Map(pStaging, 0, D3D11_MAP_READ_WRITE, 0, &ms);

				for (int j = bih.biHeight - 1; j >= 0; j--)
				{
					PIXEL * pLine = (PIXEL *)((char *)ms.pData + (j * ms.RowPitch));
					in.read((char *)pRow, RowLength);
					int pixelNumberInRow = 0;

					for (int byteNumberInRow = 0; byteNumberInRow < RowLength; byteNumberInRow++)
					{
						unsigned char* readByte = &pRow[byteNumberInRow];
						RGBQUAD c;
						for (int pixelNumberInByte = 1; pixelNumberInByte >= 0; pixelNumberInByte--)
						{
							if (pixelNumberInRow >= bih.biWidth)
								break;
							c = palette[(*readByte >> (pixelNumberInByte * 4)) & 0x0F]; 
							PIXEL color;
							color.r = c.rgbRed;
							color.g = c.rgbGreen;
							color.b = c.rgbBlue;
							color.a = 0xFF;
							pLine[pixelNumberInRow] = color;
							pixelNumberInRow++;
						}
					}
				}
				m_pContext->Unmap(pStaging, 0);
				delete[] pRow;
			}
			break;
		
		case 8:
			{
				RGBQUAD Palette[256];
				int nColors = 0 == bih.biClrUsed ? 256 : bih.biClrUsed;
				in.read((char*)Palette, nColors*sizeof(RGBQUAD));
				unsigned char *pRow = new unsigned char[RowLength];

				D3D11_MAPPED_SUBRESOURCE ms;
				m_pContext->Map(pStaging, 0, D3D11_MAP_READ_WRITE, 0, &ms);

				for (int j = bih.biHeight - 1; j >= 0; j--) {
					PIXEL *pLine = (PIXEL*)((char*)ms.pData + j*ms.RowPitch);
					in.read((char*)pRow, RowLength);
					for (int i = 0; i < bih.biWidth; i++)
					{
						RGBQUAD C = Palette[pRow[i]];
						PIXEL Color;
						Color.r = C.rgbRed;
						Color.g = C.rgbGreen;
						Color.b = C.rgbBlue;
						Color.a = 0xff;
						pLine[i] = Color;
					}
				}

				m_pContext->Unmap(pStaging, 0);
				delete[] pRow;
			}
			break;
		case 24:
			{
				unsigned char *pRow = new unsigned char[RowLength];

				D3D11_MAPPED_SUBRESOURCE ms;
				m_pContext->Map(pStaging, 0, D3D11_MAP_READ_WRITE, 0, &ms);

				int byteNumberInRow;
				for (int j = bih.biHeight - 1; j >= 0; j--) {
					PIXEL *pLine = (PIXEL*)((char*)ms.pData + j*ms.RowPitch);
					in.read((char*)pRow, RowLength);
					byteNumberInRow = 0;
					for (int pixelNumberInRow = 0; pixelNumberInRow < bih.biWidth; pixelNumberInRow++)
					{
						PIXEL Color;
						Color.b = pRow[byteNumberInRow++];
						Color.g = pRow[byteNumberInRow++];
						Color.r = pRow[byteNumberInRow++];
						Color.a = 0xff;
						pLine[pixelNumberInRow] = Color;
					}
				}

				m_pContext->Unmap(pStaging, 0);
				delete[] pRow;
			}
			break;
		case 32:
			{
				unsigned char *pRow = new unsigned char[RowLength];

				D3D11_MAPPED_SUBRESOURCE ms;
				m_pContext->Map(pStaging, 0, D3D11_MAP_READ_WRITE, 0, &ms);

				int byteNumberInRow;
				for (int j = bih.biHeight - 1; j >= 0; j--) {
					PIXEL *pLine = (PIXEL*)((char*)ms.pData + j*ms.RowPitch);
					in.read((char*)pRow, RowLength);
					byteNumberInRow = 0;
					for (int pixelNumberInRow = 0; pixelNumberInRow < bih.biWidth; pixelNumberInRow++)
					{
						PIXEL Color;
						Color.b = pRow[byteNumberInRow++];
						Color.g = pRow[byteNumberInRow++];
						Color.r = pRow[byteNumberInRow++];
						Color.a = pRow[byteNumberInRow++];
						pLine[pixelNumberInRow] = Color;
					}
				}

				m_pContext->Unmap(pStaging, 0);
				delete[] pRow;
			}
			break;
			break;
	}
	//5 .- Transferir de CPU a GPU
	dtd.Usage = D3D11_USAGE_DEFAULT;
	dtd.CPUAccessFlags = 0;
	dtd.BindFlags = D3D11_BIND_UNORDERED_ACCESS 
		          | D3D11_BIND_SHADER_RESOURCE;//Se pueden hacer varias vistas de un determinado recurso
	ID3D11Texture2D* pTexture = 0;
	hr = m_pDevice->CreateTexture2D(&dtd, NULL, &pTexture);
	if (FAILED(hr))
	{
		SAFE_RELEASE(pStaging);
		return NULL;
	}
	m_pContext->CopyResource(pTexture, pStaging);
	SAFE_RELEASE(pStaging);
	return pTexture;
}


void CDXGIManager::Resize(int sx, int sy) 
{
	m_pContext->ClearState();
	SAFE_RELEASE(m_pBackBuffer);
	m_pSwapChain->ResizeBuffers(2,sx,sy,DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	m_pSwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&m_pBackBuffer);
}