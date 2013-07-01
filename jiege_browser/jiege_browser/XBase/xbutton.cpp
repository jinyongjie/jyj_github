#include "StdAfx.h"
#include "xbutton.h"

CXButton::CXButton(void)
{
	 m_pBkgNormal = NULL;
	 m_pBkgOver = NULL;
	 m_pBkgPush = NULL;

	 m_eButtonState = e_button_state_normal;
	 m_pText = NULL;
	 m_ptTextOffset = CPoint(0,0);
}

CXButton::~CXButton(void)
{
	if(m_pBkgNormal)
	{
		delete m_pBkgNormal;
	}
	if(m_pBkgOver)
	{
		delete m_pBkgOver;
	}
	if(m_pBkgPush)
	{
		delete m_pBkgPush;
	}
}

HRESULT CXButton::SetBkgNormal(CXDraw* pDraw)
{
	if(m_pBkgNormal == pDraw)
		return E_FAIL;

	m_pBkgNormal = pDraw;
	return S_OK;
}
CXDraw* CXButton::GetBkgNormal()
{
	return m_pBkgNormal;
}

HRESULT CXButton::SetBkgOn(CXDraw* pDraw)
{
	if(m_pBkgOver == pDraw)
		return E_FAIL;

	m_pBkgOver = pDraw;
	return S_OK;
}
CXDraw* CXButton::GetBkgOn()
{
	return m_pBkgOver;
}

HRESULT CXButton::SetBkgDown(CXDraw* pDraw)
{
	if(m_pBkgPush == pDraw)
		return E_FAIL;

	m_pBkgPush = pDraw;
	return S_OK;
}
CXDraw* CXButton::GetBkgDown()
{
	return m_pBkgPush;
}

HRESULT CXButton::Paint( HDC hdc ,CRect rcDrawRect)
{
	CXDraw* pBkg = NULL;
	switch (m_eButtonState)
	{
	case e_button_state_normal:
		pBkg = m_pBkgNormal;
		break;
	case e_button_state_hover:
		pBkg = m_pBkgOver;
		break;
	case e_button_state_down:
		pBkg = m_pBkgPush;
		break;
	}

	CRect rcPaint = rcDrawRect;
	if(pBkg)
	{
		pBkg->Draw(hdc,rcPaint);
	}
	if(m_pText)
	{
		m_pText->Draw(hdc,CRect(rcPaint.left + m_ptTextOffset.x,
			rcPaint.top + m_ptTextOffset.y,
			rcPaint.right,
			rcPaint.bottom));
	}

	return S_OK;
}

LRESULT CXButton::OnXMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	XBEGIN_MSG_MAP(uMsg,wParam,lParam,bHandled)
		XMESSAGE_HANDLER(WM_X_LBUTTONCLICK,OnLButtonClick)
		XMESSAGE_HANDLER(WM_X_MOUSEENTER,OnMouseEnter)
		XMESSAGE_HANDLER(WM_X_MOUSELEAVE,OnMouseLeave)
		XMESSAGE_HANDLER(WM_LBUTTONDOWN,OnLButtonDown)
		XMESSAGE_HANDLER(WM_LBUTTONUP,OnLButtonUp)
	XEND_MSG_MAP()

	return 0;
}

LRESULT CXButton::OnLButtonClick( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	ThrowEvent(EVENT_BUTTON_LBUTTON_CLICK,NULL);
	return 0;
}

LRESULT CXButton::OnMouseEnter(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SetState(e_button_state_hover);
	return S_OK;
}
LRESULT CXButton::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SetState(e_button_state_normal);
	ThrowEvent(EVENT_BUTTON_MOUSELEAVE,NULL);
	return S_OK;
}
LRESULT CXButton::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SetState(e_button_state_down);
	return S_OK;
}
LRESULT CXButton::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CRect rcScreen = GetScreenRect();
	CPoint pt;
	GetCursorPos(&pt);
	if(rcScreen.PtInRect(pt))
	{
		SetState(e_button_state_hover);
	}
	else
	{
		SetState(e_button_state_normal);
	}
	return S_OK;
}

HRESULT CXButton::SetState( EXButtonState eState )
{
	if(m_eButtonState != eState)
	{
		m_eButtonState = eState;
		InvalidateFrame();
	}
	return S_OK;
}

HRESULT CXButton::SetTextOffset( CPoint pt )
{
	m_ptTextOffset = pt;
	return S_OK;
}

HRESULT CXButton::SetText( CString strText )
{
	if(m_pText == NULL)
	{
		m_pText = new CXText;
		
	}
	m_pText->SetText(strText);
	return S_OK;
}

CString CXButton::GetText()
{
	if(m_pText)
	{
		return m_pText->GetText();
	}
	return L"";
}

HRESULT CXButton::Relayout()
{

	__super::Relayout();
	if(m_pText)
	{
		CSize sz = m_pText->ComputeSize();
		//if(m_strName == L"±à¼­")
		//{
		//	CString strName;
		//	strName.Format(L"%d,%d",sz.cx,sz.cy);
		//	MessageBox(NULL,strName,L"",0);
		//}

		m_ptTextOffset = CPoint((m_rcRect.Width() - sz.cx)/2,
			(m_rcRect.Height()-sz.cy)/2);
	}
	return S_OK;
}

HRESULT CXButton::SetTextColor( COLORREF color )
{
	if(m_pText == NULL)
	{
		m_pText = new CXText;
	}
	m_pText->SetColor(color);
	return S_OK;
}