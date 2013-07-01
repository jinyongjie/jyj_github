#pragma once
#include "XFrame.h"
#include "xcommon.h"
#include "xtext.h"
class CXStatic :public	CXFrame
{
public:
	CXStatic(void);
	~CXStatic(void);

	HRESULT SetText(CString strText);
	HRESULT Paint(HDC hdc,CRect rcRect);
	HRESULT SetFont(HFONT hFont);
	HRESULT SetNormalColor(COLORREF clr);
private:
	CXText* m_pText;
};
