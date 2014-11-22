#pragma once
#include "XFrame.h"

//Item的基类,主要提供拖拽功能

#define EVENT_MATRIX_ITEM_DRAG_BEGIN	1000
#define EVENT_MATRIX_ITEM_DRAG			1001
#define EVENT_MATRIX_ITEM_DRAG_END		1002

class CMatrixItem : public CXFrame
{
public:
	CMatrixItem(void);
	~CMatrixItem(void);

	BOOL GetDrag();

	HRESULT OnXMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnDragBegin(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDrag(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDragEnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual	CRect GetDragRect();
	HRESULT SaveDC(BOOL bSave);
	HRESULT QuickPaint(HDC hdc);
private:
	BOOL	m_bDrag;
	HDC	m_hMemDC;
	HBITMAP m_hBmp;
};
