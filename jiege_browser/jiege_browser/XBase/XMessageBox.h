#pragma once
#include "xdialog.h"
#include "xStatic.h"
class CXMessageBox:public CXDialog
{
public:
	CXMessageBox(void);
	~CXMessageBox(void);

	HRESULT SetText(CString strText);
	HRESULT SetOkText(CString strOk);
	CXButton* GetOkButton(); 
	HRESULT SetCancelText(CString strCancel);
	HRESULT SetIcon(CXDraw* pDraw);
	HRESULT SetStyle(DWORD dwStyle);
	HRESULT SetMultiline(BOOL bVal);

	HRESULT OnCreate( UINT, WPARAM, LPARAM, BOOL& );
	int DoModal(HWND hwParent, BOOL bCenterToParent /* = TRUE */);
	HRESULT OnXEvent(INT nId, CXFrame* pEventSource, void* pArg);
	HRESULT OnSysClose( void* pArg );
	HRESULT SetBtnImage(CXDraw* pNormal,CXDraw* pOn,CXDraw* pDown);
private:
	TXEventObserverObj<CXMessageBox> m_oEventObserver;
	CXButton* m_pBtnOk;
	CXButton* m_pBtnCancel;
	CXStatic* m_pStatic;
	CXFrame* m_pIconFrame;
	DWORD m_dwStyle;
	BOOL m_bMutiline;
};
