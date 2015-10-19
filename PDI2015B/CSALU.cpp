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


CCSALU::~CCSALU()
{
}

bool CCSALU::Initialize()
{
	if (!(m_pCS_Copy =			m_pOwner->CompileCS(L"..\\Shaders\\ALU_Copy.hlsl", "Main"))) return false;
	/*if (!(m_pCS_Neg =			m_pOwner->CompileCS(L"..\\Shaders\\ALU_Neg.hlsl", "Main"))) return false;
	if (!(m_pCS_AND =			m_pOwner->CompileCS(L"..\\Shaders\\ALU_AND.hlsl", "Main"))) return false;
	if (!(m_pCS_OR =			m_pOwner->CompileCS(L"..\\Shaders\\ALU_OR.hlsl", "Main"))) return false;
	if (!(m_pCS_XOR =			m_pOwner->CompileCS(L"..\\Shaders\\ALU_XOR.hlsl", "Main"))) return false;
	if (!(m_pCS_SADD =			m_pOwner->CompileCS(L"..\\Shaders\\ALU_SADD.hlsl", "Main"))) return false;
	if (!(m_pCS_SSUB =			m_pOwner->CompileCS(L"..\\Shaders\\ALU_SADD.hlsl", "Main"))) return false;
	if (!(m_pCS_MOD =			m_pOwner->CompileCS(L"..\\Shaders\\ALU_MOD.hlsl", "Main"))) return false;
	if (!(m_pCS_ALPHAS0 =		m_pOwner->CompileCS(L"..\\Shaders\\ALU_ALPHAS0.hlsl", "Main"))) return false;
	if (!(m_pCS_ALPHAS1 =		m_pOwner->CompileCS(L"..\\Shaders\\ALU_ALPHAS1.hlsl", "Main"))) return false;
	if (!(m_pCS_HP_TRESHOLD =	m_pOwner->CompileCS(L"..\\Shaders\\ALU_HP_THRESHOLD.hlsl", "Main"))) return false;
	if (!(m_pCS_LP_THRESHOLD =	m_pOwner->CompileCS(L"..\\Shaders\\ALU_LP_THRESHOLD.hlsl", "Main"))) return false;
	if (!(m_pCS_MERGE =			m_pOwner->CompileCS(L"..\\Shaders\\ALU_MERGE.hlsl", "Main"))) return false;*/

	return true;
}

bool CCSALU::Configure(ALU_OPERATION op)
{
	//instalar el shader y conectar recursos
	ID3D11ComputeShader * m_pCS = 0;
	switch (op)
	{
	case CCSALU::ALU_COPY:			m_pCS = m_pCS_Copy; break;
	case CCSALU::ALU_NEG:			m_pCS = m_pCS_Neg; break;
	case CCSALU::ALU_AND:			m_pCS = m_pCS_AND; break;
	case CCSALU::ALU_OR:			m_pCS = m_pCS_OR; break;
	case CCSALU::ALU_XOR:			m_pCS = m_pCS_XOR; break;
	case CCSALU::ALU_SADD:			m_pCS = m_pCS_SADD; break;
	case CCSALU::ALU_SSUB:			m_pCS = m_pCS_SSUB; break;
	case CCSALU::ALU_MOD:			m_pCS = m_pCS_MOD; break;
	case CCSALU::ALU_ALPHAS0:		m_pCS = m_pCS_ALPHAS0; break;
	case CCSALU::ALU_ALPHAS1:		m_pCS = m_pCS_ALPHAS1; break;
	case CCSALU::ALU_HP_THRESHOLD:	m_pCS = m_pCS_HP_TRESHOLD; break;
	case CCSALU::ALU_LP_THRESHOLD:	m_pCS = m_pCS_LP_THRESHOLD; break;
	case CCSALU::ALU_MERGE:			m_pCS = m_pCS_MERGE; break;
	default: return false;
	}
	ID3D11ShaderResourceView *pSRV = 0;
	ID3D11UnorderedAccessView *pUAV = 0;
	m_pOwner->GetDevice()->CreateShaderResourceView(m_pInput_1, NULL, &pSRV);
	m_pOwner->GetDevice()->CreateShaderResourceView(m_pInput_2, NULL, &pSRV);
	m_pOwner->GetDevice()->CreateUnorderedAccessView(m_pOutput, NULL, &pUAV);
	m_pOwner->GetContext()->CSSetShaderResources(0, 1, &pSRV);
	m_pOwner->GetContext()->CSSetUnorderedAccessViews(0, 1, &pUAV, NULL);
	m_pOwner->GetContext()->CSSetShader(m_pCS, NULL, NULL);
	return true;
}