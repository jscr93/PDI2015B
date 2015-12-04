#include "stdafx.h"
#include "GIFManager.h"
#include "gif.h"

CGIFManager::CGIFManager()
{
	InitializeCriticalSection(&m_csLock);
}

void CGIFManager::Push(CUFrame* pFrame)
{
	EnterCriticalSection(&m_csLock);
	if (m_lstFrames.size() > 200)
	{
		CUFrame* pFrameToDestroy = m_lstFrames.front();
		m_lstFrames.pop_front();
		delete pFrameToDestroy;
	}
	m_lstFrames.push_back(pFrame);
	LeaveCriticalSection(&m_csLock);
}

CUFrame* CGIFManager::Pull()
{
	CUFrame* pFrame = NULL;
	EnterCriticalSection(&m_csLock);
	if (m_lstFrames.size())
	{
		pFrame = m_lstFrames.front();
		m_lstFrames.pop_front();
	}
	LeaveCriticalSection(&m_csLock);
	return pFrame;
}

void CGIFManager::CreateGIF()
{
	CUFrame* frame = Pull();
	if (!frame)
		return;

	//UINT frameSize = frame->m_sx*frame->m_sy;
	//uint8_t* uint8_tFrame;
	
	GifWriter gifWriter;
	GifBegin(&gifWriter, "..\\Resources\\myGif.gif", frame->m_sx, frame->m_sy, 6);

	while (frame = Pull())
	{	
		//Convert char frames into uint8_t frames
		/*uint8_tFrame = new uint8_t[frameSize * 4];
		for (int j = 0; j < frame->m_sy; j++)
		{
			for (int i = 0; i < frame->m_sx; i++)
			{
				CFrame::PIXEL Color = frame->GetPixel(i, j);
				uint8_tFrame[(frame->m_sx*j * 4) + (i * 4)] = (uint8_t)Color.r;
				uint8_tFrame[(frame->m_sx*j * 4) + (i * 4) + 1] = (uint8_t)Color.g;
				uint8_tFrame[(frame->m_sx*j * 4) + (i * 4) + 2] = (uint8_t)Color.b;
				uint8_tFrame[(frame->m_sx*j * 4) + (i * 4) + 3] = (uint8_t)Color.a;
			}
		}*/
		
		//Add frame to GifWriter
		GifWriteFrame(&gifWriter, &frame->GetPixel(0,0).r, frame->m_sx, frame->m_sy, 6);

		//Delete used frames
		delete frame;
		//delete[] uint8_tFrame;
	}

	//EOF
	GifEnd(&gifWriter);
}


CGIFManager::~CGIFManager()
{
	DeleteCriticalSection(&m_csLock);
}
