#include "stdafx.h"
#include "UFrame.h"
CUFrame::CUFrame(int sx, int sy)
{
	m_pFrame = new PIXEL[sx*sy];
	m_sx = sx;
	m_sy = sy;
}
CUFrame::CUFrame(CUFrame& Frame) : CUFrame(Frame.m_sx, Frame.m_sy)
{
	memcpy(m_pFrame, Frame.m_pFrame, sizeof(PIXEL)*m_sx*m_sy);
}
CUFrame::~CUFrame()
{
	delete[] m_pFrame;
}
CUFrame::PIXEL& CUFrame::GetPixel(int i, int  j)
{
	static PIXEL Dummy;
	if (i >= 0 && i < m_sx && j >= 0 && j < m_sy)
		return m_pFrame[m_sx*j + i];
	return Dummy;
}
