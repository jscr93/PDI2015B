#pragma once
class CUFrame
{
public:
	struct PIXEL
	{
		unsigned char r, g, b, a;
	};
protected:
	PIXEL* m_pFrame;
public:
	int m_sx, m_sy;
	CUFrame(int sx, int sy);
	CUFrame(CUFrame& Frame);
	PIXEL& GetPixel(int i, int j);
	CUFrame();
	~CUFrame();
};
