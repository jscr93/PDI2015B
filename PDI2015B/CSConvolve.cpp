#include "stdafx.h"
#include "CSConvolve.h"
#include <math.h>

CCSConvolve::CCSConvolve(CDXGIManager* pOwner)
{
	m_pOwner = pOwner;
	m_pCS = 0;
	m_pInput = 0;
	m_pOutput = 0;
	m_pCB = 0;	 
}

bool CCSConvolve::Initialize()
{
	m_pCS = m_pOwner->CompileCS(L"..\\Shaders\\Convolve.hlsl", "Main");
	if (!m_pCS)
		return false;
	D3D11_BUFFER_DESC dbd;
	memset(&dbd, 0, sizeof(dbd));
	dbd.ByteWidth = 16 * ((sizeof(PARAMS) + 15) / 16); //multiplos 128 bits o 16 bytes
	dbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	dbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dbd.Usage = D3D11_USAGE_DYNAMIC;
	HRESULT hr = m_pOwner->GetDevice()->CreateBuffer(&dbd, NULL, &m_pCB);
	if (FAILED(hr))
	{
		SAFE_RELEASE(m_pCS);
		return false;
	}
	return true;
}

void CCSConvolve::Configure()
{
	//Actualizacion del constant buffer
	PARAMS Params = m_Params;
	D3D11_MAPPED_SUBRESOURCE ms;
	m_pOwner->GetContext()->Map(m_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	memcpy(ms.pData, &Params, sizeof(PARAMS));
	m_pOwner->GetContext()->Unmap(m_pCB, 0);
	//usar el constant buffer
	m_pOwner->GetContext()->CSSetConstantBuffers(0, 1, &m_pCB);
	//m_pOwner->GetContext()->Unmap(m_pCB, 0);

	//instalar el shader y conectar recursos
	ID3D11ShaderResourceView *pSRV = 0;
	ID3D11UnorderedAccessView *pUAV = 0;
	m_pOwner->GetDevice()->CreateShaderResourceView(m_pInput, NULL, &pSRV);
	m_pOwner->GetDevice()->CreateUnorderedAccessView(m_pOutput, NULL, &pUAV);
	m_pOwner->GetContext()->CSSetShaderResources(0, 1, &pSRV);
	m_pOwner->GetContext()->CSSetUnorderedAccessViews(0, 1, &pUAV, NULL);
	m_pOwner->GetContext()->CSSetShader(m_pCS, NULL, NULL);

	SAFE_RELEASE(pSRV);
	SAFE_RELEASE(pUAV);
}

void CCSConvolve::Execute() {
	D3D11_TEXTURE2D_DESC dtd;
	m_pOutput->GetDesc(&dtd);
	int gx, gy;
	gx = (dtd.Width + 7) / 8;
	gy = (dtd.Height + 7) / 8;
	m_pOwner->GetContext()->Dispatch(gx, gy, 1);
	m_pOwner->GetContext()->Flush();//finish queued jobs and free temporal resources
	m_pOwner->GetContext()->ClearState();//reset GPU and free all references
}

MATRIX4D CCSConvolve::getKernelIdentity() {
	return MATRIX4D { 0,0,0,0,
	0,1,0,0,
	0,0,0,0,
	0,0,0,0 }; // C=0 o cualquier valor
}

MATRIX4D CCSConvolve::getKernelInvert() {
	return MATRIX4D { 0,0,0,0,
	0,-1,0,0,
	0,0,0,0,
	0,0,0,0 }; // C=1
}

MATRIX4D CCSConvolve::getKernelSoft() {
	return MATRIX4D { 1 / 9.0f,1 / 9.0f,1 / 9.0f,0,
	1 / 9.0f,1 / 9.0f,1 / 9.0f,0,
	1 / 9.0f,1 / 9.0f,1 / 9.0f,0,
	0,0,0,0 };
}

MATRIX4D CCSConvolve::getKernelLaplace() {
	return MATRIX4D {	0, -1 / 8.0f,  0, 0,
						-1 / 8.0f, 1 / 2.0f, -1 / 8.0f, 0,
						0, -1 / 8.0f, 0, 0,
						0, 0 , 0, 0 };//C=0.5
}


MATRIX4D CCSConvolve::getKernelEmbossV() {
	return MATRIX4D { -1, -1, -1, 0,
	0, 0, 0, 0,
	1, 1, 1, 1,
	0, 0, 0, 0 };//C = 0.5
}

MATRIX4D CCSConvolve::getKernelSharp(float time) {
	float Strength = fabs(cos(time));
	float c = 1 - Strength;
	return MATRIX4D { 0, c , 0, 0,
	c, 1, -c, 0,
	0, -c, 0, 0,
	0, 0, 0, 0 };//C = 0
}


CCSConvolve::~CCSConvolve()
{
	SAFE_RELEASE(m_pCS);
	SAFE_RELEASE(m_pCB);
}

