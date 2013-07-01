#include "StdAfx.h"
#include "XStandardWin.h"
#include"util.h"
CXStandardWin::CXStandardWin(void):m_oEventListener(this)
{
	m_pSysBtnClose = NULL;
	m_pContaner = NULL;
	m_oEventListener.Hook_OnXEvent(&CXStandardWin::OnXEvent);
	m_pLogo = NULL;
	m_pTitle = NULL;
}

CXStandardWin::~CXStandardWin(void)
{

}


LRESULT CXStandardWin::OnCreate( UINT, WPARAM, LPARAM, BOOL& )
{
	CXDockPanel* pDockPanel = new CXDockPanel;
	SetRootFrame(pDockPanel);

	CXDockPanel* pTopPanel = new CXDockPanel;
	pTopPanel->Create(pDockPanel);
	pTopPanel->SetDockType(XDOCK_TOP);
	pTopPanel->SetRect(CRect(0,0,0,31));
	pTopPanel->SetNCHittest(HTCAPTION);

	m_pSysBtnClose = new CXButton;
	m_pSysBtnClose->Create(pTopPanel);
	m_pSysBtnClose->SetDockType(XDOCK_RIGHT);
	m_pSysBtnClose->SetRect(CRect(0,0,47,22));
	m_pSysBtnClose->SetMargin(CRect(0,0,4,0));
	m_pSysBtnClose->SetAutoHeight(TRUE);
	ADD_EVENT_OBSERVER(m_pSysBtnClose,EVENT_BUTTON_LBUTTON_CLICK,&m_oEventListener);

	m_pLogo = new CXFrame;
	m_pLogo->Create(pTopPanel);
	m_pLogo->SetRect(CRect(0,0,24,24));
	m_pLogo->SetDockType(XDOCK_LEFT);
	m_pLogo->SetAutoHeight(TRUE);
	m_pLogo->SetMargin(CRect(5,3,0,0));
	m_pLogo->SetVisible(FALSE);
	m_pLogo->SetNCHittest(HTCAPTION);

	m_pTitle = new CXStatic;
	m_pTitle->Create(pTopPanel);
	m_pTitle->SetRect(CRect(0,0,0,18));
	m_pTitle->SetDockType(XDOCK_FILL);
	m_pTitle->SetAutoHeight(TRUE);
	m_pTitle->SetMargin(CRect(8,8,0,0));
	
	m_pTitle->SetNCHittest(HTCAPTION);
	m_pTitle->SetText(L"");
	m_pTitle->SetFont(Util::GetBoldFont());
	m_pTitle->SetNormalColor(RGB(106,60,27));

	m_pContaner = new CXDockPanel;
	m_pContaner->Create(pDockPanel);
	m_pContaner->SetDockType(XDOCK_FILL);

	return 0;
}

HRESULT CXStandardWin::SetBtnCloseImage( CXDraw* pNormal,CXDraw* pOn,CXDraw* pDown )
{
	if(m_pSysBtnClose)
	{
		m_pSysBtnClose->SetBkgNormal(pNormal);
		m_pSysBtnClose->SetBkgOn(pOn);
		m_pSysBtnClose->SetBkgDown(pDown);
	}
	return S_OK;
}

HRESULT CXStandardWin::OnXEvent( INT nId, CXFrame* pEventSource, void* pArg )
{
	BEGIN_EVENT_MAP(pEventSource,nId,pArg)
		ON_EVENT_BY_ID(m_pSysBtnClose,EVENT_BUTTON_LBUTTON_CLICK,OnSysClose)
	END_EVENT_MAP()

	return S_OK;
}

HRESULT CXStandardWin::OnSysClose( void* pArg )
{
	DestroyWindow();
	return S_OK;
}

HRESULT CXStandardWin::SetLogo( CXDraw* pLogo )
{
	if(m_pLogo)
	{
		m_pLogo->SetBackground(pLogo);
	}
	m_pLogo->SetVisible(m_pLogo!=NULL);
	return S_OK;
}

HRESULT CXStandardWin::SetTitle( CString strTitle )
{
	if(m_pTitle)
	{
		m_pTitle->SetText(strTitle);
	}
	return S_OK;
}