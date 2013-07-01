#include "StdAfx.h"
#include "matrixbox.h"
#include "xbrush.h"
#include "util.h"

CMatrixBox::CMatrixBox(void):m_oEventListener(this)
{
	m_rcGaps = CRect(10,10,10,10);
	m_szItem = CSize(48,48);
	m_szItemGpas = CSize(10,10);

	m_bWidthAsParent = TRUE;

	m_oEventListener.Hook_OnXEvent(&CMatrixBox::OnXEvent);

	m_hMemDC = NULL;
}

CMatrixBox::~CMatrixBox(void)
{
	m_oEventListener.UnhookAll();
}

HRESULT CMatrixBox::Relayout()
{
	CRect rc = GetRect();
	CRect rcParent(0,0,0,0);
	if(m_bWidthAsParent)
	{
		CXFrame* pParent = GetParent();
		if(pParent)
		{
			rcParent = pParent->GetRect();
			rc.right = rc.left + rcParent.Width();
		}
	}
	CSize szRemain = rc.Size();
	int nCount = GetFrameCount();

	int nVolum = (szRemain.cx - m_rcGaps.left - m_rcGaps.right + m_szItemGpas.cx)/(m_szItem.cx + m_szItemGpas.cx);
	if(nVolum == 0)
	{
		return E_FAIL;
	}
	int nLine = nCount/nVolum;
	if(nCount % nVolum )
	{
		nLine ++;
	}
	int nBeginX = m_rcGaps.left;
	int nBeginY = m_rcGaps.top;
	for(int y = 0;y < nLine;y++)
	{
		for(int x = 0;x<nVolum;x++)
		{
			if(y*nVolum + x >= nCount)
			{
				break;
			}
			CRect rcItem;
			rcItem.left = nBeginX + x*(m_szItem.cx + m_szItemGpas.cx);
			rcItem.top = nBeginY + y*(m_szItem.cy + m_szItemGpas.cy);
			rcItem.right = rcItem.left + m_szItem.cx;
			rcItem.bottom = rcItem.top + m_szItem.cy;

			CMatrixItem* pChild = (CMatrixItem*)m_vecItem[y*nVolum + x];
			if(pChild && !pChild->GetDrag())
			{
				pChild->SetRect(rcItem);
			}
		}
	}
	if(GetAutoHeight())
	{
		int nFrameHeight = m_rcGaps.top + m_rcGaps.bottom + nLine*m_szItem.cy + (nLine - 1)*m_szItemGpas.cy;
		if(nFrameHeight < rcParent.Height())
		{
			nFrameHeight = rcParent.Height();
		}
		SetRect(CRect(rc.left,rc.top,rc.right,rc.top + nFrameHeight));
	}
	__super::Relayout();

	return S_OK;
}
HRESULT CMatrixBox::SetGaps(CRect rc)
{
	m_rcGaps = rc;
	return S_OK;
}
CRect CMatrixBox::GetGaps()
{
	return m_rcGaps;
}

HRESULT CMatrixBox::SetItemSize(CSize sz)
{
	m_szItem = sz;
	return S_OK;
}
CSize CMatrixBox::GetItemSize()
{
	return m_szItem;
}
HRESULT CMatrixBox::SetItemGaps(CSize sz)
{
	m_szItemGpas = sz;
	return S_OK;
}
CSize CMatrixBox::GetItemGaps()
{
	return m_szItemGpas;
}

HRESULT CMatrixBox::SetWidthAsParent(BOOL bVal)
{
	if(m_bWidthAsParent != bVal)
	{
		m_bWidthAsParent = bVal;
	}
	
	return S_OK;
}
BOOL CMatrixBox::GetWidthAsParent()
{
	return m_bWidthAsParent;
}

HRESULT CMatrixBox::OnXEvent(INT nId, CXFrame* pEventSource, void* pArg) 
{
	if(nId == EVENT_MATRIX_ITEM_DRAG_BEGIN)
	{
		OnItemDragBegin(pEventSource);
	}
	else if(nId == EVENT_MATRIX_ITEM_DRAG)
	{
		OnItemDrag((CMatrixItem*)pEventSource);
	}
	else if(nId == EVENT_MATRIX_ITEM_DRAG_END)
	{
		OnItemDragEnd(pEventSource);
	}
	return S_OK;
}
HRESULT CMatrixBox::OnItemDragBegin(CXFrame* pEventSource)
{
	vector<CXFrame*>::iterator it = m_vecChild.begin();
	m_nDragLastIndex = 0;
	for( ; it != m_vecChild.end() ;it++)
	{
		if(*it == pEventSource)
		{
			m_vecChild.erase(it);
			m_vecChild.push_back(pEventSource);//为了使拖拽对象显示在Z轴顶部
			break;
		}
		m_nDragLastIndex++;//记录拖拽前位置
	}
	SaveChildDC(TRUE);

	OnItemDrag((CMatrixItem*)pEventSource);

	return S_OK;
}
HRESULT CMatrixBox::QuickPaint(CRect rcClip0)
{
	//拖拽时直接向窗口绘制缓存的dc，提高效率
	CRect rcParent(0,0,0,0);
	if(m_pParentFrame)
	{
		rcParent = m_pParentFrame->GetPaintRect();
	}
	CRect rcClip;
	CRect rcTmp;
	if(IntersectRect(&rcTmp,&rcParent,&m_rcPaint) == 0)
	{
		return E_FAIL;
	}
	if(IntersectRect(&rcClip,&rcTmp,&rcClip0) == 0)
	{
		return E_FAIL;
	}

	CRect rcWin;
	::GetWindowRect(GetHwnd(),&rcWin);

	HDC hdcMem = ::CreateCompatibleDC(m_hMemDC);
	HBITMAP hbmp = ::CreateCompatibleBitmap(m_hMemDC,rcWin.Width(),rcWin.Height());
	HBITMAP hOldBmp = (HBITMAP)::SelectObject(hdcMem,hbmp);

	::BitBlt(hdcMem,rcClip.left,rcClip.top,rcClip.Width(),rcClip.Height(),m_hMemDC,rcClip.left,rcClip.top,SRCCOPY);

	int nCount = GetFrameCount();
	
	for(int i= 0;i<nCount;i++)
	{
		CMatrixItem* pItem = (CMatrixItem*)GetFrameByIndex(i);
		if(pItem)
		{
			CRect rcItem = pItem->GetPaintRect();
			if ( IntersectRect( &rcTmp,&rcItem,&rcClip ) != 0 )
			{
				pItem->QuickPaint(hdcMem);
			}	
		}
	}

	HDC hdcWin = ::GetDC(GetHwnd());
	::BitBlt(hdcWin,rcClip.left,rcClip.top,rcClip.Width(),rcClip.Height(),hdcMem,rcClip.left,rcClip.top,SRCCOPY);
	ReleaseDC(GetHwnd(),hdcWin);

	::SelectObject(hdcMem,hOldBmp);
	::DeleteObject(hbmp);
	::DeleteDC(hdcMem);

	return S_OK;
}
HRESULT CMatrixBox::OnItemDrag(CMatrixItem* pEventSource)
{
	if(m_pParentFrame == NULL)
	{
		return E_FAIL;
	}
	CPoint pt;
	GetCursorPos(&pt);
	CRect rcScreen = GetScreenRect();
	CRect rcScreenParent = m_pParentFrame->GetScreenRect();
	CRect rcClip;
	::IntersectRect(&rcClip,&rcScreen,&rcScreenParent);

	CRect rcDrag = pEventSource->GetDragRect();

	int nWidth = rcDrag.Width()/2 + 9 ;
	int nHeight = rcDrag.Height()/2 + 3;
	CRect rcDragItem = pEventSource->GetRect();


	int nOverFlow = 0;
	if(pt.x < rcClip.left + nWidth)
	{
		nOverFlow = 1;
		pt.x = rcClip.left + nWidth;
	}
	if(pt.y < rcClip.top + nHeight)
	{
		nOverFlow = 2;
		pt.y = rcClip.top + nHeight;
	}
	if(pt.x > rcClip.right - (rcDragItem.Width() - nWidth))
	{
		nOverFlow = 3;
		pt.x = rcClip.right - (rcDragItem.Width() - nWidth);
	}
	if(pt.y > rcClip.bottom - (rcDragItem.Height() - nHeight))
	{
		nOverFlow = 4;
		pt.y = rcClip.bottom - (rcDragItem.Height() - nHeight);
	}

	pt.x = pt.x - nWidth - rcScreen.left ;
	pt.y = pt.y - nHeight - rcScreen.top ;
	
	CRect rcOldPaint = pEventSource->GetPaintRect();
	pEventSource->SetPosition(pt);

	pt.x += nWidth;
	pt.y += nHeight;

	vector<CXFrame*> vec = m_vecItem;
	int nIndexMine = 0;
	for(vector<CXFrame*>::iterator it = vec.begin();it != vec.end();it++)
	{
		if(*it == pEventSource)
		{
			vec.erase(it);
			break;
		}
		nIndexMine ++;
	}

	BOOL bMoved = FALSE;

	int nIndex = 0;
	for(vector<CXFrame*>::iterator it = vec.begin();it != vec.end();it++)
	{
		CMatrixItem* pItem = (CMatrixItem*)*it;

		CRect rcItem = pItem->GetRect();
		if(pItem->GetEnableDrag() && rcItem.PtInRect(pt))
		{
			if(nIndex < nIndexMine)
			{
				vec.insert(it,pEventSource);
			}
			else if(nIndex == vec.size() - 1)
			{
				vec.push_back(pEventSource);
			}
			else
			{
				vec.insert(it+1,pEventSource);
			}
			
			bMoved = TRUE;
			break;
		}
		nIndex ++;
	}
	if(bMoved)
	{
		m_vecItem.clear();
		m_vecItem = vec;
		Relayout();
		QuickPaint(GetPaintRect());
	}
	else
	{
		pEventSource->Relayout();
		CRect rcDst;
		CRect rcCur = pEventSource->GetPaintRect();
		UnionRect(&rcDst,&rcOldPaint,&rcCur);
		QuickPaint(rcDst);
	}
	if(nOverFlow)
	{
		ThrowEvent(EVENT_MATRIX_DRAG_OVERFLOW,&nOverFlow);
	}

	return S_OK;
}
HRESULT CMatrixBox::OnItemDragEnd(CXFrame* pEventSource)
{
	MarkRelayout();
	_tagMatrixItemMoved info;
	info.pItem = pEventSource;
	info.nStard = m_nDragLastIndex;
	info.nEnd = GetIndexByItem((CMatrixItem*)pEventSource);
	
	ThrowEvent(EVENT_MATRIX_ITEM_MOVED,&info);
	SaveChildDC(FALSE);
	return S_OK;
}

HRESULT	CMatrixBox::AddFrame(CXFrame* pFrame)
{
	__super::AddFrame(pFrame);
	m_vecItem.push_back(pFrame);

	ADD_EVENT_OBSERVER(pFrame,EVENT_MATRIX_ITEM_DRAG_BEGIN,&m_oEventListener);
	ADD_EVENT_OBSERVER(pFrame,EVENT_MATRIX_ITEM_DRAG,&m_oEventListener);
	ADD_EVENT_OBSERVER(pFrame,EVENT_MATRIX_ITEM_DRAG_END,&m_oEventListener);
	return S_OK;
}
HRESULT	CMatrixBox::DelFrame(CXFrame* pFrame)
{
	__super::DelFrame(pFrame);
	
	vector<CXFrame*>::iterator it = m_vecItem.begin();
	for(;it != m_vecItem.end();it++)
	{
		if(*it == pFrame)
		{
			m_vecItem.erase(it);
			break;
		}
	}

	DEL_EVENT_OBSERVER(pFrame,EVENT_MATRIX_ITEM_DRAG_BEGIN,&m_oEventListener);
	DEL_EVENT_OBSERVER(pFrame,EVENT_MATRIX_ITEM_DRAG,&m_oEventListener);
	DEL_EVENT_OBSERVER(pFrame,EVENT_MATRIX_ITEM_DRAG_END,&m_oEventListener);

	return S_OK;
}
HRESULT CMatrixBox::InsertFrame(CXFrame* pFrame,int nIndex)
{
	__super::InsertFrame(pFrame,nIndex);
	vector<CXFrame*>::iterator it = m_vecItem.begin() + nIndex;
	m_vecItem.insert(it,pFrame);

	ADD_EVENT_OBSERVER(pFrame,EVENT_MATRIX_ITEM_DRAG_BEGIN,&m_oEventListener);
	ADD_EVENT_OBSERVER(pFrame,EVENT_MATRIX_ITEM_DRAG,&m_oEventListener);
	ADD_EVENT_OBSERVER(pFrame,EVENT_MATRIX_ITEM_DRAG_END,&m_oEventListener);
	return S_OK;
}

int CMatrixBox::GetIndexByItem( CMatrixItem* pItem )
{
	int nCount = m_vecItem.size();
	for(int i = 0;i<nCount;i++)
	{
		if(m_vecItem[i] == pItem)
		{
			return i;
		}
	}
	return 0;
}

HRESULT CMatrixBox::SaveBkg( HDC hdc ,CRect rc/*窗口相对父窗口坐标*/)
{
	if(m_hMemDC)
	{
		DeleteObject(m_hMemDC);
	}
	m_hMemDC = ::CreateCompatibleDC(hdc);
	HBITMAP hBmp = ::CreateCompatibleBitmap(hdc,rc.Width(),rc.Height());
	::SelectObject(m_hMemDC,hBmp);

	::BitBlt(m_hMemDC,0,0,rc.Width(),rc.Height(),hdc,rc.left,rc.top,SRCCOPY);

	return S_OK;
}

HRESULT CMatrixBox::SaveChildDC(BOOL bSave)
{
	int nCount = GetFrameCount();
	for(int i= 0;i<nCount;i++)
	{
		CMatrixItem* pItem = (CMatrixItem*)GetFrameByIndex(i);
		if(pItem)
		{
			pItem->SaveDC(bSave);
		}
	}
	return S_OK;
}

HRESULT CMatrixBox::OnXMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	XBEGIN_MSG_MAP(uMsg,wParam,lParam,bHandled)
		XMESSAGE_HANDLER(WM_X_LBUTTONCLICK,OnLButtonClick)
		XEND_MSG_MAP()

		return S_OK;
}

HRESULT CMatrixBox::OnLButtonClick( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	ThrowEvent(EVENT_MATRIX_LBUTTON_CLICK,NULL);
	return 0;
}

HRESULT CMatrixBox::Clear()
{
	__super::Clear();
	m_vecItem.clear();
	return S_OK;
}