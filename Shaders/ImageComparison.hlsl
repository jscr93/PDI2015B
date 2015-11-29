Texture2D<float4>	Input_0:register(t0);		// SRV
Texture2D<float4>	Input_1:register(t1);		// SRV
RWTexture2D<float4>	Output:register(u0);	// UAV

[numthreads(8, 8, 1)]
void Main(uint3 id:SV_DispatchThreadID)
{
	//Output[id.xy] = abs(Input_0[id.xy] - Input_1[id.xy]) < 0.1 ? float4(0, 0, 0, 0) : Input_0[id.xy];
	
	float4 Color = 0;
	for (int j = 0; j < 3; j++)
	{
		for (int i = 0; i < 3; i++)
		{
			int2 coord = int2(i - 1, j - 1) + int2(id.xy);
			Color = abs(Input_0[id.xy] - Input_0[coord]) > 0.03? float4(1, 0, 0, 0) : Input_0[id.xy];
		}
	}
	Output[id.xy] = Color;

	//Output[id.xy] = (Input_0[id.xy] > 0.53) ? float4(1, 0, 0, 0) : Input_0[id.xy];
}

