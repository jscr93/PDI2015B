#pragma once
#include <dxgi.h>
#include <d3d11.h>
#include "Frame.h"
#include "UFrame.h"
#ifndef SAFE_RELEASE
	#define SAFE_RELEASE(X) if((X)){ (X)->Release();(X)=0; }
#endif

class CDXGIManager
{
protected:
	ID3D11Device* m_pDevice; //Rescurso de hardware/software de computacion (Factory)
	ID3D11DeviceContext* m_pContext; //Cola de comandos del dispositivo
	IDXGISwapChain* m_pSwapChain; //Cadena de intercambio

	ID3D11Texture2D* m_pBackBuffer; //BackBuffer en swapchain

public:
	struct PIXEL
	{
		unsigned char r, g, b, a;
	};

	bool Initialize(HWND hWnd, IDXGIAdapter* pAdapter, bool bFullScreen);
	ID3D11ComputeShader* CompileCS(wchar_t* pszFileName, char* pszEntryPoint);
	void Unitialize(void);
	void Resize(int sx, int sy);
	ID3D11Device* GetDevice() { return m_pDevice; }
	ID3D11DeviceContext* GetContext() { return m_pContext; }
	IDXGISwapChain* GetSwapChain() { return m_pSwapChain; }
	ID3D11Texture2D* GetBackBuffer() { return m_pBackBuffer; }
	IDXGIAdapter* EnumAdapters(HWND hWnd);
	ID3D11Texture2D* LoadTexture(
		char* pszFileName, 
		int nMipMapLevels=-1, 
		PIXEL (*pPixel)(PIXEL) = NULL);
	ID3D11Texture2D* LoadTexture(CFrame*);
	ID3D11Texture2D* LoadWhiteTextureOfSize(CFrame*);
	CUFrame* LoadTextureBack(ID3D11Texture2D*);
	CDXGIManager();
	~CDXGIManager();
};

