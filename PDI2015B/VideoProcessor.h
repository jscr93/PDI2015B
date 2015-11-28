#pragma once
#include "..\Video\AtWareVideoCapture.h"
#include "Frame.h"
#include <list>
#include <Windows.h>
using namespace std;
class CVideoProcessor :
	public IAtWareSampleGrabberCB
{
protected:
	//Implementación de la interfase IAtWareSampleGrabberCB
	virtual HRESULT SampleCB(double SampleTime, IMediaSample *pSample);
	virtual HRESULT BufferCB(double SampleTime, BYTE *pBuffer, long BufferLen);
	list<CFrame*> m_lstFrames;
	CRITICAL_SECTION m_csLock;
public:
	AM_MEDIA_TYPE m_mt;    //Media type info (Video Format)
	void Push(CFrame* pFrame);
	CFrame* Pull(void);
	CVideoProcessor();
	~CVideoProcessor();
};

