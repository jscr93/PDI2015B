#pragma once
#include"DXGIManager.h"
class CCSALU
{
protected:
	//Logicas
	ID3D11ComputeShader * m_pCS_Copy;
	ID3D11ComputeShader * m_pCS_Neg;
	ID3D11ComputeShader * m_pCS_AND;
	ID3D11ComputeShader * m_pCS_OR;
	ID3D11ComputeShader * m_pCS_XOR;

	//Aritmeticas
	ID3D11ComputeShader * m_pCS_SADD;
	ID3D11ComputeShader * m_pCS_SSUB;
	ID3D11ComputeShader * m_pCS_MOD;
	ID3D11ComputeShader * m_pCS_ALPHAS0;
	ID3D11ComputeShader * m_pCS_ALPHAS1;
	ID3D11ComputeShader * m_pCS_HP_TRESHOLD;
	ID3D11ComputeShader * m_pCS_LP_THRESHOLD;
	ID3D11ComputeShader * m_pCS_MERGE;

	CDXGIManager * m_pOwner;
public:
	enum ALU_OPERATION { ALU_COPY, ALU_NEG, ALU_AND, ALU_OR, ALU_XOR, ALU_SADD, ALU_SSUB, ALU_MOD, ALU_ALPHAS0, ALU_ALPHAS1, ALU_HP_THRESHOLD, ALU_LP_THRESHOLD, ALU_MERGE };

	ID3D11Texture2D * m_pInput_1;
	ID3D11Texture2D * m_pInput_2;
	ID3D11Texture2D * m_pOutput;

	CCSALU(CDXGIManager *pOwner);
	~CCSALU();

	bool Initialize();
	bool Configure(ALU_OPERATION);

	
};

