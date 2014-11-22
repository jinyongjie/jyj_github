#include "StdAfx.h"
#include "XVScrollBar.h"

CXVScrollBar::CXVScrollBar(void)
{
	m_eSliderState = e_slider_normal;
	SetRect(CRect(0,0,14,0));
	m_nRange = 0;
	m_nPos = 0;

	m_pBkgSliderNormal = NULL;
	m_pBkgSliderOn = NULL;
	m_pBkgSliderDown = NULL;
	m_nStep = 30;
	m_bDrag = FALSE;
}

CXVScrollBar::~CXVScrollBar(void)
{
	if(m_pBkgSliderNormal)
	{
		delete m_pBkgSliderNormal;
	}
	if(m_pBkgSliderOn)
	{
		delete m_pBkgSliderOn;
	}
	if(m_pBkgSliderDown)
	{
		delete m_pBkgSliderDown;
	}
}

HRESULT CXVScrollBar::Relayout()
{
	CRect rcRemain(0,0,GetRect().Width(),GetRect().Height());
	if(m_nRange <= rcRemain.Height())
	{
		m_rcSlier = CRect(0,0,rcRemain.Width(),rcRemain.Height());
	}
	else
	{

		m_rcSlier.left = 0;
		m_rcSlier.right = rcRemain.Width();

		int nHeight = rcRemain.Height()*rcRemain.Height()/m_nRange;
		if(nHeight < 5)
		{
			nHeight = 5;
		}
		if(m_nPos == 0)
		{
			m_rcSlier.top = 0;
			m_rcSlier.bottom = nHeight;
		}
		else if(m_nPos == m_nRange - rcRemain.Height() )
		{
			m_rcSlier.bottom = rcRemain.Height();
			m_rcSlier.top = m_rcSlier.bottom - nHeight;
		}
		else
		{
			m_rcSlier.top = (rcRemain.Height())*m_nPos / m_nRange;
			if(m_rcSlier.top > rcRemain.Height() - nHeight)
			{
				m_rcSlier.top = rcRemain.Height() - nHeight;
			}
			m_rcSlier.bottom = m_rcSlier.top + nHeight;
		}
		
	}

	__super::Relayout();
	return S_OK;
}

HRESULT CXVScrollBar::Paint( HDC hdc ,CRect rcDrawRect)
{
	//绘制背景
	__super::Paint(hdc,rcDrawRect);

	//绘制滑动条
	CXDraw* pBkgSlider = NULL;
	switch(m_eSliderState)
	{
	case e_slider_normal:
		pBkgSlider = m_pBkgSliderNormal;
		break;
	case e_slider_on:
		pBkgSlider = m_pBkgSliderOn;
		break;
	case e_slider_down:
		pBkgSlider = m_pBkgSliderDown;
		break;

	}
	if(pBkgSlider)
	{
		CRect rcPaint = rcDrawRect;
		pBkgSlider->Draw(hdc,CRect(rcPaint.left + m_rcSlier.left,
			rcPaint.top + m_rcSlier.top,
			rcPaint.left + m_rcSlier.right,
			rcPaint.top + m_rcSlier.bottom));
	}

	return S_OK;
}

HRESULT CXVScrollBar::SetRange( INT nVal )
{
	if(m_nRange != nVal)
	{
		m_nRange = nVal;
	}
	return S_OK;
}

INT CXVScrollBar::GetPos()
{
	return m_nPos;
}

HRESULT CXVScrollBar::SetPos( INT nPos )
{
	if(m_nPos != nPos)
	{
		m_nPos = nPos;
	}
	return S_OK;
}

LRESULT CXVScrollBar::OnXMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	XBEGIN_MSG_MAP(uMsg,wParam,lParam,bHandled)
		XMESSAGE_HANDLER(WM_X_LBUTTONCLICK,OnLButtonClick)
		XMESSAGE_HANDLER(WM_X_MOUSEENTER,OnMouseEnter)
		XMESSAGE_HANDLER(WM_X_MOUSELEAVE,OnMouseLeave)
		XMESSAGE_HANDLER(WM_MOUSEMOVE,OnMouseMove)
		XMESSAGE_HANDLER(WM_MOUSEWHEEL,OnMouseWheel)
		XMESSAGE_HANDLER(WM_LBUTTONDOWN,OnLButtonDown)
		XMESSAGE_HANDLER(WM_LBUTTONUP,OnLButtonUp)
		XMESSAGE_HANDLER(WM_CAPTURECHANGED,OnCaptureChanged)
		XMESSAGE_HANDLER(WM_KEYDOWN,OnKeyDown)
		XEND_MSG_MAP()

		return 0;
}
LRESULT CXVScrollBar::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPoint pt;
	GetCursorPos(&pt);
	CRect rcScreen = GetScreenRect();
	CRect rcSlider(rcScreen.left + m_rcSlier.left,
		rcScreen.top + m_rcSlier.top,
		rcScreen.left + m_rcSlier.right,
		rcScreen.top + m_rcSlier.bottom);
	if(rcSlider.PtInRect(pt))
	{
		m_bDrag = TRUE;
		m_nMouseOffset = rcSlider.top - pt.y;
	}

	Internal_SetSliderState(e_slider_down);

	return 0;
}
LRESULT CXVScrollBar::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_bDrag = FALSE;

	CPoint pt;
	GetCursorPos(&pt);
	if(Internal_PtInRect(pt,m_rcSlier))
	{
		Internal_SetSliderState(e_slider_on);
	}
	else
	{
		Internal_SetSliderState(e_slider_normal);
	}

	return 0;
}
LRESULT CXVScrollBar::OnLButtonClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}
LRESULT CXVScrollBar::OnMouseEnter(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPoint pt;
	GetCursorPos(&pt);
	if(Internal_PtInRect(pt,m_rcSlier))
	{
		Internal_SetSliderState(e_slider_on);
	}
	return 0;
}
LRESULT CXVScrollBar::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Internal_SetSliderState(e_slider_normal);
	return 0;
}
LRESULT CXVScrollBar::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(m_bDrag)
	{
		CPoint pt;
		GetCursorPos(&pt);

		CPoint ptSlider(0,pt.y + m_nMouseOffset);
		CRect rcScreen = GetScreenRect();
		int nNewY = pt.y + m_nMouseOffset - rcScreen.top;
		if(nNewY < 0)
		{
			nNewY = 0;
		}
		if(nNewY > rcScreen.Height() - m_rcSlier.Height())
		{
			nNewY = rcScreen.Height() - m_rcSlier.Height();
		}
		if(nNewY == 0)
		{
			m_nPos = 0;
		}
		else if(nNewY == rcScreen.Height() - m_rcSlier.Height())
		{
			m_nPos = m_nRange - rcScreen.Height();
		}
		else
		{
			m_nPos = m_nRange*nNewY/rcScreen.Height();
		}
		ThrowEvent(EVENT_SCROLLBAR_ROLL,NULL);
		MarkRelayout();
	}
	else
	{
		CPoint pt;
		GetCursorPos(&pt);
		if(Internal_PtInRect(pt,m_rcSlier))
		{
			Internal_SetSliderState(e_slider_on);
		}
		else
		{
			Internal_SetSliderState(e_slider_normal);
		}
	}
	return 0;
}
LRESULT CXVScrollBar::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;

	if(m_pParentFrame)
	{
		CPoint pt;
		GetCursorPos(&pt);
		CRect rc = m_pParentFrame->GetScreenRect();
		if(!rc.PtInRect(pt))
		{
			return E_FAIL;
		}
	}

	int nDelta = (int)(short)(HIWORD(wParam));

	if(nDelta > 0)
	{
		Roll(-1);

	}
	else
	{
		Roll(1);

	}

	return 0;
}

HRESULT CXVScrollBar::SetSliderBkg( CXDraw* pNormal,CXDraw* pOn,CXDraw* pDown )
{
	if(m_pBkgSliderNormal != pNormal)
	{
		m_pBkgSliderNormal =pNormal;
	}
	if(m_pBkgSliderOn != pOn)
	{
		m_pBkgSliderOn =pOn;
	}
	if(m_pBkgSliderDown != pDown)
	{
		m_pBkgSliderDown =pDown;
	}
	return 0;
}

HRESULT CXVScrollBar::SetStep( int nStep )
{
	m_nStep = nStep;
	return S_OK;
}

LRESULT CXVScrollBar::OnCaptureChanged( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	m_bDrag = FALSE;
	return 0;
}

BOOL CXVScrollBar::Internal_PtInRect( POINT pt ,CRect rc)
{
	CRect rcPaint = GetScreenRect();
	CRect rcNew(rcPaint.left + rc.left,
		rcPaint.top + rc.top,
		rcPaint.left + rc.right,
		rcPaint.top + rc.bottom);

	return rcNew.PtInRect(pt);
}

HRESULT CXVScrollBar::Internal_SetSliderState( ESliderState eState )
{
	if(m_eSliderState != eState)
	{
		m_eSliderState = eState;
		InvalidateFrame();
	}
	return S_OK;
}

LRESULT CXVScrollBar::OnKeyDown( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if(wParam == VK_UP)
	{
		Roll(-1);
	}
	else if(wParam == VK_DOWN)
	{
		Roll(1);
	}
	else
	{
		bHandled = FALSE;
	}
	return 0;
}

HRESULT CXVScrollBar::Roll( int nStep )
{
	CRect rc = GetRect();
	if(rc.Height() >= m_nRange)
	{
		return S_OK;
	}

	m_nPos += m_nStep*nStep;
	if(m_nPos < 0)
	{
		m_nPos = 0;
	}
	if(m_nPos > (m_nRange - rc.Height()))
	{
		m_nPos = m_nRange - rc.Height();
	}


	ThrowEvent(EVENT_SCROLLBAR_ROLL,NULL);
	MarkRelayout();
	return S_OK;
}