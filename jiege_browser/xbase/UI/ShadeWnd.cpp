#include "StdAfx.h"
#include "ShadeWnd.h"
#include "Util.h"
CShadeWnd::CShadeWnd(void)
{
	m_rcMaskMargin = CRect(0,0,0,0);
	m_pMask = NULL;
}

CShadeWnd::~CShadeWnd(void)
{
}

BOOL CShadeWnd::CreateShadow(HDC hdc,CSize szWindow,CRect rcMargin)
{
	HDC hdcMask = ::CreateCompatibleDC(hdc);
	HBITMAP	hBmpMask = ::CreateCompatibleBitmap( hdc, szWindow.cx,szWindow.cy );
	HGDIOBJ hBmpOldMask = ( HBITMAP )::SelectObject( hdcMask, hBmpMask );

	if(hdcMask && hBmpMask)
	{
		CRect rcClip(rcMargin.left,rcMargin.top,szWindow.cx - rcMargin.right,szWindow.cy - rcMargin.bottom);

		int nSave = SaveDC(hdc);
		HRGN hRgn = ::CreateRectRgnIndirect( &rcClip );
		::ExtSelectClipRgn( hdc, hRgn ,RGN_DIFF);
		::DeleteObject(hRgn);
		HBRUSH hbrush = CreateSolidBrush(RGB(0,0,0));
		FillRect(hdc,CRect(0,0,szWindow.cx,szWindow.cy),hbrush);
		DeleteObject(hbrush);
		RestoreDC( hdc,nSave );

		//将阴影图片绘制到hdcMask
		m_pMask->Draw(hdcMask,CRect(0,0,szWindow.cx,szWindow.cy));

		BLENDFUNCTION bf = {AC_SRC_OVER,0,255,AC_SRC_ALPHA};
		::AlphaBlend(hdc,0,0,szWindow.cx,szWindow.cy,hdcMask,0,0,szWindow.cx,szWindow.cy,bf);

		::SelectObject( hdcMask, hBmpOldMask );
		::DeleteObject( hBmpMask );
		::DeleteDC( hdcMask );

		return TRUE;
	}
	return FALSE;
}
HRESULT CShadeWnd::UpdateUI()
{
	if(m_pMask == NULL)
	{
		return __super::UpdateUI();
	}
	if(m_pRootFrame == NULL)
	{
		return E_FAIL;
	}

	HDC hdcScreen = ::GetDC(NULL);

	CRect rcWindow;
	GetWindowRect(&rcWindow);

	CRect rcFrame(0,0,rcWindow.Width(),rcWindow.Height());
	HDC hdcMem = ::CreateCompatibleDC(hdcScreen);
	HBITMAP hBmp = ::CreateCompatibleBitmap( hdcScreen, rcWindow.Width(),rcWindow.Height() );
	HGDIOBJ hBmpOld = ( HBITMAP )::SelectObject( hdcMem, hBmp );

	
	if ( hdcMem && hBmp)
	{
		PaintBkg(hdcMem);
		Util::BroadcastPaint(m_pRootFrame,hdcMem,m_pRootFrame->GetPaintRect(),rcFrame);

		CreateShadow(hdcMem,CSize(rcWindow.Width(),rcWindow.Height()),m_rcMaskMargin);
		
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

LRESULT CShadeWnd::OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
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

HRESULT CShadeWnd::SetMask( CXDraw* pMask )
{
	m_pMask = pMask;
	return S_OK;
}

HRESULT CShadeWnd::SetMaskMargin( CRect rc )
{
	m_rcMaskMargin = rc;
	return S_OK;
}