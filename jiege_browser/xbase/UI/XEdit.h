#pragma once
#include "childwnd.h"

#define EVENT_EDIT_CHAR 100

class CXEdit :
	public CXChildWnd,public CWindowImpl<CXEdit,CEdit>
{
public:
	CXEdit(void);
	~CXEdit(void);

	DECLARE_WND_SUPERCLASS(_T("CXEdit"), CEdit::GetWndClassName())

	BEGIN_MSG_MAP(CXEdit)
		MESSAGE_HANDLER(WM_CHAR,OnChar)
	END_MSG_MAP()

	LRESULT	OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	HRESULT Create(CXFrame* pParent);
	HWND GetChildHwnd();
	HRESULT SetRect(CRect rc);
	HRESULT SetText(CString strText);
	CString GetText();
private:

};
