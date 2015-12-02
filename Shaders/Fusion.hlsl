

cbuffer PARAMS
{
	int cursor_posX;
	int cursor_posY;
	int brush_size;
};

Texture2D<float4>	VideoFrame:register(t0);//VideoFrame
Texture2D<float4>	VideoFrameModified:register(t1);//VideoFrameModified
Texture2D<float4>	MetaCanvas:register(t2);//MetaCanvas
RWTexture2D<float4>	Output:register(u0);//BackBuffer
//SamplerState		Sampler:register(s0);  //Por default es bilineal.
//Todo lo que termine en state es una pieza de hardware. No es programable, es configurable


[numthreads(8, 8, 1)]
void Main(uint3 id:SV_DispatchThreadID)
{
	//if ((abs(cursor_posX - id.x) + abs(cursor_posY - id.y)) < 20)
	
	if (MetaCanvas[id.xy].x == 0 && MetaCanvas[id.xy].y == 0 && MetaCanvas[id.xy].z == 0 && MetaCanvas[id.xy].w == 0 )
	{
		Output[id.xy] = VideoFrameModified[id.xy];
	}
	else
	{
		Output[id.xy] = VideoFrame[id.xy];
	}

}

