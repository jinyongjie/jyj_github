#include "StdAfx.h"
#include "MainWnd.h"
#include "../XBase/xDockPanel.h"
#include "../XBase/xbrush.h"
#include "../xbase/xGpImage.h"
#include "../XBase/ResourceMgr.h"
CMainWnd::CMainWnd(void):m_oEventListener(this)
{
	m_pIECore = NULL;
	m_pEditAddress = NULL;
	m_oEventListener.Hook_OnXEvent(&CMainWnd::OnXEvent);
}

CMainWnd::~CMainWnd(void)
{
	m_oEventListener.UnhookAll();
}

HRESULT CMainWnd::CreateMainWnd()
{
	Create(NULL,NULL,L"test_browser",WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_CLIPSIBLINGS);

	SetCalcRgn(FALSE);
	ShowWindow(SW_SHOW);
	MoveWindow(CRect(0,0,800,600));
	CenterWindow();
	return S_OK;
}

LRESULT CMainWnd::OnCreate( UINT, WPARAM, LPARAM, BOOL& )
{


	CXDockPanel* pRootFrame = new CXDockPanel;
	SetRootFrame(pRootFrame);

	{
		//CGpImage* pBkg = new CGpImage;
		//pBkg->SetFile(L"bkg.png");
		//pBkg->SetDrawType(XDRAWTYPE_TITLE);
		//pRootFrame->SetBackground(pBkg);

		CXBrush* pBrush = new CXBrush;
		pBrush->SetColor(RGB(255,255,255));
		pRootFrame->SetBackground(pBrush);
	}

	CXDockPanel* pTopPanel = new CXDockPanel;
	pTopPanel->Create(pRootFrame);
	pTopPanel->SetDockType(XDOCK_TOP);
	pTopPanel->SetRect(CRect(0,0,0,30));
	pTopPanel->SetMargin(CRect(0,10,0,0));


	{//ä¯ÀÀ°´Å¥
		m_pBtnGo = new CXButton;
		m_pBtnGo->Create(pTopPanel);
		m_pBtnGo->SetDockType(XDOCK_RIGHT);
		m_pBtnGo->SetRect(CRect(0,0,60,23));
		m_pBtnGo->SetAutoHeight(TRUE);
		m_pBtnGo->SetMargin(CRect(0,4,10,0));

		CGpImage* pNormal = new CGpImage;
		pNormal->SetFile(L"button_state4.png");
		pNormal->SetCutRect(CRect(0,0,60,23));
		CGpImage* pOn = new CGpImage;
		pOn->SetFile(L"button_state4.png");
		pOn->SetCutRect(CRect(60,0,120,23));
		CGpImage* pDown = new CGpImage;
		pDown->SetFile(L"button_state4.png");
		pDown->SetCutRect(CRect(120,0,180,23));
		m_pBtnGo->SetBkgNormal(pNormal);
		m_pBtnGo->SetBkgOn(pOn);
		m_pBtnGo->SetBkgDown(pDown);
		m_pBtnGo->SetText(L"Go");
		ADD_EVENT_OBSERVER(m_pBtnGo,EVENT_BUTTON_LBUTTON_CLICK,&m_oEventListener);
	}

	{//µØÖ·À¸
		m_pEditAddress = new CXEdit;
		m_pEditAddress->Create((CXFrame*)pTopPanel);
		m_pEditAddress->SetDockType(XDOCK_FILL);
		m_pEditAddress->SetRect(CRect(0,0,0,30));
		m_pEditAddress->SetMargin(CRect(10,0,10,0));
		m_pEditAddress->SetGaps(CRect(10,6,10,6));
		m_pEditAddress->SetAutoHeight(TRUE);

		CGpImage* pBkg = new CGpImage;
		pBkg->SetFile(L"edit.png");
		pBkg->SetDrawType(XDRAWTYPE_9PART);
		pBkg->SetTileRect(CRect(5,8,11,17));

		m_pEditAddress->SetBackground(pBkg);

		//{
		//	CXBrush* pBkg = new CXBrush;
		//	pBkg->SetColor(RGB(255,0,0));
		//	m_pEditAddress->SetBackground(pBkg);
		//}
		ADD_EVENT_OBSERVER(m_pEditAddress,EVENT_EDIT_CHAR,&m_oEventListener);

	}

	{//ä¯ÀÀÆ÷ÄÚºË
		m_pIECore = new CIECore;
		m_pIECore->Create(pRootFrame);
		m_pIECore->SetDockType(XDOCK_FILL);
		m_pIECore->SetVisible(TRUE);
		m_pIECore->SetMargin(CRect(0,10,0,0));
	}

	return 0;
}

LRESULT CMainWnd::OnDestroy( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	__super::OnDestroy(uMsg,wParam,lParam,bHandled);

	PostQuitMessage(0);
	return 0;
}

HRESULT CMainWnd::OnBtnGo( void* pArg )
{
	CString str;
	m_pEditAddress->GetWindowText(str);
	m_pIECore->Navigate(str);


	return S_OK;
}

HRESULT CMainWnd::OnXEvent( INT nId, CXFrame* pEventSource, void* pArg )
{
	BEGIN_EVENT_MAP(pEventSource,nId,pArg)
		ON_EVENT_BY_ID(m_pBtnGo,EVENT_BUTTON_LBUTTON_CLICK,OnBtnGo)
		ON_EVENT_BY_ID(m_pEditAddress,EVENT_EDIT_CHAR,OnAddressChar)
	END_EVENT_MAP()
	return S_OK;
}

HRESULT CMainWnd::OnAddressChar( void* pArg )
{
	DWORD dwChar = *((DWORD*)pArg);
	if(dwChar == VK_RETURN)
	{
		OnBtnGo(NULL);
	}
	return S_OK;
}