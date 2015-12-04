#include "stdafx.h"
#include "TextureQueue.h"
#ifndef SAFE_RELEASE
	#define SAFE_RELEASE(X) if((X)){ (X)->Release();(X)=0; }
#endif

CTextureQueue::CTextureQueue()
{
	InitializeCriticalSection(&m_csLock);
}

CTextureQueue::~CTextureQueue()
{
	DeleteCriticalSection(&m_csLock);
}

void CTextureQueue::Push(ID3D11Texture2D* pTexture)
{
	EnterCriticalSection(&m_csLock);
	if (m_lstTextures.size() > 300)
	{
		ID3D11Texture2D* pTextureToDestroy = m_lstTextures.front();
		m_lstTextures.pop_front();
		SAFE_RELEASE(pTextureToDestroy);
	}
	m_lstTextures.push_back(pTexture);
	LeaveCriticalSection(&m_csLock);
}
ID3D11Texture2D*  CTextureQueue::Pull()
{
	ID3D11Texture2D* pTexture = NULL;
	EnterCriticalSection(&m_csLock);
	if (m_lstTextures.size())
	{
		pTexture = m_lstTextures.front();
		m_lstTextures.pop_front();
		Push(pTexture);
	}
	LeaveCriticalSection(&m_csLock);
	return pTexture;
}