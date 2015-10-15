#include "stdafx.h"
#include "CSDefault.h"


CCSDefault::CCSDefault(CDXGIManager *pOwner)
{
	m_pCB = 0;
	m_pCS = 0;
	m_pInput = 0;
	m_pOutput = 0;
	m_pOwner = pOwner;
}


CCSDefault::~CCSDefault()
{
	SAFE_RELEASE(m_pCS);
	SAFE_RELEASE(m_pCB);
}


bool CCSDefault::Initialize()
{
	m_pCS = m_pOwner->CompileCS(L"..\\Shaders\\Default.hlsl", "Main");
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

void CCSDefault::Configure()
{
	//Actualizacion del constant buffer
	PARAMS Params = m_Params;
	Params.M = Transpose(m_Params.M);
	D3D11_MAPPED_SUBRESOURCE ms;
	D3D11_TEXTURE2D_DESC dtd;
	m_pInput->GetDesc(&dtd);
	Params.invInputSizeX = 1.0f / dtd.Width;
	Params.invInputSizeY = 1.0f / dtd.Height;
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
}

void CCSDefault::Execute()
{
	D3D11_TEXTURE2D_DESC dtd;
	m_pOutput->GetDesc(&dtd);
	int gx, gy;
	gx = (dtd.Width + 7) / 8;
	gy = (dtd.Height + 7) / 8;
	m_pOwner->GetContext()->Dispatch(gx, gy, 1);
	m_pOwner->GetContext()->Flush();//finish queued jobs and free temporal resources
	m_pOwner->GetContext()->ClearState();//reset GPU and free all references
}
