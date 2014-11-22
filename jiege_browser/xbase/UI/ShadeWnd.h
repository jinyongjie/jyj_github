#pragma once
#include "layerdwindow.h"

class CShadeWnd :
	public CLayerdWindow
{
public:
	CShadeWnd(void);
	~CShadeWnd(void);

	BEGIN_MSG_MAP(CLayerdWindow)
		MESSAGE_HANDLER(WM_SIZE,OnSize)
		CHAIN_MSG_MAP(CXWindow)
	END_MSG_MAP()

	HRESULT UpdateUI();
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	HRESULT SetMask(CXDraw* pMask);
	HRESULT SetMaskMargin(CRect rc);
	BOOL CreateShadow(HDC hdc,CSize szWindow,CRect rcMargin);
private:
	CRect m_rcMaskMargin;
	CXDraw* m_pMask;
};
