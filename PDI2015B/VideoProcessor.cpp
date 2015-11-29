#include "stdafx.h"
#include "VideoProcessor.h"
#include <algorithm>


CVideoProcessor::CVideoProcessor()
{
	InitializeCriticalSection(&m_csLock);
}
CVideoProcessor::~CVideoProcessor()
{
	DeleteCriticalSection(&m_csLock);
}

void CVideoProcessor::Push(CFrame* pFrame)
{
	EnterCriticalSection(&m_csLock);
	if (m_lstFrames.size() > 60)
	{
		CFrame* pFrameToDestroy = m_lstFrames.front();
		m_lstFrames.pop_front();
		delete pFrameToDestroy;
	}
	m_lstFrames.push_back(pFrame);
	LeaveCriticalSection(&m_csLock);
}
CFrame*  CVideoProcessor::Pull()
{
	CFrame* pFrame = NULL;
	EnterCriticalSection(&m_csLock);
	if (m_lstFrames.size())
	{
		pFrame = m_lstFrames.front();
		m_lstFrames.pop_front();
	}
	LeaveCriticalSection(&m_csLock);
	return pFrame;
}

HRESULT CVideoProcessor::SampleCB(double SampleTime, IMediaSample *pSample)
{
	return S_OK;
}
HRESULT CVideoProcessor::BufferCB(double SampleTime, BYTE *pBuffer, long BufferLen)
{
	if (MEDIATYPE_Video == m_mt.majortype)
	{
		if (MEDIASUBTYPE_YUY2 == m_mt.subtype)
		{
			//[ Y0 U0 Y1 V0] [ Y2 U1 Y3 V1]  
			VIDEOINFOHEADER* pVIH=(VIDEOINFOHEADER*)m_mt.pbFormat;
			CFrame* pF = new CFrame(
				pVIH->bmiHeader.biWidth,
				pVIH->bmiHeader.biHeight);
			struct YUY2
			{
				unsigned char y0, u0, y1, v0;
			};
			//Remember: Macropixel Y0 U0 Y1 V0 = 2 image pixel
			for (int j = 0; j < pVIH->bmiHeader.biHeight; j++)
			{
				YUY2* pRow = (YUY2*)(pBuffer + j*pVIH->bmiHeader.biWidth * 2);
				for (int i = 0; i < pVIH->bmiHeader.biWidth / 2; i++)
				{
					CFrame::PIXEL pixel_1, pixel_2;
					//YUY2 to RGB
					YUY2* MacroPixel = &pRow[i];
					int C = (int)MacroPixel->y0 - 15;
					int D = (int)MacroPixel->u0 - 128;
					int E = (int)MacroPixel->v0 - 128;

					pixel_1.r = max(0,min(255,(298 * C + 409 * E + 128) / 256));
					pixel_1.g = max(0, min(255,(298 * C - 100 * D - 208 * E + 128) / 256));
					pixel_1.b = max(0, min(255, (298 * C + 516 * D + 128) / 256));

					C = (int)MacroPixel->y1 -16;
					pixel_2.r = max(0, min(255, (298 * C + 409 * E + 128) / 256));
					pixel_2.g = max(0, min(255, (298 * C - 100 * D - 208 * E + 128) / 256));
					pixel_2.b = max(0, min(255, (298 * C + 516 * D + 128) / 256));

					pF->GetPixel(i * 2, j) = pixel_1;
					pF->GetPixel((i * 2) + 1, j) = pixel_2;
					/*LummaGray.r = LummaGray.g = LummaGray.b = pRow->y0;
					pF->GetPixel(i * 2, j) = LummaGray;
					LummaGray.r = LummaGray.g = LummaGray.b = pRow->y1;
					pF->GetPixel((i * 2)+1, j) = LummaGray;*/
					//pRow++;
				}
			}
			Push(pF);
		}
	}
	return S_OK;
}