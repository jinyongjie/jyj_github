#pragma once
#include "xframe.h"

class CXChildWnd :
	public CXFrame
{
public:
	CXChildWnd(void);
	~CXChildWnd(void);

	HRESULT Relayout();	
	HRESULT SetVisible(BOOL bVisible);
	HRESULT SetParentVisible(BOOL bVisible);
	virtual HWND GetChildHwnd();
	
	HRESULT 	Create(CXFrame* pParent);

	HRESULT Destroy();

	HRESULT SetGaps(CRect rcGaps);
private:
	CRect m_rcGaps;
};
