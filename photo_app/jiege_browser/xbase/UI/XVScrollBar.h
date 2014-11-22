#pragma once
#include "xframe.h"
#include "xbutton.h"

#define EVENT_SCROLLBAR_ROLL	0

class CXVScrollBar :
	public CXFrame
{
public:
	CXVScrollBar(void);
	~CXVScrollBar(void);
	
	HRESULT Relayout();
	virtual	HRESULT	Paint(HDC hdc,CRect rcDrawRect);

	HRESULT	SetRange(INT nRange);
	INT GetPos();
	HRESULT SetPos(INT nPos);
	LRESULT OnXMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	HRESULT SetSliderBkg(CXDraw* pNormal,CXDraw* pOn,CXDraw* pDown);
	HRESULT SetStep(int nStep);
	HRESULT Roll(int nStep);
private:
	LRESULT OnLButtonClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseEnter(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCaptureChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
private:
	enum ESliderState
	{
		e_slider_normal,
		e_slider_on,
		e_slider_down,
	};
	BOOL Internal_PtInRect( POINT pt ,CRect rc);
	HRESULT Internal_SetSliderState(ESliderState eState);
private:


	CXDraw* m_pBkgSliderNormal;
	CXDraw* m_pBkgSliderOn;
	CXDraw* m_pBkgSliderDown;
	ESliderState m_eSliderState;

	CRect m_rcSlier;//滑动条绘制区域
	INT m_nRange;	//滚动总范围
	INT m_nPos;		//当前位置
	INT m_nStep;	//每次滚动幅度

	BOOL m_bDrag;
	INT m_nMouseOffset;//拖拽时相对鼠标距离
};
