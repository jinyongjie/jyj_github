#include "StdAfx.h"
#include "IECore.h"

_ATL_FUNC_INFO CIECore::NewWindow3_Info = { CC_STDCALL, VT_EMPTY, 5, { VT_BYREF | VT_DISPATCH, VT_BYREF | VT_BOOL, VT_UINT, VT_BSTR, VT_BSTR } };

CIECore::CIECore(void)
{
}

CIECore::~CIECore(void)
{
}

HWND CIECore::GetChildHwnd()
{
	return m_hWnd;
}

HRESULT CIECore::Create( CXFrame* pParent )
{
	CBrowserCore::Create(pParent);

	if(pParent)
	{
		HWND hWnd = CWindowImpl<CIECore, CAxWindow>::Create(pParent->GetHwnd(), 0, L"", WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0 , 0U, NULL);
		if(hWnd == NULL)
		{
			ATLASSERT(FALSE);
		}
		//CreateControl(L"MSHTML:<HTML><BODY>This is a line of text</BODY></HTML>");


		BSTR bsGUID;
		StringFromIID(CLSID_WebBrowser,&bsGUID);
		CreateControl(bsGUID);
		CoTaskMemFree(bsGUID);

		//CreateControl(L"Shell.Explorer");

		CComPtr<IWebBrowser2> spWebBrowser2;
		HRESULT hRet = QueryControl(IID_IWebBrowser2, (void**)&spWebBrowser2);
		if(SUCCEEDED(hRet))
		{
			HRESULT hr = IDispEventSimpleImpl<0, CIECore, &DIID_DWebBrowserEvents2>::DispEventAdvise(spWebBrowser2, &DIID_DWebBrowserEvents2);

			if(FAILED(hr))
				ATLASSERT(FALSE);
		}
	
	}
	return S_OK;
}

HRESULT CIECore::Navigate( CString strUrl )
{
	CComPtr<IWebBrowser2> spWebBrowser2;
	HRESULT hRet = QueryControl(IID_IWebBrowser2, (void**)&spWebBrowser2);
	if (FAILED(hRet))
		return E_FAIL;

	CComBSTR bsUrl = strUrl;
	spWebBrowser2->Navigate(bsUrl, NULL, NULL, NULL, NULL);
	return S_OK;
}

LRESULT CIECore::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lRet = DefWindowProc(uMsg, wParam, lParam);

	// Connect events

	return lRet;
}


LRESULT CIECore::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// Disconnect events
	CComPtr<IWebBrowser2> spWebBrowser2;
	HRESULT hRet = QueryControl(IID_IWebBrowser2, (void**)&spWebBrowser2);
	if(SUCCEEDED(hRet))
		IDispEventSimpleImpl<0, CIECore, &DIID_DWebBrowserEvents2>::DispEventUnadvise(spWebBrowser2, &DIID_DWebBrowserEvents2);

	bHandled	= FALSE;
	return 1;
}
LRESULT CIECore::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	LRESULT lRet = DefWindowProc(uMsg, wParam, lParam);
	SetFocusToHTML();

	return lRet;
}

void CIECore::Stop()
{
	CComPtr<IWebBrowser2> spWebBrowser2;
	HRESULT hRet = QueryControl(IID_IWebBrowser2, (void**)&spWebBrowser2);
	if (FAILED(hRet))
		return;

	spWebBrowser2->Stop();
}
void CIECore::Refresh()
{
	CComPtr<IWebBrowser2> spWebBrowser2;
	HRESULT hRet = QueryControl(IID_IWebBrowser2, (void**)&spWebBrowser2);
	if (FAILED(hRet))
		return;

	spWebBrowser2->Refresh();
}

void CIECore::SetAllowContextMenu(BOOL bAllow)
{
	CComPtr<IAxWinAmbientDispatch> spHost;
	HRESULT hRet = QueryHost(IID_IAxWinAmbientDispatch, (void**)&spHost);
	if(SUCCEEDED(hRet))
	{
		hRet = spHost->put_AllowContextMenu(bAllow ? VARIANT_TRUE : VARIANT_FALSE);
		ATLASSERT(SUCCEEDED(hRet));
	}
}
void CIECore::SetDocHostFlags(DWORD dwDocHostFlags)
{
	CComPtr<IAxWinAmbientDispatch> spHost;
	HRESULT hRet = QueryHost(IID_IAxWinAmbientDispatch, (void**)&spHost);
	if(SUCCEEDED(hRet))
	{
		// Set host flag to indicate that we handle themes
		hRet = spHost->put_DocHostFlags(dwDocHostFlags);
		ATLASSERT(SUCCEEDED(hRet));
	}
}
HRESULT CIECore::GetHTMLDocument(IHTMLDocument2** ppHTMLDocument)
{
	HRESULT hRet = QueryControl(IID_IHTMLDocument2, (void**)ppHTMLDocument);
	if (SUCCEEDED(hRet))
		return hRet;

	CComPtr<IWebBrowser2> spWebBrowser;
	hRet = QueryControl(IID_IWebBrowser2, (void**)&spWebBrowser);
	if(SUCCEEDED(hRet) && spWebBrowser != NULL)
	{
		CComPtr<IDispatch> spDocument;
		hRet = spWebBrowser->get_Document(&spDocument);
		if(SUCCEEDED(hRet) && spDocument != NULL)
		{
			CComQIPtr<IHTMLDocument2> spHtmlDoc = spDocument;
			if(spHtmlDoc != NULL)
			{
				*ppHTMLDocument	= spHtmlDoc;
				spHtmlDoc.Detach();
				return S_OK;
			}
		}
	}
	return E_FAIL;
}
HRESULT CIECore::GetScriptDispatch(IDispatch** ppScript)
{
	if (ppScript == NULL)
		return E_INVALIDARG;

	*ppScript	= NULL;

	do 
	{
		CComPtr<IHTMLDocument2> spDoc2;

		if (FAILED(GetHTMLDocument(&spDoc2)))
			break;

		if (FAILED(spDoc2->get_Script(ppScript)))
			break;

		return S_OK;

	} while (false);
	return E_FAIL;
}
HRESULT CIECore::CreateScriptObject(IDispatchEx** ppScritObject)
{
	if (ppScritObject == NULL)
		return E_INVALIDARG;

	*ppScritObject	= NULL;

	do 
	{
		CComPtr<IHTMLDocument2> spDoc2;

		if (FAILED(GetHTMLDocument(&spDoc2)))
			break;

		CComPtr<IDispatch> spScript;
		if (FAILED(spDoc2->get_Script(&spScript)))
			break;

		CComVariant vRet;
		if (FAILED(spScript.Invoke1(L"eval", &CComVariant(L"new Object;"), &vRet)))
			break;

		if (vRet.vt != VT_DISPATCH || vRet.pdispVal == NULL)
			break;

		CComQIPtr<IDispatchEx> spRet(vRet.pdispVal);

		if (!spRet)
			break;

		*ppScritObject	= spRet.Detach();

		return S_OK;

	} while (false);

	return E_FAIL;
}
void CIECore::SetFocusToHTML()
{
	CComPtr<IHTMLDocument2> spHtmlDoc;
	HRESULT hRet = GetHTMLDocument(&spHtmlDoc);
	if(SUCCEEDED(hRet) && spHtmlDoc != NULL)
	{
		CComPtr<IHTMLWindow2> spParentWindow;
		hRet = spHtmlDoc->get_parentWindow(&spParentWindow);
		if(spParentWindow != NULL)
			spParentWindow->focus();
	}
}

LRESULT CIECore::OnForwardMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPMSG pMsg = (LPMSG)lParam;

	if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
		(pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
		return FALSE;

	BOOL bRet = FALSE;
	// give HTML page a chance to translate this message
	if(pMsg->hwnd == m_hWnd || IsChild(pMsg->hwnd))
		bRet = (BOOL)SendMessage(WM_FORWARDMSG, 0, (LPARAM)pMsg);

	return bRet;
}
void CIECore::OnEventNewWindow3(IDispatch** /*ppDisp*/, VARIANT_BOOL* Cancel, DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl)
{
	*Cancel	= VARIANT_TRUE;
	::ShellExecute(NULL, L"open", bstrUrl, 0, 0, SW_SHOWNORMAL);
}