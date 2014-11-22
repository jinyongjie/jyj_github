#include "StdAfx.h"
#include "LayerdWindow.h"
#include "Util.h"
CLayerdWindow::CLayerdWindow(void)
{
}

CLayerdWindow::~CLayerdWindow(void)
{
}

HRESULT CLayerdWindow::CreateLayerdWnd()
{
	CXWindow::Create(NULL,CRect(0,0,200,200),0,WS_POPUP,WS_EX_LAYERED);

	return S_OK;
}

HRESULT CLayerdWindow::UpdateUI()
{
	if(m_pRootFrame == NULL)
	{
		return E_FAIL;
	}
	
	HDC hdcScreen = ::GetDC(NULL);
	HDC		hdcMem;
	HBITMAP	hBmp, hBmpOld;

	CRect rcWindow;
	GetWindowRect(&rcWindow);

	hdcMem = ::CreateCompatibleDC(hdcScreen);
	hBmp = ::CreateCompatibleBitmap( hdcScreen, rcWindow.Width(),rcWindow.Height() );

	if ( hdcMem && hBmp)
	{
		hBmpOld = ( HBITMAP )::SelectObject( hdcMem, hBmp );

		PaintBkg(hdcMem);
		Util::BroadcastPaint(m_pRootFrame,hdcMem,m_pRootFrame->GetPaintRect(),CRect(0,0,rcWindow.Width(),rcWindow.Height()));

		BLENDFUNCTION blend;
		blend.BlendOp               = AC_SRC_OVER;
		blend.BlendFlags            = 0;
		blend.SourceConstantAlpha   = 255;
		blend.AlphaFormat           = AC_SRC_ALPHA;

		
		::UpdateLayeredWindow(m_hWnd, hdcScreen, 
			&rcWindow.TopLeft(),&rcWindow.Size(),
			hdcMem, &CPoint(0,0), RGB(0,0,0), &blend, ULW_ALPHA );


		::SelectObject( hdcMem, hBmpOld );
		::DeleteObject( hBmp );
		::DeleteDC( hdcMem );

		::ReleaseDC(NULL,hdcScreen);
	}


	return S_OK;
}

LRESULT CLayerdWindow::OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	CRect rcClient;
	GetClientRect(&rcClient);

	if(m_pRootFrame->GetRect() != rcClient)
	{
		m_pRootFrame->SetRect(rcClient);
		m_pRootFrame->Relayout();
		UpdateUI();
	}

	return 0;
}