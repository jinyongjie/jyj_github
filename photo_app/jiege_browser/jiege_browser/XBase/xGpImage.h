#pragma once
#include "xdraw.h"
#include <gdiplus.h>
using namespace Gdiplus;
class CGpImage:public CXDraw
{
public:
	CGpImage(void);
	~CGpImage(void);
	virtual	HRESULT Draw(HDC hdc,CRect rcPaint);
	
	static HRESULT InitGdiplus();
public:
	
	HRESULT SetFile(LPCWSTR strFile);
	HRESULT SaveFile(CString strPath);

	HRESULT SetDrawType(int nType);
	HRESULT SetCutRect(CRect rcCut);//²Ã¼ôÇøÓò

	CSize GetSize();
	HRESULT SetTileRect(CRect rc);//¾Å¹¬¸ñ¼ÆËã×ø±ê

	HRESULT SetTransparent(BOOL bTrans);
	BOOL GetTransparent();
	HRESULT RotateFlip(int nVal);
public:


private:
	HRESULT Internal_DrawNormal(HDC hdc,CRect rcPaint,CRect rcImage);
	HRESULT Internal_DrawStretch(HDC hdc,CRect rcPaint,CRect rcImage);
	HRESULT Internal_Draw9Part(HDC hdc,CRect rcPaint,CRect rcImage);
	HRESULT Internal_Draw3PartH(HDC hdc,CRect rcPaint,CRect rcImage);
	HRESULT Internal_Draw3PartV(HDC hdc,CRect rcPaint,CRect rcImage);
	HRESULT Internal_DrawTile(HDC hdc,CRect rcPaint,CRect rcImage);
	HRESULT SetEnableGL(BOOL bEnable);
private:
	static ULONG_PTR m_gdiplusToken;
	CString m_strFile;
	Bitmap*		m_pImage;
	int m_nDrawType;
	CRect m_rcCut;
	CRect m_rcTile;
	BOOL m_bTransparent;
	BOOL m_bEnableGL;
};
