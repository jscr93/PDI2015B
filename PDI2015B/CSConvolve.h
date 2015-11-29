#pragma once
#include "DXGIManager.h"
#include"Matrix4D.h"

class CCSConvolve
{
protected:
	CDXGIManager	   * m_pOwner;
	ID3D11ComputeShader* m_pCS;
	ID3D11Buffer* m_pCB;
public:
	ID3D11Texture2D* m_pInput;
	ID3D11Texture2D* m_pOutput;
	struct PARAMS
	{
		MATRIX4D Kernel;
		float C;
	}m_Params;
	//Convolucion integral del producto de dos funciones ...
public:
	static MATRIX4D getKernelIdentity();
	static MATRIX4D getKernelInvert();
	static MATRIX4D getKernelSoft();
	static MATRIX4D getKernelLaplace();
	static MATRIX4D getKernelLaplace_M1();
	static MATRIX4D getKernelEmbossV();
	static MATRIX4D getKernelSharp(float);

	CCSConvolve(CDXGIManager *pOwner);
	bool Initialize();
	void Configure();
	void Execute();
	~CCSConvolve();
};

