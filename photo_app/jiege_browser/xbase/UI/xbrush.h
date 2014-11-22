#pragma once
#include "xdraw.h"
class CXBrush:public CXDraw
{
public:
	CXBrush(void);
	~CXBrush(void);

	HRESULT Draw(HDC hdc,CRect rcPaint);

	HRESULT SetColor(COLORREF clr);
private:
	COLORREF	m_clr;
	HBRUSH m_hBrush;
};
