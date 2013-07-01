#include "StdAfx.h"
#include "XMessageBox.h"

//#define MB_OK                       0x00000000L
//#define MB_OKCANCEL                 0x00000001L

//#define MB_ICONQUESTION             0x00000020L
//#define MB_ICONEXCLAMATION          0x00000030L //叹号

#include <math.h>
CXMessageBox::CXMessageBox(void):m_oEventObserver(this)
{
	m_pBtnCancel = NULL;
	m_pBtnOk = NULL;
	m_pStatic = NULL;
	m_pIconFrame = NULL;
	m_dwStyle = MB_OK;
	m_oEventObserver.Hook_OnXEvent(&CXMessageBox::OnXEvent);
	m_bMutiline = TRUE;
}

CXMessageBox::~CXMessageBox(void)
{
}

HRESULT CXMessageBox::SetText( CString strText )
{
	if(m_pStatic == NULL)
	{
		return E_FAIL;
		
	}
	m_pStatic->SetText(strText);

	CXText *pText = new CXText;
	pText->SetText(strText);
	CSize sz = pText->ComputeSize();

	float line = 1; 
	
	line = sqrt((float)sz.cx / (sz.cy*6));//长宽比例大致为4:1
	
	CSize szStatic((sz.cx/line )*1.5,sz.cy * line*1.8);

	CRect rcMargin = m_pStatic->GetMargin();
	CRect rcWin(0,0,szStatic.cx + rcMargin.left + rcMargin.right ,szStatic.cy + rcMargin.top + rcMargin.bottom + 60);
	
	if(m_pIconFrame)
	{
		rcMargin = m_pIconFrame->GetMargin();
		rcWin.right + rcMargin.left + rcMargin.right + m_pIconFrame->GetRect().Width();
	}
	MoveWindow(rcWin);

	return S_OK;
}


int CXMessageBox::DoModal( HWND hwParent, BOOL bCenterToParent /* = TRUE */ )
{
	//按钮

	__super::DoModal(hwParent,bCenterToParent);
	return m_nRet;
}

HRESULT CXMessageBox::OnXEvent( INT nId, CXFrame* pEventSource, void* pArg )
{
	if(pEventSource == m_pBtnOk)
	{
		EndDialog(IDOK);
	}
	else if(pEventSource == m_pBtnCancel)
	{
		EndDialog(IDCANCEL);
	}
	return S_OK;
}

HRESULT CXMessageBox::OnSysClose( void* pArg )
{
	EndDialog(IDCANCEL);
	return S_OK;
}

HRESULT CXMessageBox::SetOkText( CString strOk )
{
	if(m_pBtnOk)
	{
		m_pBtnOk->SetText(strOk);
	}
	return S_OK;
}

HRESULT CXMessageBox::SetCancelText( CString strCancel )
{
	if(m_pBtnCancel)
	{
		m_pBtnCancel->SetText(strCancel);
	}

	return S_OK;
}

HRESULT CXMessageBox::SetIcon( CXDraw* pDraw )
{
	if(m_pIconFrame)
	{
		m_pIconFrame->SetBackground(pDraw);
	}
	return S_OK;
}

HRESULT CXMessageBox::OnCreate( UINT u, WPARAM w, LPARAM l, BOOL& b)
{
	__super::OnCreate(u,w,l,b);

	CXDockPanel* pBottomPanel = new CXDockPanel;
	pBottomPanel->Create(m_pContaner);
	pBottomPanel->SetDockType(XDOCK_BOTTOM);
	pBottomPanel->SetRect(CRect(0,0,0,30));

	if(m_dwStyle & MB_OKCANCEL)
	{
		m_pBtnCancel = new CXButton;
		m_pBtnCancel->Create(pBottomPanel);
		m_pBtnCancel->SetDockType(XDOCK_RIGHT);
		m_pBtnCancel->SetRect(CRect(0,0,60,23));
		m_pBtnCancel->SetAutoHeight(TRUE);
		m_pBtnCancel->SetMargin(CRect(0,0,18,4));

		m_pBtnCancel->SetText(L"取消");

		ADD_EVENT_OBSERVER(m_pBtnCancel,EVENT_BUTTON_LBUTTON_CLICK,&m_oEventObserver);
	}
	{
		m_pBtnOk = new CXButton;
		m_pBtnOk->Create(pBottomPanel);
		m_pBtnOk->SetDockType(XDOCK_RIGHT);
		m_pBtnOk->SetRect(CRect(0,0,60,23));
		m_pBtnOk->SetAutoHeight(TRUE);
		m_pBtnOk->SetMargin(CRect(0,0,18,4));

		m_pBtnOk->SetText(L"确定");

		ADD_EVENT_OBSERVER(m_pBtnOk,EVENT_BUTTON_LBUTTON_CLICK,&m_oEventObserver);
	}

	//图片
	BOOL bShowIcon = FALSE;
	if(m_dwStyle & MB_ICONEXCLAMATION || m_dwStyle & MB_ICONQUESTION)
	{
		bShowIcon = TRUE;
	}
	if(bShowIcon)
	{
		m_pIconFrame = new CXFrame;
		m_pIconFrame->Create(m_pContaner);
		m_pIconFrame->SetDockType(XDOCK_LEFT);
		m_pIconFrame->SetRect(CRect(0,0,41,40));
		m_pIconFrame->SetMargin(CRect(20,20,0,0));
		m_pIconFrame->SetAutoHeight(TRUE);
	}

	{
		//文字
		m_pStatic = new CXStatic;
		m_pStatic->Create(m_pContaner);
		m_pStatic->SetDockType(XDOCK_FILL);
		m_pStatic->SetMargin(CRect(20,20,20,20));
	}
	//确定大小
	{
		MoveWindow(CRect(0,0,338,163));
	}

	return S_OK;
}

HRESULT CXMessageBox::SetBtnImage(CXDraw* pNormal,CXDraw* pOn,CXDraw* pDown)
{
	if(m_pBtnCancel)
	{
		m_pBtnCancel->SetBkgNormal(pNormal);
		m_pBtnCancel->SetBkgOn(pOn);
		m_pBtnCancel->SetBkgDown(pDown);
	}
	if(m_pBtnOk)
	{
		m_pBtnOk->SetBkgNormal(pNormal);
		m_pBtnOk->SetBkgOn(pOn);
		m_pBtnOk->SetBkgDown(pDown);
	}
	return S_OK;
}


HRESULT CXMessageBox::SetStyle( DWORD dwStyle )
{
	m_dwStyle = dwStyle;
	return S_OK;
}

CXButton* CXMessageBox::GetOkButton()
{
	return m_pBtnOk;
}

HRESULT CXMessageBox::SetMultiline( BOOL bVal )
{
	m_bMutiline = bVal;
	return S_OK;
}