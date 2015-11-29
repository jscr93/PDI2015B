Texture2D<float4>	Input_0:register(t0);		// SRV
Texture2D<float4>	Input_1:register(t1);		// SRV
RWTexture2D<float4>	Output:register(u0);	// UAV

[numthreads(8, 8, 1)]
void Main(uint3 id:SV_DispatchThreadID)
{
	uint4 iInput_0 = uint4(255 * Input_0[id.xy]);
	uint4 iInput_1 = uint4(255 * Input_1[id.xy]);

	//uint4 Color = (iInput_0 == iInput_1) ? iInput_0:0;
	uint4 Color = iInput_0 | iInput_1;
	Output[id.xy] = float4(Color / 255.0);
}

