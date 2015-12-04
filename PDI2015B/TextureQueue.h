#pragma once
#include <list>
#include<d3d11.h>
#include <Windows.h>
using namespace std;
class CTextureQueue
{
	list<ID3D11Texture2D*> m_lstTextures;
	CRITICAL_SECTION m_csLock;
public:
	CTextureQueue();
	void Push(ID3D11Texture2D* pTexture);
	ID3D11Texture2D* Pull(void);
	~CTextureQueue();
};

