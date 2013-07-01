#pragma once
#include "xGpImage.h"
typedef  std::map<CString,Bitmap*> MAP_BITMAP;
class CResourceMgr
{
public:
	CResourceMgr(void);
	~CResourceMgr(void);

	static	Bitmap* GetBitmap(CString strPath);
	static HRESULT SetSkinPath(CString strPath);
private:
	static	MAP_BITMAP m_mapBitmap;
	static  CString m_strSkinPath;
};
