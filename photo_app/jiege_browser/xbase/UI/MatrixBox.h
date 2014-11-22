#pragma once
#include "XFrame.h"
#include "MatrixItem.h"

#define EVENT_MATRIX_ITEM_MOVED	400
#define EVENT_MATRIX_LBUTTON_CLICK 410
#define EVENT_MATRIX_DRAG_OVERFLOW 420
struct _tagMatrixItemMoved
{
	CXFrame* pItem;
	int nStard;
	int nEnd;
};

class CMatrixBox : public CXFrame
{
public:
	CMatrixBox(void);
	~CMatrixBox(void);


	HRESULT Relayout();
	HRESULT SetGaps(CRect rc);
	CRect GetGaps();

	HRESULT SetItemSize(CSize sz);
	CSize GetItemSize();

	HRESULT SetItemGaps(CSize sz);
	CSize GetItemGaps();
	HRESULT SetWidthAsParent(BOOL bVal);
	BOOL GetWidthAsParent();

	HRESULT OnXEvent(INT nId, CXFrame* pEventSource, void* pArg) ;
	HRESULT OnItemDragBegin(CXFrame* pEventSource);
	HRESULT OnItemDrag(CMatrixItem* pEventSource);
	HRESULT OnItemDragEnd(CXFrame* pEventSource);

	HRESULT	AddFrame(CXFrame* pFrame);
	HRESULT	DelFrame(CXFrame* pFrame);
	HRESULT InsertFrame(CXFrame* pFrame,int nIndex);
	HRESULT	Clear();
	int		GetIndexByItem(CMatrixItem* pItem);

	HRESULT QuickPaint(CRect rcClip);
	HRESULT SaveBkg(HDC hdc,CRect rc);
	HRESULT SaveChildDC(BOOL bSave);
	HRESULT OnXMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	HRESULT OnLButtonClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
private:
	CRect m_rcGaps;//四周间隙
	CSize m_szItem;//item大小
	CSize m_szItemGpas;//item之间间隙
	BOOL  m_bWidthAsParent;

	TXEventObserverObj<CMatrixBox> m_oEventListener;
	vector<CXFrame*> m_vecItem;
	int m_nDragLastIndex;//拖拽前index
	HDC	m_hMemDC;
};
