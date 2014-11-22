#pragma once
#include "BrowserCore.h"
#include <ExDispid.h>
#include <ShellAPI.h>
class CIECore :public CBrowserCore ,public CWindowImpl<CIECore,CAxWindow>,public IDispEventSimpleImpl<0, CIECore, &DIID_DWebBrowserEvents2>
{
public:
	CIECore(void);
	~CIECore(void);

	DECLARE_WND_SUPERCLASS(_T("CXIECore"), CAxWindow::GetWndClassName())
	
	BEGIN_MSG_MAP(CIECore)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
	END_MSG_MAP()


	BEGIN_SINK_MAP(CIECore)
		SINK_ENTRY_INFO(0, DIID_DWebBrowserEvents2, DISPID_NEWWINDOW3, OnEventNewWindow3, &NewWindow3_Info)
	END_SINK_MAP()


	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
public:
	HWND GetChildHwnd();
	HRESULT Create(CXFrame* pParent);

public:
	HRESULT Navigate(CString strUrl);
	void Stop();
	void Refresh();

	void SetAllowContextMenu(BOOL bAllow);
	void SetDocHostFlags(DWORD dwDocHostFlags);
	HRESULT GetHTMLDocument(IHTMLDocument2** ppHTMLDocument);
	HRESULT GetScriptDispatch(IDispatch** ppScript);
	HRESULT CreateScriptObject(IDispatchEx** ppScritObject);
	void SetFocusToHTML();
public:
	LRESULT OnForwardMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void __stdcall OnEventNewWindow3(IDispatch** /*ppDisp*/, VARIANT_BOOL* Cancel, DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl);
private:
	static _ATL_FUNC_INFO NewWindow3_Info;
};
