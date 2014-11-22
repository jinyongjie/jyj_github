#include "StdAfx.h"
#include "ResourceMgr.h"

std::map<CString,Bitmap*> CResourceMgr::m_mapBitmap;
CString CResourceMgr::m_strSkinPath;
CResourceMgr::CResourceMgr(void)
{
}

CResourceMgr::~CResourceMgr(void)
{
}

Bitmap* CResourceMgr::GetBitmap( CString strPath )
{
	CString strAbsolutePath = m_strSkinPath + strPath;

	MAP_BITMAP::iterator it = m_mapBitmap.find(strAbsolutePath);
	if(it != m_mapBitmap.end())
	{
		return it->second;
	}
	Bitmap* pBmp = Bitmap::FromFile(strAbsolutePath);
	m_mapBitmap[strAbsolutePath] = pBmp;

	return pBmp;
}

HRESULT CResourceMgr::SetSkinPath( CString strPath )
{
	m_strSkinPath = strPath;
	CGpImage::InitGdiplus();
	return S_OK;
}