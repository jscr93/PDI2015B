Texture2D<float4>	Input_1:register(t0);		// SRV
Texture2D<float4>	Input_2:register(t0);		// SRV
RWTexture2D<float4>	Output:register(u0);	// UAV

[numthreads(8, 8, 1)]
void Main(uint3 id:SV_DispatchThreadID)
{
	[numthreads(8, 8, 1)]
void Main(uint3 id:SV_DispatchThreadID)
{
	Output[id.xy] = Input[id.xy];
}

