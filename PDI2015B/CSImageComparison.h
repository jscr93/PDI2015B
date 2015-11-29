#pragma once
#include<d3d11.h>
#include"Matrix4D.h"
#include"DXGIManager.h"
class CCSImageComparison
{
protected:
	ID3D11ComputeShader * m_pCS;
	//ID3D11Buffer		* m_pCB;

	CDXGIManager		* m_pOwner;
public:
	//struct PARAMS
	//{
	//	/*MATRIX4D M;
	//	float Time;
	//	int Flag;
	//	float invInputSizeX, invInputSizeY;*/
	//}m_Params;
	ID3D11Texture2D * m_pInput_1;
	ID3D11Texture2D * m_pInput_2;
	ID3D11Texture2D * m_pOutput;

	CCSImageComparison(CDXGIManager *pOwner);
	~CCSImageComparison();
	bool Initialize();//Compilar los Shaders desde hlsl -> dxgi->native
	void Configure();//configurar la tuberia de procesamiento
	void Execute();// Despachar la cantidad de hilos necesarios 
};

