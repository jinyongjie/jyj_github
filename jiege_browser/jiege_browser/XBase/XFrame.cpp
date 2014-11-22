#include "StdAfx.h"
#include "XFrame.h"
#include "XWindow.h"
CXFrame::CXFrame(void)
{
	m_pParentFrame = NULL;

	m_rcRect = CRect(0,0,0,0);	
	m_rcPaint = CRect(0,0,0,0);	

	m_pBackground = NULL;
	m_rcMargin = CRect(0,0,0,0);

	m_bVisible = TRUE;
	m_bParentVisible = TRUE;
	m_pWindow = NULL;

	m_bEnableDrag = FALSE;

	m_cAlpha = 255;
	m_bAutoHeight = FALSE;
	m_bAutoWidth = FALSE;

	m_nDockType = XDOCK_FILL;
	m_bNeedRelayout = TRUE;
	m_bSensitiveChildSize = FALSE;
	m_nNCHittest = HTCLIENT;
	m_pBackground2 = NULL;
}

CXFrame::~CXFrame(void)
{
	if(m_vecChild.size())
	{
		Destroy();
	}
	if(m_pBackground)
	{
		delete m_pBackground;
	}
}
HRESULT	CXFrame::AddFrame(CXFrame* pFrame)
{
	if(pFrame == NULL)
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}
	VEC_CHILD::iterator it = m_vecChild.begin();
	for(;it != m_vecChild.end();it++)
	{
		if(*it == pFrame)
		{
			ATLASSERT(FALSE);
			return E_FAIL;
		}
	}
	m_vecChild.push_back(pFrame);
	pFrame->SetParent(this);
	MarkRelayout();
	return S_OK;
}
HRESULT	CXFrame::DelFrame(CXFrame* pFrame)
{
	if(pFrame == NULL)
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}
	VEC_CHILD::iterator it = m_vecChild.begin();
	for(;it != m_vecChild.end();it++)
	{
		CXFrame* pTmpFrame = *it;
		if(pTmpFrame == pFrame)
		{
			m_vecChild.erase(it);
			pTmpFrame->SetParent(NULL);
			pTmpFrame->SetWindow(NULL);

			CXWindow* pWindow = GetWindow();
			if(pWindow)
			{
				pWindow->NotifyFrameRemoved(pFrame);
			}

			MarkRelayout();
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT	CXFrame::Relayout()
{
	Internal_syncPaintRect();
	VEC_CHILD::iterator it = m_vecChild.begin();
	for(;it != m_vecChild.end();it++)
	{
		if((*it)->GetVisible())
		{
			(*it)->Relayout();
		}
	}
	m_bNeedRelayout = FALSE;
	return S_OK;
}

HRESULT CXFrame::SetRect(CRect rc)
{
	if(m_rcRect != rc)
	{
		m_rcRect = rc;
	}
	return S_OK;
}
CRect	CXFrame::GetRect()
{
	return m_rcRect;
}

HRESULT CXFrame::SetParent(CXFrame* pParent)
{
	if(m_pParentFrame != pParent)
	{
		m_pParentFrame = pParent;
		if(m_pParentFrame)
		{
			m_pWindow = m_pParentFrame->GetWindow();
		}
		
	}
	return S_OK;
}
CXFrame* CXFrame::GetParent()
{
	return m_pParentFrame;
}

CRect	CXFrame::GetPaintRect()
{
	return m_rcPaint;
}

HRESULT CXFrame::Destroy()
{
	VEC_CHILD::iterator it = m_vecChild.begin();
	for(;it != m_vecChild.end();it++)
	{
		if(*it)
		{
			(*it)->Destroy();
			delete *it;
		}
	}
	m_vecChild.clear();

	return S_OK;
}

HRESULT	CXFrame::SetBackground(CXDraw* pDraw)
{
	if(m_pBackground == pDraw)
		return E_FAIL;
	if(m_pBackground)
	{
		delete m_pBackground;
	}
	m_pBackground = pDraw;
	InvalidateFrame();
	return S_OK;
}
CXDraw* CXFrame::GetBackground()
{
	return m_pBackground;
}
HRESULT	CXFrame::SetBackground2(CXDraw* pDraw)
{
	if(m_pBackground2 == pDraw)
		return E_FAIL;
	if(m_pBackground2)
	{
		delete m_pBackground2;
	}
	m_pBackground2 = pDraw;
	InvalidateFrame();
	return S_OK;
}
CXDraw* CXFrame::GetBackground2()
{
	return m_pBackground2;
}
LRESULT CXFrame::OnXMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	LRESULT lResult = 0;
	XMESSAGE_HANDLER(WM_SETCURSOR,OnSetCursor)
	XMESSAGE_HANDLER(WM_MOUSEWHEEL,OnMouseWheel)
	XMESSAGE_HANDLER(WM_KEYDOWN,OnKeyDown)

	bHandled = FALSE;

	return 0;
}
LRESULT CXFrame::OnSetCursor( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;
	return 0;
}
HRESULT	CXFrame::Paint(HDC hdc,CRect rcRect)
{
	if(m_pBackground)
	{
		m_pBackground->Draw(hdc,rcRect);
	}
	if(m_pBackground2)
	{
		m_pBackground2->Draw(hdc,rcRect);
	}

#ifdef _DEBUG
	//HPEN hpen =	CreatePen(PS_DASH,1,RGB(0,255,0));
	//HGDIOBJ hOldPen = SelectObject(hdc,hpen);

	//Rectangle(hdc,rcRect.left,rcRect.top,rcRect.right,rcRect.bottom);

	//SelectObject(hdc,hOldPen);
	//DeleteObject(hpen);
#endif
	return S_OK;
}
int		CXFrame::GetFrameCount()
{
	return (int)m_vecChild.size();
}
CXFrame*	CXFrame::GetFrameByIndex(int nIndex)
{
	if(nIndex < 0 || nIndex > ((int)m_vecChild.size() - 1))
	{
		ATLASSERT(FALSE);
		return NULL;
	}
	return m_vecChild[nIndex];
}

HRESULT CXFrame::SetMargin(CRect rc)
{
	m_rcMargin = rc;
	return S_OK;
}
CRect	CXFrame::GetMargin()
{
	return m_rcMargin;
}

HRESULT CXFrame::InvalidateFrame()
{
	HWND hWnd = GetHwnd();
	if(hWnd)
	{
		::InvalidateRect(hWnd,m_rcPaint,FALSE);
	}
	
	
	
	return S_OK;
}

HWND CXFrame::GetHwnd()
{
	if(m_pWindow)
	{
		return m_pWindow->m_hWnd;
	}
	m_pWindow = GetWindow();
	if(m_pWindow)
	{
		return m_pWindow->m_hWnd;
	}
	return NULL;
}


HRESULT CXFrame::ThrowEvent( INT nId, void* pArg )
{
	map<int, set<IEventObserver* > > mapListener( m_mapEventListener );
	map<int, set<IEventObserver* > >::const_iterator i = mapListener.find( nId );
	if ( i != mapListener.end() )
	{
		for ( set<IEventObserver* >::const_iterator j = i->second.begin(); j != i->second.end(); j++ )
		{
			( *j )->OnXEvent( nId, this, pArg );
		}
	}

	return S_OK;
}


HRESULT CXFrame::AddEventObserver( INT nId, IEventObserver* pEventListener )
{
	if( pEventListener == NULL)
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}

	set<IEventObserver*>& setListener( m_mapEventListener[nId] );
	if(setListener.find( pEventListener ) == setListener.end() )
	{
		setListener.insert( pEventListener );
	}

	return S_OK;
}

HRESULT CXFrame::RemoveEventObserver( INT nId, IEventObserver* pEventListener )
{
	if( pEventListener == NULL)
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}

	map<int, set<IEventObserver*> >::iterator i = m_mapEventListener.find( nId );
	if ( i != m_mapEventListener.end() )
	{
		i->second.erase( pEventListener );
	}

	return S_OK;
}


HRESULT CXFrame::ClearEventObserver( void )
{
	m_mapEventListener.clear();

	return S_OK;
}

CRect CXFrame::GetScreenRect()
{
	CPoint pt(m_rcPaint.TopLeft());
	::ClientToScreen(GetHwnd(),&pt);
	CRect rcScreen(pt.x,pt.y,pt.x + m_rcPaint.Width(),pt.y + m_rcPaint.Height());
	return rcScreen;
}

HRESULT CXFrame::SetVisible(BOOL bVisible)
{
	if(m_bVisible != bVisible)
	{
		m_bVisible = bVisible;

		VEC_CHILD::iterator it = m_vecChild.begin();
		for(;it != m_vecChild.end();it++)
		{
			if(*it)
			{
				(*it)->SetParentVisible(m_bVisible && m_bParentVisible);
			}
		}
		if(m_pParentFrame)
		{
			m_pParentFrame->MarkRelayout();
		}
	}
	return S_OK;
}
BOOL CXFrame::GetVisible()
{
	return (m_bVisible && m_bParentVisible);
}

HRESULT CXFrame::SetParentVisible(BOOL bVisible)
{
	if(m_bParentVisible != bVisible)
	{
		m_bParentVisible = bVisible;
		
		VEC_CHILD::iterator it = m_vecChild.begin();
		for(;it != m_vecChild.end();it++)
		{
			if(*it)
			{
				(*it)->SetParentVisible(m_bVisible && m_bParentVisible);
			}
		}

	}

	return S_OK;
}

HRESULT CXFrame::Internal_syncPaintRect()
{
	if(m_pParentFrame)
	{
		CRect rcParentPaint = m_pParentFrame->GetPaintRect();
		m_rcPaint = CRect(rcParentPaint.left + m_rcRect.left,
			rcParentPaint.top + m_rcRect.top,
			rcParentPaint.left + m_rcRect.right,
			rcParentPaint.top + m_rcRect.bottom);
	}
	else
	{
		m_rcPaint = m_rcRect;
	}
	return S_OK;
}

HRESULT CXFrame::MarkRelayout()
{
	if(!m_bNeedRelayout)
	{
		if(GetWindow())
		{
			m_pWindow->MarkRelayout(this);
			InvalidateFrame();		
		}
		m_bNeedRelayout = TRUE;
	}

	return S_OK;
}

HRESULT CXFrame::SetWindow(CXWindow* pWindow)
{
	m_pWindow = pWindow;
	return S_OK;
}
CXWindow* CXFrame::GetWindow()
{
	if(m_pWindow)
	{
		return m_pWindow;
	}
	if(m_pParentFrame)
	{
		m_pWindow = m_pParentFrame->GetWindow();
	}
	return m_pWindow;
}

HRESULT CXFrame::Create( CXFrame* pParent )
{
	if(pParent)
	{
		pParent->AddFrame(this);
	}
	return S_OK;
}

HRESULT CXFrame::SetName(LPCWSTR strName)
{
	m_strName = strName;
	return S_OK;
}
CString CXFrame::GetName()
{
	return m_strName;
}

HRESULT CXFrame::SetPosition(CPoint pt)
{
	if(m_rcRect.TopLeft() != pt)
	{
		CRect rcOld = m_rcRect;
		m_rcRect = CRect(pt.x,
			pt.y,
			pt.x + rcOld.Width(),
			pt.y + rcOld.Height());
	}
	return S_OK;
}
CPoint	CXFrame::GetPosition()
{
	return m_rcRect.TopLeft();
}

LRESULT CXFrame::OnMouseWheel( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;
	VEC_CHILD::iterator it = m_vecChild.begin();
	for(;it != m_vecChild.end();it++)
	{
		CXFrame* pChild = *it;
		if(pChild && pChild->GetVisible())
		{
			pChild->OnXMessage(uMsg,wParam,lParam,bHandled);
		}
	}
	return 0;
}
LRESULT CXFrame::OnKeyDown( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;
	VEC_CHILD::iterator it = m_vecChild.begin();
	for(;it != m_vecChild.end();it++)
	{
		CXFrame* pChild = *it;
		if(pChild && pChild->GetVisible())
		{
			pChild->OnXMessage(uMsg,wParam,lParam,bHandled);
		}
	}
	return 0;
}
HRESULT CXFrame::SetData(void* pData)
{
	m_pData = pData;
	return S_OK;
}
void* CXFrame::GetData()
{
	return m_pData;
}

HRESULT CXFrame::InsertFrame( CXFrame* pFrame,int nIndex )
{
	if(pFrame == NULL)
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}

	VEC_CHILD::iterator it = m_vecChild.begin();
	for(;it != m_vecChild.end();it++)
	{
		if(*it == pFrame)
		{
			ATLASSERT(FALSE);
			return E_FAIL;
		}
	}
	if(nIndex < 0)
	{
		nIndex = 0;
	}
	if(nIndex > m_vecChild.size())
	{
		nIndex = m_vecChild.size();
	}

	it = m_vecChild.begin();
	it+=nIndex;
	m_vecChild.insert(it,pFrame);
	pFrame->SetParent(this);
	MarkRelayout();
	return S_OK;
}

BOOL CXFrame::IsAncestry( CXFrame* pAncestry )
{
	CXFrame* pFrame = this;
	while(pFrame)
	{
		if(pFrame == pAncestry)
		{
			return TRUE;
		}
		pFrame = pFrame->GetParent();
	}
	return FALSE;
}

HRESULT CXFrame::SetEnableDrag(BOOL bEnable)
{
	m_bEnableDrag = bEnable;
	return S_OK;
}
BOOL	CXFrame::GetEnableDrag()
{
	return m_bEnableDrag;
}

HRESULT CXFrame::SetAlpha(BYTE cAlpha)
{
	m_cAlpha = cAlpha;
	return S_OK;
}
BYTE	CXFrame::GetAlpha()
{
	return m_cAlpha;
}

HRESULT CXFrame::SetAutoHeight(BOOL bVal)
{
	m_bAutoHeight = bVal;
	return S_OK;
}
BOOL	CXFrame::GetAutoHeight()
{
	return m_bAutoHeight;
}
HRESULT CXFrame::SetAutoWidth(BOOL bVal)
{
	m_bAutoWidth = bVal;
	return S_OK;
}
BOOL	CXFrame::GetAutoWidth()
{
	return m_bAutoWidth;
}
HRESULT CXFrame::SetDockType(int nDockType)
{
	m_nDockType = nDockType;
	return S_OK;
}
int		CXFrame::GetDockType()
{
	return m_nDockType;
}

BOOL CXFrame::GetNeedRelayout()
{
	return m_bNeedRelayout;
}

HRESULT CXFrame::SetSensitiveChildSize( BOOL bVal )
{
	m_bSensitiveChildSize = bVal;
	return S_OK;
}
BOOL	CXFrame::GetSensitiveChildSize()
{
	return m_bSensitiveChildSize;
}

HRESULT CXFrame::SetNCHittest( int nVal )
{
	m_nNCHittest = nVal;
	return S_OK;
}

int CXFrame::GetNCHittest()
{
	return m_nNCHittest;
}

HRESULT CXFrame::Clear()
{
	VEC_CHILD::iterator it = m_vecChild.begin();
	for(;it != m_vecChild.end();it++)
	{
		CXFrame* pTmpFrame = *it;
		pTmpFrame->SetParent(NULL);
		pTmpFrame->SetWindow(NULL);

		CXWindow* pWindow = GetWindow();
		if(pWindow)
		{
			pWindow->NotifyFrameRemoved(pTmpFrame);
		}

		MarkRelayout();
	}
	m_vecChild.clear();
	return S_OK;
}