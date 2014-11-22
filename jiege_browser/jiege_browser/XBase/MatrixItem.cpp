#include "StdAfx.h"
#include "matrixitem.h"
#include "Util.h"
CMatrixItem::CMatrixItem(void)
{
	m_bDrag = FALSE;
	m_hMemDC = NULL;
	m_hBmp = NULL;
}

CMatrixItem::~CMatrixItem(void)
{
	if(m_hMemDC)
	{
		DeleteDC(m_hMemDC);
	}	
}

HRESULT CMatrixItem::OnXMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	XBEGIN_MSG_MAP(uMsg,wParam,lParam,bHandled)
		XMESSAGE_HANDLER(WM_X_DRAGBEGIN,OnDragBegin)
		XMESSAGE_HANDLER(WM_X_DRAG,OnDrag)
		XMESSAGE_HANDLER(WM_X_DRAGEND,OnDragEnd)
		XEND_MSG_MAP()

	return S_OK;
}

LRESULT CMatrixItem::OnDragBegin(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_bDrag = TRUE;
	SetAlpha(200);
	ThrowEvent(EVENT_MATRIX_ITEM_DRAG_BEGIN,NULL);
	return 0;
}
LRESULT CMatrixItem::OnDrag(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ThrowEvent(EVENT_MATRIX_ITEM_DRAG,NULL);
	return 0;
}
LRESULT CMatrixItem::OnDragEnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SetAlpha(255);
	m_bDrag = FALSE;
	ThrowEvent(EVENT_MATRIX_ITEM_DRAG_END,NULL);

	return 0;
}

BOOL CMatrixItem::GetDrag()
{
	return m_bDrag;
}

CRect CMatrixItem::GetDragRect()
{
	return GetRect();
}

HRESULT CMatrixItem::SaveDC(BOOL bSave)
{
	if(m_hBmp)
	{
		DeleteObject(m_hBmp);
		m_hBmp = NULL;
	}
	if(m_hMemDC)
	{
		DeleteDC(m_hMemDC);
		m_hMemDC = NULL;
	}
	if(bSave)
	{
		HDC hdc = GetDC(NULL);
		m_hMemDC = ::CreateCompatibleDC(hdc);
		m_hBmp = ::CreateCompatibleBitmap(hdc,m_rcPaint.Width(),m_rcPaint.Height());
		HGDIOBJ hOldBmp = ::SelectObject(m_hMemDC,m_hBmp);
		DeleteObject(hOldBmp);

		CRect rcPaint(0,0,m_rcPaint.Width(),m_rcPaint.Height());
		Util::BroadcastPaint(this,m_hMemDC,rcPaint,rcPaint);

		ReleaseDC(NULL,hdc);
	}

	return S_OK;
}

HRESULT CMatrixItem::QuickPaint( HDC hdc )
{
	if(m_hMemDC)
	{
		BLENDFUNCTION bf = {AC_SRC_OVER, 0, m_cAlpha, AC_SRC_ALPHA};
		AlphaBlend(hdc,m_rcPaint.left,m_rcPaint.top,m_rcPaint.Width(),m_rcPaint.Height(),
			m_hMemDC,0,0,m_rcPaint.Width(),m_rcPaint.Height(),bf);
	}
	return S_OK;
}