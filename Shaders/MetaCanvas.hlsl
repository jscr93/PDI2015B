

cbuffer PARAMS
{
	int cursor_posX;
	int cursor_posY;
	int brush_size;
};

Texture2D<float4>	FusionOut:register(t0);//FusionOut
Texture2D<float4>	MetaCanvas:register(t1);//MetaCanvas
RWTexture2D<float4>	BackBuffer:register(u0);//BackBuffer
RWTexture2D<float4>	MetaCanvasOut:register(u1);//MetaCanvasOut
//SamplerState		Sampler:register(s0);  //Por default es bilineal.
//Todo lo que termine en state es una pieza de hardware. No es programable, es configurable


[numthreads(8, 8, 1)]
void Main(uint3 id:SV_DispatchThreadID)
{
	//if ((abs(cursor_posX - id.x) + abs(cursor_posY - id.y)) < 20)
	uint dx, dy;
	float4 brush_color = float4(.96, .97, .46, 0);
	float brush_alpha = 0.6;


	if (cursor_posX < id.x)
		dx = id.x - cursor_posX;
	else
		dx = cursor_posX - id.x;

	if (cursor_posY < id.y)
		dy = id.y - cursor_posY;
	else
		dy = cursor_posY - id.y;
	

	//uint4 brush_color = uint4(245, 247, 117, 0);
	if (dx < brush_size && dy < brush_size)
	{
		BackBuffer[id.xy] = FusionOut[id.xy] * (brush_alpha)+(brush_color * (1 - brush_alpha));
		MetaCanvasOut[id.xy] = float4(0, 0, 0, 0);
	}
		//Output[id.xy] = brush_color;
	else
	{
		BackBuffer[id.xy] = FusionOut[id.xy];
		MetaCanvasOut[id.xy] = MetaCanvas[id.xy];
	}


}

