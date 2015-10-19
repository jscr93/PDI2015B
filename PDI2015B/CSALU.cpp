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
	m_pOwner = pOwner;
}

bool CCSALU::Initialize()
{
	if (!(m_pCS_Copy =			m_pOwner->CompileCS(L"..\\Shaders\\ALU_Copy.hlsl", "Main"))) return false;
	if (!(m_pCS_Neg =			m_pOwner->CompileCS(L"..\\Shaders\\ALU_Neg.hlsl", "Main"))) return false;
	if (!(m_pCS_AND =			m_pOwner->CompileCS(L"..\\Shaders\\ALU_AND.hlsl", "Main"))) return false;
	if (!(m_pCS_OR =			m_pOwner->CompileCS(L"..\\Shaders\\ALU_OR.hlsl", "Main"))) return false;
	if (!(m_pCS_XOR =			m_pOwner->CompileCS(L"..\\Shaders\\ALU_XOR.hlsl", "Main"))) return false;
	if (!(m_pCS_SADD =			m_pOwner->CompileCS(L"..\\Shaders\\ALU_SADD.hlsl", "Main"))) return false;
	if (!(m_pCS_SSUB =			m_pOwner->CompileCS(L"..\\Shaders\\ALU_SSUB.hlsl", "Main"))) return false;
	if (!(m_pCS_MOD =			m_pOwner->CompileCS(L"..\\Shaders\\ALU_MOD.hlsl", "Main"))) return false;
	if (!(m_pCS_ALPHAS0 =		m_pOwner->CompileCS(L"..\\Shaders\\ALU_ALPHAS0.hlsl", "Main"))) return false;
	if (!(m_pCS_ALPHAS1 =		m_pOwner->CompileCS(L"..\\Shaders\\ALU_ALPHAS1.hlsl", "Main"))) return false;
	/*if (!(m_pCS_HP_TRESHOLD =	m_pOwner->CompileCS(L"..\\Shaders\\ALU_HP_THRESHOLD.hlsl", "Main"))) return false;
	if (!(m_pCS_LP_THRESHOLD =	m_pOwner->CompileCS(L"..\\Shaders\\ALU_LP_THRESHOLD.hlsl", "Main"))) return false;*/
	if (!(m_pCS_MERGE =			m_pOwner->CompileCS(L"..\\Shaders\\ALU_MERGE.hlsl", "Main"))) return false;

	return true;
}

bool CCSALU::Configure(ALU_OPERATION op)
{
	//instalar el shader y conectar recursos
	ID3D11ComputeShader ** m_pCS = 0; //Puntero a los demas punteros compute shaders


	bool multipleArgs = true;

	switch (op)
	{
	case CCSALU::ALU_COPY:			m_pCS = &m_pCS_Copy; multipleArgs = false; break;
	case CCSALU::ALU_NEG:			m_pCS = &m_pCS_Neg; multipleArgs = false; break;
	case CCSALU::ALU_AND:			m_pCS = &m_pCS_AND; break;
	case CCSALU::ALU_OR:			m_pCS = &m_pCS_OR; break;
	case CCSALU::ALU_XOR:			m_pCS = &m_pCS_XOR; break;
	case CCSALU::ALU_SADD:			m_pCS = &m_pCS_SADD; break;
	case CCSALU::ALU_SSUB:			m_pCS = &m_pCS_SSUB; break;
	case CCSALU::ALU_MOD:			m_pCS = &m_pCS_MOD; break;
	case CCSALU::ALU_ALPHAS0:		m_pCS = &m_pCS_ALPHAS0; break;
	case CCSALU::ALU_ALPHAS1:		m_pCS = &m_pCS_ALPHAS1; break;
	case CCSALU::ALU_HP_THRESHOLD:	m_pCS = &m_pCS_HP_TRESHOLD; break;
	case CCSALU::ALU_LP_THRESHOLD:	m_pCS = &m_pCS_LP_THRESHOLD; break;
	case CCSALU::ALU_MERGE:			m_pCS = &m_pCS_MERGE; break;
	default: return false;
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

CCSALU::~CCSALU()
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