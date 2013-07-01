#pragma once
#include "XFrame.h"

class CXWindow:public CWindowImpl<CXWindow>
{
public:
	CXWindow(void);
	~CXWindow(void);
public:
	HRESULT SetRootFrame(CXFrame* pTopFrame);
	CXFrame* GetRootFrame();
	HRESULT MarkRelayout(CXFrame* pFrame);
	void SetMargin(RECT rc);
	RECT GetMargin();

	void SetRect(RECT rc);
	RECT GetRect();

	HRESULT SetBorder(CRect rcBorder);
	CRect GetBorder();

	HRESULT NotifyFrameRemoved(CXFrame* pFrame);
	HRESULT SetCalcRgn(BOOL bVal);

public:

	DECLARE_WND_CLASS_EX(_T("XWindow"),CS_DBLCLKS,HOLLOW_BRUSH)

	BEGIN_MSG_MAP(CXWindow)
		MESSAGE_HANDLER(WM_PAINT,OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND,OnEraseBkgrnd)
		MESSAGE_HANDLER(WM_SIZE,OnSize)
		MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
		MESSAGE_HANDLER(WM_CAPTURECHANGED,OnCaptureChanged)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		MESSAGE_HANDLER(WM_DESTROY,OnDestroy)
		MESSAGE_HANDLER(WM_X_RELAYOUT,OnRelayout)
		MESSAGE_HANDLER(WM_MOUSEMOVE,OnMouseMove)
		MESSAGE_HANDLER(WM_NCMOUSEMOVE,OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE,OnMouseLeave)
		MESSAGE_HANDLER(WM_NCMOUSELEAVE,OnMouseLeave)
		MESSAGE_HANDLER(WM_MOUSEWHEEL,OnMouseWheel)
		MESSAGE_HANDLER(WM_LBUTTONDOWN,OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP,OnLButtonUP)
		MESSAGE_HANDLER(WM_SETCURSOR,OnSetCursor)
		MESSAGE_HANDLER(WM_NCHITTEST,OnNCHittest)
		MESSAGE_HANDLER(WM_NCCALCSIZE,OnNCCalcSize)
		MESSAGE_HANDLER(WM_NCACTIVATE,OnNCActivate)
		MESSAGE_HANDLER(WM_KEYDOWN,OnKeyDown)
		//MESSAGE_HANDLER(WM_WINDOWPOSCHANGING,OnPosChanging)
	END_MSG_MAP()

protected:
	virtual	LRESULT OnCreate(UINT, WPARAM, LPARAM, BOOL&);
	virtual	LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnPaint( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual HRESULT PaintBkg(HDC hdc);
	LRESULT OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkgrnd( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNotify( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCaptureChanged(UINT, WPARAM, LPARAM, BOOL&);
	
	LRESULT OnRelayout(UINT, WPARAM, LPARAM, BOOL&);
	virtual	LRESULT OnMouseMove(UINT, WPARAM, LPARAM, BOOL&);
	virtual	LRESULT OnMouseLeave(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnLButtonDown(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnLButtonUP(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnSetCursor(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnMouseWheel(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnNCHittest(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnNCCalcSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNCActivate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyDown( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPosChanging( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	HRESULT GetTopFrameFromPoint( POINT ptMousePos, CXFrame** ppTargetFrame );
	HRESULT DoPaint();
	HRESULT	Relayout();


private:
	HRESULT Internal_RelayoutFrame(CXFrame* pFrame);
protected:
	CXFrame*	m_pRootFrame;

	CXFrame* m_pLButtonDown;
	CXFrame* m_pMouseIn;
	BOOL	m_bDrag;

	CRect m_rcMargin;
	CRect m_rcRect;
	CPoint m_ptLButtonDown;
	vector<CXFrame*> m_vecRelayout;
	vector<CXFrame*> m_vecRelayoutTmp;
	BOOL	m_bRelayout;
	CRect m_rcBorder;
	BOOL	m_bCalcRgn;

	bool								m_bTracking;
	TRACKMOUSEEVENT						m_Tme; 
};
