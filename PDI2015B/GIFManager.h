#pragma once
#include "Frame.h"
#include "UFrame.h"
#include <list>
//#include "gif.h"

using namespace std;
class CGIFManager
{
protected:
	bool record;
	list<CUFrame*> m_lstFrames;
	CRITICAL_SECTION m_csLock;
public:
	CGIFManager();
	void Push(CUFrame* pFrame);
	CUFrame* Pull();
	void CreateGIF();
	~CGIFManager();
};

