#pragma once
#include<d3d11.h>
#include"Matrix4D.h"
#include"DXGIManager.h"
class CCSFusion
{
protected:
	ID3D11ComputeShader * m_pCS;
	ID3D11Buffer		* m_pCB;

	CDXGIManager		* m_pOwner;
public:
	struct PARAMS
	{
		int cursor_posX, cursor_posY;
		int brush_size;
	}m_Params;
	ID3D11Texture2D * m_pInput_1;
	ID3D11Texture2D * m_pInput_2;
	ID3D11Texture2D * m_pInput_3;
	ID3D11Texture2D * m_pOutput;

	CCSFusion(CDXGIManager *pOwner);
	~CCSFusion();
	bool Initialize();//Compilar los Shaders desde hlsl -> dxgi->native
	void Configure();//configurar la tuberia de procesamiento
	void Execute();// Despachar la cantidad de hilos necesarios 
};

