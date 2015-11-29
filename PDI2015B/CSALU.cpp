#include "stdafx.h"
#include "CSALU.h"


CCSALU::CCSALU(CDXGIManager *pOwner)
{
	m_pCS_Copy			 = 0;
	m_pCS_Neg			 = 0;
	m_pCS_AND			 = 0;
	m_pCS_OR			 = 0;
	m_pCS_XOR			 = 0;
	m_pCS_SADD			 = 0;
	m_pCS_SSUB			 = 0;
	m_pCS_MOD			 = 0;
	m_pCS_ALPHAS0		 = 0;
	m_pCS_ALPHAS1		 = 0;
	m_pCS_HP_TRESHOLD	 = 0;
	m_pCS_LP_THRESHOLD	 = 0;
	m_pCS_MERGE			 = 0;

	m_pInput_1 = 0;
	m_pInput_2 = 0;
	m_pOutput = 0;
	m_pCB = 0;
	m_pOwner = pOwner;
}

bool CCSALU::Initialize()
{
	if (!(m_pCS_Copy =			m_pOwner->CompileCS(L"..\\Shaders\\ALU_Copy.hlsl", "Main"))) {
		ReleaseShaders();
		return false;
	}
	if (!(m_pCS_Neg =			m_pOwner->CompileCS(L"..\\Shaders\\ALU_Neg.hlsl", "Main"))) {
		ReleaseShaders();
		return false;
	}
	if (!(m_pCS_AND =			m_pOwner->CompileCS(L"..\\Shaders\\ALU_AND.hlsl", "Main"))) {
		ReleaseShaders();
		return false;
	}
	if (!(m_pCS_OR =			m_pOwner->CompileCS(L"..\\Shaders\\ALU_OR.hlsl", "Main"))) {
		ReleaseShaders();
		return false;
	}
	if (!(m_pCS_XOR =			m_pOwner->CompileCS(L"..\\Shaders\\ALU_XOR.hlsl", "Main"))) {
		ReleaseShaders();
		return false;
	}
	if (!(m_pCS_SADD =			m_pOwner->CompileCS(L"..\\Shaders\\ALU_SADD.hlsl", "Main"))) {
		ReleaseShaders();
		return false;
	}
	if (!(m_pCS_SSUB =			m_pOwner->CompileCS(L"..\\Shaders\\ALU_SSUB.hlsl", "Main"))) {
		ReleaseShaders();
		return false;
	}
	if (!(m_pCS_MOD =			m_pOwner->CompileCS(L"..\\Shaders\\ALU_MOD.hlsl", "Main"))) {
		ReleaseShaders();
		return false;
	}
	if (!(m_pCS_ALPHAS0 =		m_pOwner->CompileCS(L"..\\Shaders\\ALU_ALPHAS0.hlsl", "Main"))) {
		ReleaseShaders();
		return false;
	}
	if (!(m_pCS_ALPHAS1 =		m_pOwner->CompileCS(L"..\\Shaders\\ALU_ALPHAS1.hlsl", "Main"))) {
		ReleaseShaders();
		return false;
	}
	if (!(m_pCS_HP_TRESHOLD =	m_pOwner->CompileCS(L"..\\Shaders\\ALU_HP_THRESHOLD.hlsl", "Main"))) {
		ReleaseShaders();
		return false;
	}
	if (!(m_pCS_LP_THRESHOLD =	m_pOwner->CompileCS(L"..\\Shaders\\ALU_LP_THRESHOLD.hlsl", "Main"))) {
		ReleaseShaders();
		return false;
	}
	if (!(m_pCS_MERGE =			m_pOwner->CompileCS(L"..\\Shaders\\ALU_MERGE.hlsl", "Main"))) {
		ReleaseShaders();
		return false;
	}

	D3D11_BUFFER_DESC dbd;
	memset(&dbd, 0, sizeof(dbd));
	dbd.ByteWidth = 16 * ((sizeof(PARAMS) + 15) / 16); //multiplos 128 bits o 16 bytes
	dbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	dbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dbd.Usage = D3D11_USAGE_DYNAMIC;
	HRESULT hr = m_pOwner->GetDevice()->CreateBuffer(&dbd, NULL, &m_pCB);
	if (FAILED(hr))
	{
		ReleaseShaders();
		return false;
	}
	return true;
}

bool CCSALU::Configure(ALU_OPERATION op)
{
	//instalar el shader y conectar recursos
	ID3D11ComputeShader ** m_pCS = 0; //Puntero a los demas punteros compute shaders


	bool multipleArgs = true;

	switch (op)
	{
	case ALU_COPY:			m_pCS = &m_pCS_Copy; multipleArgs = false; break;
	case ALU_NEG:			m_pCS = &m_pCS_Neg; multipleArgs = false; break;
	case ALU_AND:			m_pCS = &m_pCS_AND; break;
	case ALU_OR:			m_pCS = &m_pCS_OR; break;
	case ALU_XOR:			m_pCS = &m_pCS_XOR; break;
	case ALU_SADD:			m_pCS = &m_pCS_SADD; break;
	case ALU_SSUB:			m_pCS = &m_pCS_SSUB; break;
	case ALU_MOD:			m_pCS = &m_pCS_MOD; break;
	case ALU_ALPHAS0:		m_pCS = &m_pCS_ALPHAS0; break;
	case ALU_ALPHAS1:		m_pCS = &m_pCS_ALPHAS1; break;
	case ALU_HP_THRESHOLD:	m_pCS = &m_pCS_HP_TRESHOLD; multipleArgs = false; UpdateConstantBuffer(); break;
	case ALU_LP_THRESHOLD:	m_pCS = &m_pCS_LP_THRESHOLD; multipleArgs = false;  UpdateConstantBuffer(); break;
	case ALU_MERGE:			m_pCS = &m_pCS_MERGE; break;
	default:				m_pCS = &m_pCS_Copy; multipleArgs = false; break;
	}

	ID3D11ShaderResourceView *pSRV[2] = { 0,0 };
	ID3D11UnorderedAccessView *pUAV = 0;

	int nInputs = 1;
	m_pOwner->GetDevice()->CreateShaderResourceView(m_pInput_1, NULL, &pSRV[0]);
	if (multipleArgs) {
		m_pOwner->GetDevice()->CreateShaderResourceView(m_pInput_2, NULL, &pSRV[1]);
		nInputs++;
	}
	m_pOwner->GetContext()->CSSetShaderResources(0, nInputs, pSRV);

	m_pOwner->GetDevice()->CreateUnorderedAccessView(m_pOutput, NULL, &pUAV);
	m_pOwner->GetContext()->CSSetUnorderedAccessViews(0, 1, &pUAV, NULL);

	m_pOwner->GetContext()->CSSetShader(*m_pCS, NULL, NULL);

	SAFE_RELEASE(pSRV[0]);
	SAFE_RELEASE(pSRV[1]);
	SAFE_RELEASE(pUAV);
	return true;
}

void CCSALU::Execute()
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

void CCSALU::UpdateConstantBuffer()
{
	//Actualizacion del constant buffer
	PARAMS Params = m_Params;
	D3D11_MAPPED_SUBRESOURCE ms;
	m_pOwner->GetContext()->Map(m_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	memcpy(ms.pData, &Params, sizeof(PARAMS));
	m_pOwner->GetContext()->Unmap(m_pCB, 0);
	//usar el constant buffer
	m_pOwner->GetContext()->CSSetConstantBuffers(0, 1, &m_pCB);
}

void CCSALU::ReleaseShaders()
{
	SAFE_RELEASE(m_pCS_Copy);
	SAFE_RELEASE(m_pCS_Neg);
	SAFE_RELEASE(m_pCS_AND);
	SAFE_RELEASE(m_pCS_OR);
	SAFE_RELEASE(m_pCS_XOR);
	SAFE_RELEASE(m_pCS_SADD);
	SAFE_RELEASE(m_pCS_SSUB);
	SAFE_RELEASE(m_pCS_MOD);
	SAFE_RELEASE(m_pCS_ALPHAS0);
	SAFE_RELEASE(m_pCS_ALPHAS1);
	SAFE_RELEASE(m_pCS_HP_TRESHOLD);
	SAFE_RELEASE(m_pCS_LP_THRESHOLD);
	SAFE_RELEASE(m_pCS_MERGE);
}

CCSALU::~CCSALU()
{
	ReleaseShaders();
	SAFE_RELEASE(m_pCB);
}

