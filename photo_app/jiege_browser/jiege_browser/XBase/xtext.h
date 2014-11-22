#pragma once
#include "xdraw.h"
class CXText :public CXDraw
{
public:
	CXText(void);
	~CXText(void);

	
public:
	virtual	HRESULT Draw(HDC hdc,CRect rcPaint);
	
	HRESULT SetFont(HFONT hFont);
	HFONT GetFont();
	
	HRESULT SetText(LPCWSTR strText);
	CString GetText();
	
	HRESULT SetFormat(DWORD dwFormat);
	DWORD GetFormat();
	
	HRESULT SetColor(COLORREF clr);
	COLORREF GetColor();

	HRESULT SetSurportAlpha(BOOL bVal);
	CSize ComputeSize();

	HRESULT SetBold(BOOL bBold);
private:
	HFONT m_hFont;
	CString m_strText;
	DWORD m_dwFormat;
	COLORREF m_clr;
	BOOL	m_bSuportAlpha;
};
