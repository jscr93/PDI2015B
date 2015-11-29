Texture2D<float4>	Input_0:register(t0);		// SRV
Texture2D<float4>	Input_1:register(t1);		// SRV
RWTexture2D<float4>	Output:register(u0);	// UAV

[numthreads(8, 8, 1)]
void Main(uint3 id:SV_DispatchThreadID)
{
	//uint4 iInput_0 = uint4(255 * Input_0[id.xy]);
	//uint4 iInput_1 = uint4(255 * Input_1[id.xy]);
	//uint4 Color = uint4(iInput_0.x, iInput_0.y, iInput_0.z, iInput_0.w);
				
	//uint4 Color = iInput_0*(iInput_0[3])+(iInput_1 * (1-iInput_0[3]));
	//Output[id.xy] = float4(Color / 255.0);

				//       Arg0     *(    Arg0.a      ) + (  Arg1         * (1 -    Arg0.a       ))	
	Output[id.xy] = Input_0[id.xy]*(Input_0[id.xy].w) + (Input_1[id.xy] * (1 - Input_0[id.xy].w));
}

