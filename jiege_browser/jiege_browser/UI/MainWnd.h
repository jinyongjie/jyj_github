#pragma once
#include "..\xbase\xwindow.h"
#include "../core/IECore.h"
#include "../xbase/XEdit.h"
#include "../xbase/xbutton.h"

class CMainWnd :
	public CXWindow
{
public:
	CMainWnd(void);
	~CMainWnd(void);

	LRESULT OnCreate(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL&);
public:
	//对外接口
	HRESULT CreateMainWnd();

	HRESULT OnXEvent(INT nId, CXFrame* pEventSource, void* pArg) ;
	HRESULT OnBtnGo(void* pArg);
	HRESULT OnAddressChar(void* pArg);
private:
	CIECore* m_pIECore;
	CXEdit* m_pEditAddress;
	CXButton* m_pBtnGo;

	TXEventObserverObj<CMainWnd> m_oEventListener;
};
