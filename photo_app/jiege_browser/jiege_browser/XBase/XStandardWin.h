#pragma once
#include "xwindow.h"
#include "xbutton.h"
#include "xDockPanel.h"
#include "xStatic.h"

class CXStandardWin :
	public CXWindow
{
public:
	CXStandardWin(void);
	~CXStandardWin(void);

	LRESULT OnCreate( UINT, WPARAM, LPARAM, BOOL& );

	HRESULT SetBtnCloseImage(CXDraw* pNormal,CXDraw* pOn,CXDraw* pDown);
	HRESULT OnXEvent(INT nId, CXFrame* pEventSource, void* pArg) ;
	virtual HRESULT OnSysClose(void* pArg);
	HRESULT SetLogo(CXDraw* pLogo);
	HRESULT SetTitle(CString strTitle);
protected:
	CXButton* m_pSysBtnClose;
	CXDockPanel* m_pContaner;
	CXFrame* m_pLogo;
	CXStatic* m_pTitle;
private:
	TXEventObserverObj<CXStandardWin> m_oEventListener;
};
