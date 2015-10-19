

cbuffer PARAMS
{
	matrix M;
	float Time;
	int Flag;
	float2 invInputSize;
};

Texture2D<float4>	Input:register(t0);		// SRV
RWTexture2D<float4>	Output:register(u0);	// UAV
//SamplerState		Sampler:register(s0);  //Por default es bilineal.
//Todo lo que termine en state es una pieza de hardware. No es programable, es configurable

float4 BilinearSampler(float2 uv)
{
	uv -= float2(0.5f, 0.5f);//moverse al centro del pixel

	int2 ij = int2(uv);
	float2 pq = uv - ij;

	float4 A, B, C, D;//4 pixeles a interpolar
	A = Input[ij];
	B = Input[ij + int2(1, 0)];
	C = Input[ij + int2(0, 1)];
	D = Input[ij + int2(1, 0)];

	float4 R, S;//interpolaciones entre a-b,c-d
	R = A + pq.x*(B - A);
	S = C + pq.y*(D - C);

	return R + pq.y*(S - R);//interpolado de los interpolados
}

[numthreads(8, 8, 1)]
void Main(uint3 id:SV_DispatchThreadID)
{
	float4 Source = float4(id.xy, 0, 1);
	Source = mul(Source, M);
	//Output[id.xy] = Input[/*id.xy*/int2(Source.xy)]/*+cos(Time)*/;
	//Output[id.xy] = Input.Gather(Sampler, Source.xy*invInputSize);//BilinearSampler(Source.xy);
	Output[id.xy] = BilinearSampler(Source.xy);
	//Input.Gather es una invocacion a pieza de hardware
}

