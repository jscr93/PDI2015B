Texture2D<float4> Input:register(t0);
RWTexture2D<float4> Output:register(u0);

cbuffer PARAMS
{
	matrix Kernel;
	float C;
};

[numthreads(8,8,1)]
void Main(uint3 id:SV_DispatchThreadID)
{
	float4 Color = 0;
	for (int j = 0; j < 3; j++)
		for (int i = 0; i < 3; i++)
		{
			int2 coord = int2(i - 1, j - 1) + int2(id.xy);
			Color += Kernel[j][i] * Input[coord];
		}
	//if (abs(dot(Color.rgb, float3(0.3, 0.3, 0.3)))>0.003)
	//	Output[id.xy] = 0;
	//else {
	//	Output[id.xy] = Input[id.xy];
	//	//Output[id.xy] = Color + C;
	//}
	Output[id.xy] = Color + C;
}