

cbuffer PARAMS
{
	int cursor_posX;
	int cursor_posY;
	int cursor_prev_posX;
	int cursor_prev_posY;
	int brush_size;
	float m;
	int click;
};

Texture2D<float4>	FusionOut:register(t0);//FusionOut
Texture2D<float4>	MetaCanvas:register(t1);//MetaCanvas
RWTexture2D<float4>	BackBuffer:register(u0);//BackBuffer
RWTexture2D<float4>	MetaCanvasOut:register(u1);//MetaCanvasOut
//SamplerState		Sampler:register(s0);  //Por default es bilineal.
//Todo lo que termine en state es una pieza de hardware. No es programable, es configurable


[numthreads(8, 8, 1)]
void Main(int3 id:SV_DispatchThreadID)
{
	//if ((abs(cursor_posX - id.x) + abs(cursor_posY - id.y)) < 20)
	int dx, dy;
	float4 brush_color = float4(.96, .97, .46, 0);
	float brush_alpha = 0.6;

	int x_intersectionIdy;
	int y_intersectionIdx;
	if (m != 0)
	{
		x_intersectionIdy = cursor_prev_posX - ((cursor_prev_posY - id.y) / m);
		y_intersectionIdx = cursor_prev_posY - (m*(cursor_prev_posX-id.x));

		dx = abs(x_intersectionIdy - id.x);
		dy = abs(y_intersectionIdx - id.y);
	}
	else
	{
		dx = abs(cursor_posX - id.x);
		dy = abs(cursor_posY - id.y);
	}
		



	/*if (cursor_posX < id.x)
		dx = id.x - cursor_posX;
	else
		dx = cursor_posX - id.x;

	if (cursor_posY < id.y)
		dy = id.y - cursor_posY;
	else
		dy = cursor_posY - id.y;*/

	/*dx = abs(cursor_posX - id.x);
	dy = abs(cursor_posY - id.y);*/

	int max_x, min_x;
	if (cursor_posX > cursor_prev_posX)
	{
		max_x = cursor_posX;
		min_x = cursor_prev_posX;
	}
	else
	{
		max_x = cursor_prev_posX;
		min_x = cursor_posX;
	}

	int max_y, min_y;
	if (cursor_posY > cursor_prev_posY)
	{
		max_y = cursor_posY;
		min_y = cursor_prev_posY;
	}
	else
	{
		max_y = cursor_prev_posY;
		min_y = cursor_posY;
	}
	

	//uint4 brush_color = uint4(245, 247, 117, 0);

	//if ((dx < brush_size && id.x >(min_x - brush_size / 3) && id.x < (max_x + brush_size / 3)) && (dy < brush_size && id.y >(min_y - brush_size / 3) && id.y < (max_y + brush_size / 3)))
	if (((dx < brush_size) || (dy < brush_size)) && id.x > (min_x - brush_size) && id.x < (max_x + brush_size) && id.y >(min_y - brush_size) && id.y < (max_y + brush_size))
	{
		BackBuffer[id.xy] = FusionOut[id.xy] * (brush_alpha)+(brush_color * (1 - brush_alpha));
		if (click == 1)
		{
			MetaCanvasOut[id.xy] = float4(0, 0, 0, 0);
		}
		if (click == 2)
		{
			MetaCanvasOut[id.xy] = float4(1, 1, 1, 0);
		}
		if (click == 0)
		{
			MetaCanvasOut[id.xy] = MetaCanvas[id.xy];
		}
	}
		//Output[id.xy] = brush_color;
	else
	{
		BackBuffer[id.xy] = FusionOut[id.xy];
		MetaCanvasOut[id.xy] = MetaCanvas[id.xy];
	}


}

