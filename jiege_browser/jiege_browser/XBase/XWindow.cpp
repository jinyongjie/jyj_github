#include "StdAfx.h"
#include "xwindow.h"
#include "Util.h"
#include "xbrush.h"
CXWindow::CXWindow(void)
{
	m_pRootFrame = NULL;
	m_pLButtonDown = NULL;
	m_pMouseIn = NULL;
	m_rcMargin = CRect(0,0,0,0);
	m_rcRect = CRect(0,0,0,0);

	m_bDrag = FALSE;
	m_bRelayout = FALSE;
	m_rcBorder = CRect(8,8,8,8);
	m_bCalcRgn = TRUE;
	m_bTracking = false;

	memset( &m_Tme, 0, sizeof( m_Tme ) );
	m_Tme.cbSize = sizeof( m_Tme );
	SystemParametersInfo( SPI_GETMOUSEHOVERTIME, 0, &m_Tme.dwHoverTime, 0 );
}

CXWindow::~CXWindow(void)
{
	if(IsWindow())
	{
		DestroyWindow();	
	}
	if(m_pRootFrame)
	{
		m_pRootFrame->Destroy();
		delete m_pRootFrame;
		m_pRootFrame = NULL;
	}
}

LRESULT CXWindow::OnPaint( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DoPaint();
	return 0;
}
LRESULT CXWindow::OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(m_pRootFrame && IsWindow())
	{
		CRect rc;
		GetClientRect(&rc);
		if(m_pRootFrame->GetRect() != rc)
		{
			m_pRootFrame->SetRect(rc);
			m_pRootFrame->Relayout();
			InvalidateRect(NULL);
		}
	}
	if(m_bCalcRgn && wParam != SIZE_MINIMIZED)
	{
		int cx = LOWORD(lParam);
		int cy = HIWORD(lParam);
		CRgn WndRgn = CreateRoundRectRgn(0, 0, cx + 1, cy + 1, 7, 7);
		POINT pt[4] = { {1, 1}, {cx - 2, 1}, {1, cy - 2}, {cx - 2, cy -2} };
		for (int i = 0; i < 4; i++)
		{
			CRgn rgn;
			rgn.CreateRectRgn(pt[i].x, pt[i].y, pt[i].x + 1, pt[i].y + 1);
			WndRgn.CombineRgn(WndRgn, rgn, RGN_DIFF);
		}

		SetWindowRgn(WndRgn, TRUE);	
	}
	
	return 0;
}
LRESULT CXWindow::OnEraseBkgrnd( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DoPaint();
	return 0;
}
LRESULT CXWindow::OnNotify( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}
LRESULT CXWindow::OnCaptureChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(m_pLButtonDown)
	{
		m_pLButtonDown->OnXMessage(WM_CAPTURECHANGED,wParam,lParam,bHandled);
	}

	m_bDrag = FALSE;
	m_pLButtonDown = NULL;
	return 0;
}

LRESULT CXWindow::OnCreate( UINT, WPARAM, LPARAM, BOOL& )
{
	if(m_pRootFrame == NULL)
	{
		CXFrame* pFrame = new CXFrame;
		SetRootFrame(pFrame);
	}


	return 0;
}

HRESULT CXWindow::SetRootFrame( CXFrame* pTopFrame )
{
	if(m_pRootFrame != pTopFrame)
	{
		if(m_pRootFrame)
		{
			delete m_pRootFrame;
		}
		m_pRootFrame = pTopFrame;

	}
	if(m_pRootFrame)
	{
		m_pRootFrame->SetWindow(this);
		Relayout();
	}


	return S_OK;
}

HRESULT CXWindow::Relayout()
{
	if(m_vecRelayout.size())
	{
		m_bRelayout = TRUE;
		vector<CXFrame*>::iterator it = m_vecRelayout.begin();
		for(;it != m_vecRelayout.end();it++)
		{
			CXFrame* pFrame = *it;
			if(pFrame && pFrame->GetNeedRelayout())
			{
				Internal_RelayoutFrame(pFrame);
			}
		}
		m_vecRelayout.clear();
		m_vecRelayout = m_vecRelayoutTmp;
		m_vecRelayoutTmp.clear();
		m_bRelayout = FALSE;
	}

	return S_OK;
}


HRESULT CXWindow::GetTopFrameFromPoint( POINT ptMousePos, CXFrame** ppTargetFrame )
{
	if ( NULL == ppTargetFrame || m_pRootFrame == NULL)
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}

	CXFrame*	pFrame = NULL;
	CRect rcFrame;
	BOOL bReachable = FALSE;

	pFrame =m_pRootFrame;
	rcFrame	= pFrame->GetScreenRect(  );
	bReachable = pFrame->GetVisible( );

	BOOL bInXFrame = FALSE;
	bInXFrame = rcFrame.PtInRect(ptMousePos);
	if ( bInXFrame == FALSE || !bReachable )
	{
		*ppTargetFrame = NULL;
		return E_FAIL;
	}

	CXFrame*	pFrameFootprint = NULL;
	UINT uChildCount = 0;
	int	nFrameIdx = 0;

	pFrameFootprint = pFrame;
	uChildCount = pFrame->GetFrameCount(  );
	nFrameIdx = ( ( int )uChildCount ) - 1;
	pFrame = NULL;
	while ( nFrameIdx >= 0 )
	{
		pFrame =	pFrameFootprint->GetFrameByIndex( ( INT )nFrameIdx );
		if (pFrame)
		{
			rcFrame = pFrame->GetScreenRect(  );
			bReachable	= pFrame->GetVisible(  );

			BOOL bPtInXFrame = FALSE;
			
			bPtInXFrame = rcFrame.PtInRect(ptMousePos);
			if ( bPtInXFrame && bReachable )
			{
				uChildCount =	pFrame->GetFrameCount(  );
				nFrameIdx = ( ( int )uChildCount ) - 1;
				pFrameFootprint = pFrame;
			}
			else
			{
				nFrameIdx--;
			}
		}
		else
		{
			nFrameIdx --;
		}
		pFrame = NULL;
	}


	

	*ppTargetFrame = pFrameFootprint;

	return S_OK;
}

HRESULT CXWindow::DoPaint()
{
	if(m_pRootFrame == NULL)
	{
		return E_FAIL;
	}
	Relayout();

	PAINTSTRUCT ps;
	HDC hdc = ::BeginPaint(m_hWnd,&ps);

	HDC		hdcMem;
	HBITMAP	hBmp, hBmpOld;
	CRect rcClip = ps.rcPaint;

	CRect rc;
	GetClientRect(&rc);

	if(m_pRootFrame->GetRect() != rc)
	{
		m_pRootFrame->SetRect(rc);
		m_pRootFrame->Relayout();
		rcClip = rc;
		InvalidateRect(NULL);
	}
	hdcMem = ::CreateCompatibleDC(hdc);
	hBmp = ::CreateCompatibleBitmap( hdc, rc.Width(),rc.Height() );


	if ( hdcMem && hBmp)
	{
		hBmpOld = ( HBITMAP )::SelectObject( hdcMem, hBmp );

		PaintBkg(hdcMem);
		//BitBlt( hdcMem, rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), hdc,0, 0, SRCCOPY );
		Util::BroadcastPaint(m_pRootFrame,hdcMem,m_pRootFrame->GetPaintRect(),rcClip);
		BitBlt( hdc, 0, 0, rc.Width(), rc.Height(), hdcMem,0, 0, SRCCOPY );

		::SelectObject( hdcMem, hBmpOld );
		::DeleteObject( hBmp );
		::DeleteDC( hdcMem );
	}


	::EndPaint(m_hWnd,&ps);


	return S_OK;
}

LRESULT CXWindow::OnRelayout( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	Relayout();
	return S_OK;
}

HRESULT CXWindow::MarkRelayout(CXFrame* pFrame)
{
	if(m_bRelayout)
	{
		m_vecRelayoutTmp.push_back(pFrame);
	}
	else
	{
		m_vecRelayout.push_back(pFrame);
	}

	PostMessage(WM_X_RELAYOUT,0,0);
	return S_OK;
}

LRESULT CXWindow::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( m_bTracking == false )
	{
		m_bTracking = true;

		m_Tme.hwndTrack = m_hWnd;
		m_Tme.dwFlags = TME_LEAVE;
		TrackMouseEvent( &m_Tme );
	}
	if(m_pRootFrame)
	{
		CPoint pt;
		GetCursorPos(&pt);

		if(m_pLButtonDown)
		{
			m_pLButtonDown->OnXMessage(WM_MOUSEMOVE,wParam,lParam,bHandled);
			if(m_bDrag)
			{
				m_pLButtonDown->OnXMessage(WM_X_DRAG,wParam,lParam,bHandled);
			}
			else
			{
				if(m_pLButtonDown->GetEnableDrag())
				{
					if(abs(pt.x - m_ptLButtonDown.x) > 10 || abs(pt.y - m_ptLButtonDown.y) > 10)
					{
						m_bDrag = TRUE;
						m_pLButtonDown->OnXMessage(WM_X_DRAGBEGIN,wParam,lParam,bHandled);
					}
				}
			}

		}
		else
		{
			CXFrame* pTargetFrame = NULL;
			GetTopFrameFromPoint(pt,&pTargetFrame);
			if(pTargetFrame)
			{
				if(pTargetFrame != m_pMouseIn)
				{
					if(m_pMouseIn)
					{
						m_pMouseIn->OnXMessage(WM_X_MOUSELEAVE,wParam,lParam,bHandled);
					}
					m_pMouseIn = pTargetFrame;
					m_pMouseIn->OnXMessage(WM_X_MOUSEENTER,wParam,lParam,bHandled);
				}
				else
				{
					pTargetFrame->OnXMessage(WM_MOUSEMOVE,wParam,lParam,bHandled);
				}
			}
		}


	}
	return 0;
}
LRESULT CXWindow::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	SetFocus();
	SetCapture();
	if(m_pRootFrame)
	{
		CPoint pt;
		GetCursorPos(&pt);
	
		CXFrame* pTargetFrame = NULL;
		GetTopFrameFromPoint(pt,&pTargetFrame);
		if(pTargetFrame)
		{
			m_pLButtonDown = pTargetFrame;
			m_ptLButtonDown = pt;
			pTargetFrame->OnXMessage(uMsg,wParam,lParam,bHandled);
		}
	}
	return 0;
}
LRESULT CXWindow::OnLButtonUP(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CXFrame* pLbuttonDown = m_pLButtonDown;
	BOOL bDrag = m_bDrag;
	ReleaseCapture();
	if(m_pRootFrame)
	{
		CPoint pt;
		GetCursorPos(&pt);

		CXFrame* pTargetFrame = NULL;
		GetTopFrameFromPoint(pt,&pTargetFrame);

		if(bDrag)
		{
			m_bDrag = FALSE;
			if(pLbuttonDown)
			{
				pLbuttonDown->OnXMessage(WM_X_DRAGEND,wParam,lParam,bHandled);
			}
		}
		else
		{
			if(pLbuttonDown)
			{
				pLbuttonDown->OnXMessage(WM_LBUTTONUP,wParam,lParam,bHandled);
				if(pTargetFrame == pLbuttonDown)
				{
					pLbuttonDown->OnXMessage(WM_X_LBUTTONCLICK,wParam,lParam,bHandled);
				}
			}
			else 
			{
				if(pTargetFrame)
				{
					pTargetFrame->OnXMessage(WM_LBUTTONUP,wParam,lParam,bHandled);
				}
			}
		}

		m_pLButtonDown = NULL;
	}
	
	return 0;
}

LRESULT CXWindow::OnMouseLeave( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	m_bTracking = false;
	if(m_pMouseIn)
	{
		return m_pMouseIn->OnXMessage(WM_X_MOUSELEAVE,wParam,lParam,bHandled);
	}
	return 0;
}

LRESULT CXWindow::OnSetCursor( UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lr = HTCLIENT;
	POINT ptMousePos = {0, 0};
	bHandled = FALSE;

	ptMousePos.x = GET_X_LPARAM(lParam);
	ptMousePos.y = GET_Y_LPARAM(lParam);

	if(!m_bCalcRgn)
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		ClientToScreen(rcClient);
		if(!rcClient.PtInRect(ptMousePos))
		{
			bHandled = FALSE;
			return 0;
		}
	}

	if(m_pRootFrame)
	{
		CPoint pt;
		GetCursorPos(&pt);

		if(m_pLButtonDown)
		{
			m_pLButtonDown->OnXMessage(WM_SETCURSOR,wParam,lParam,bHandled);
		}
		else
		{
			CXFrame* pTargetFrame = NULL;
			GetTopFrameFromPoint(pt,&pTargetFrame);
			if(pTargetFrame)
			{
				pTargetFrame->OnXMessage(WM_SETCURSOR,wParam,lParam,bHandled);
			}
		}
	}
	return 0;
}

LRESULT CXWindow::OnMouseWheel( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if(m_pRootFrame)
	{
		return m_pRootFrame->OnXMessage(uMsg,wParam,lParam,bHandled);
	}
	return 0;
}

void CXWindow::SetMargin(RECT rc)
{
	if(m_rcMargin != rc)
	{
		m_rcMargin = rc;
	}
}
RECT CXWindow::GetMargin()
{
	return m_rcMargin;
}

void CXWindow::SetRect(RECT rc)
{
	if(m_rcRect != rc)
	{
		m_rcRect = rc;
	}
}
RECT CXWindow::GetRect()
{
	return m_rcRect;
}

HRESULT CXWindow::NotifyFrameRemoved( CXFrame* pFrame )
{
	if(m_pMouseIn)
	{
		if(m_pMouseIn->IsAncestry(pFrame))
		{
			m_pMouseIn = NULL;
		}
	}
	if(m_pLButtonDown)
	{
		if(m_pLButtonDown->IsAncestry(pFrame))
		{
			m_pLButtonDown = NULL;
		}
	}

	return S_OK;
}

HRESULT	CXWindow::PaintBkg( HDC hdc )
{
	return 0;
}

CXFrame* CXWindow::GetRootFrame()
{
	return m_pRootFrame;
}

LRESULT CXWindow::OnDestroy( UINT, WPARAM, LPARAM, BOOL& )
{
	m_pLButtonDown = NULL;
	m_pMouseIn = NULL;
	if(m_pRootFrame)
	{
		m_pRootFrame->Destroy();
		delete m_pRootFrame;
		m_pRootFrame = NULL;
	}

	return 0;
}


HRESULT CXWindow::Internal_RelayoutFrame( CXFrame* pFrame )
{
	CXFrame* pParent = pFrame->GetParent();
	if(pParent && pParent->GetSensitiveChildSize())
	{
		Internal_RelayoutFrame(pParent);
	}
	else
	{
		pFrame->Relayout();
	}
	return S_OK;
}

LRESULT CXWindow::OnNCHittest( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	LRESULT lr = HTCLIENT;
	POINT ptMousePos = {0, 0};
	bHandled = FALSE;
	
	RECT rcWin;
	GetWindowRect(&rcWin);

	ptMousePos.x = GET_X_LPARAM(lParam);
	ptMousePos.y = GET_Y_LPARAM(lParam);

	if(!m_bCalcRgn)
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		ClientToScreen(rcClient);
		if(!rcClient.PtInRect(ptMousePos))
		{
			bHandled = FALSE;
			return 0;
		}
	}

	HWND hWnd;
	{
		RECT rcBorder = m_rcBorder;

		int	l, t, r, b;
		l = abs( ptMousePos.x - rcWin.left );
		t = abs( ptMousePos.y - rcWin.top );
		r = abs( ptMousePos.x - rcWin.right );
		b = abs( ptMousePos.y - rcWin.bottom );

		if ( r <= rcBorder.right && b <= rcBorder.bottom && rcBorder.right != 0 && rcBorder.bottom != 0 )
		{
			bHandled = TRUE;
			lr = HTBOTTOMRIGHT;
		}
		else if ( l <= rcBorder.left && b <= rcBorder.bottom && rcBorder.left != 0 && rcBorder.bottom != 0 )
		{
			bHandled = TRUE;
			lr = HTBOTTOMLEFT;
		}
		else if ( r <= rcBorder.right && t <= rcBorder.top && rcBorder.right != 0 && rcBorder.top != 0 )
		{
			bHandled = TRUE;
			lr = HTTOPRIGHT;
		}
		else if ( l <= rcBorder.left && t <= rcBorder.top && rcBorder.left != 0 && rcBorder.top != 0 )
		{
			bHandled = TRUE;
			lr = HTTOPLEFT;
		}
		else if ( b <= rcBorder.bottom && rcBorder.bottom != 0 )
		{
			bHandled = TRUE;
			lr = HTBOTTOM;
		}
		else if ( r <= rcBorder.right && rcBorder.right != 0 )
		{
			bHandled = TRUE;
			lr = HTRIGHT;
		}
		else if ( t <= rcBorder.top && rcBorder.top != 0 )
		{
			bHandled = TRUE;
			lr = HTTOP;
		}
		else if ( l <= rcBorder.left && rcBorder.left != 0 )
		{
			bHandled = TRUE;
			lr = HTLEFT;
		}
	}

	if ( bHandled == FALSE )
	{
		CXFrame* pTopFrame;
		GetTopFrameFromPoint(ptMousePos,&pTopFrame);
		if(pTopFrame)
		{
			lr = pTopFrame->GetNCHittest();
			bHandled = TRUE;	
		}
	}

	return lr;
}

HRESULT CXWindow::SetBorder( CRect rcBorder )
{
	m_rcBorder = rcBorder;
	return S_OK;
}

CRect CXWindow::GetBorder()
{
	return m_rcBorder;
}

HRESULT CXWindow::SetCalcRgn(BOOL bVal)
{
	m_bCalcRgn = bVal;
	if(IsWindow() && !bVal)
	{
		SetWindowRgn(NULL);
	}
	return S_OK;
}

LRESULT CXWindow::OnNCCalcSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = m_bCalcRgn;
	return 0;
}

LRESULT CXWindow::OnNCActivate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = m_bCalcRgn;
	return 1;
}

LRESULT CXWindow::OnKeyDown( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if(m_pRootFrame)
	{
		return m_pRootFrame->OnXMessage(uMsg,wParam,lParam,bHandled);
	}
	return 0;
}

LRESULT CXWindow::OnPosChanging( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if(m_pRootFrame && IsWindow())
	{
		CRect rc;
		GetClientRect(&rc);
		m_pRootFrame->SetRect(rc);
		m_pRootFrame->Relayout();
		InvalidateRect(NULL);
		UpdateWindow();
	}
	return 0;
}