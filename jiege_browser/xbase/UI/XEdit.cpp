#include "StdAfx.h"
#include "XEdit.h"
#include "Util.h"
CXEdit::CXEdit(void)
{
}

CXEdit::~CXEdit(void)
{
}

HRESULT CXEdit::Create( CXFrame* pParent )
{
	CXChildWnd::Create(pParent);

	CWindowImpl<CXEdit,CEdit>::Create(pParent->GetHwnd());

	HFONT hFont = Util::GetDefaultFont();

	SetFont(hFont);
	return S_OK;
}

HWND CXEdit::GetChildHwnd()
{
	return m_hWnd;
}

HRESULT CXEdit::SetRect( CRect rc )
{
	CXChildWnd::SetRect(rc);
	return S_OK;
}

HRESULT CXEdit::SetText( CString strText )
{
	SetWindowText(strText);
	return S_OK;
}

CString CXEdit::GetText()
{
	CString str;
	GetWindowText(str);
	return str;
}

LRESULT CXEdit::OnChar( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	ThrowEvent(EVENT_EDIT_CHAR,&wParam);
	bHandled = FALSE;
	return 0;
}