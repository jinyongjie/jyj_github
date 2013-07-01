#include "StdAfx.h"
#include "ChildWnd.h"

CXChildWnd::CXChildWnd(void)
{
	m_rcGaps = CRect(0,0,0,0);
}

CXChildWnd::~CXChildWnd(void)
{
}

HWND CXChildWnd::GetChildHwnd()
{
	return NULL;
}

HRESULT CXChildWnd::Relayout()
{
	__super::Relayout();
	MoveWindow(GetChildHwnd(),m_rcPaint.left + m_rcGaps.left,m_rcPaint.top + m_rcGaps.top,m_rcPaint.Width() - m_rcGaps.right - m_rcGaps.left,m_rcPaint.Height() - m_rcGaps.bottom - m_rcGaps.top,TRUE);
	return S_OK;
}

HRESULT CXChildWnd::SetVisible( BOOL bVisible )
{
	__super::SetVisible(bVisible);
	if(GetVisible())
	{
		ShowWindow(GetChildHwnd(),SW_SHOW);
	}
	else
	{
		ShowWindow(GetChildHwnd(),SW_HIDE);
	}
	return S_OK;
}

HRESULT CXChildWnd::SetParentVisible(BOOL bVisible)
{
	__super::SetParentVisible(bVisible);
	if(GetVisible())
	{
		ShowWindow(GetChildHwnd(),SW_SHOW);
	}
	else
	{
		ShowWindow(GetChildHwnd(),SW_HIDE);
	}
	return S_OK;
}

HRESULT CXChildWnd::Destroy()
{
	__super::Destroy();
	HWND hwnd = GetChildHwnd();
	if(IsWindow(hwnd))
	{
	DestroyWindow(GetChildHwnd());
	}

	return S_OK;
}

HRESULT CXChildWnd::Create( CXFrame* pParent )
{
	CXFrame::Create(pParent);
	SetVisible(TRUE);
	return S_OK;
}


HRESULT CXChildWnd::SetGaps( CRect rcGaps )
{
	m_rcGaps = rcGaps;
	return S_OK;
}