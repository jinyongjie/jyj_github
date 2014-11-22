#pragma once
#include "xwindow.h"

class CLayerdWindow :
	public CXWindow
{
public:
	CLayerdWindow(void);
	~CLayerdWindow(void);
	BEGIN_MSG_MAP(CLayerdWindow)
		MESSAGE_HANDLER(WM_SIZE,OnSize)
		CHAIN_MSG_MAP(CXWindow)
	END_MSG_MAP()

	HRESULT	CreateLayerdWnd();

	virtual	HRESULT UpdateUI();
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
private:
};
