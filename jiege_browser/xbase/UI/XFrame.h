#pragma once
#include "xcommon.h"
#include "xdraw.h"
class CXWindow;
class CXFrame
{
public:
	CXFrame(void);
	~CXFrame(void);

public:
	virtual	HRESULT	AddFrame(CXFrame* pFrame);
	virtual	HRESULT	DelFrame(CXFrame* pFrame);
	virtual HRESULT InsertFrame(CXFrame* pFrame,int nIndex);
	virtual HRESULT	Clear();

	virtual	int		GetFrameCount();
	virtual CXFrame* GetFrameByIndex(int nIndex);

	virtual	HRESULT	Create(CXFrame* pParent);

	virtual	HRESULT	Relayout();

	virtual	HRESULT SetRect(CRect rc);
	virtual CRect	GetRect();

	virtual	HRESULT SetPosition(CPoint pt);
	virtual CPoint	GetPosition();

	virtual	HRESULT SetMargin(CRect rc);
	virtual CRect	GetMargin();

	virtual	HRESULT SetParent(CXFrame* pParent);
	virtual CXFrame* GetParent();

	virtual	HRESULT SetWindow(CXWindow* pParent);
	virtual CXWindow* GetWindow();

	virtual CRect	GetPaintRect();

	virtual HRESULT Destroy();

	virtual HRESULT	SetBackground(CXDraw* pDraw);
	virtual	CXDraw* GetBackground();

	virtual HRESULT	SetBackground2(CXDraw* pDraw);
	virtual	CXDraw* GetBackground2();

	virtual LRESULT OnXMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual	HRESULT	Paint(HDC hdc,CRect rcRect);

	HRESULT	InvalidateFrame();

	virtual	HWND GetHwnd();

	HRESULT AddEventObserver( INT nId, IEventObserver* pEventListener );
	HRESULT AddAllEventObserver( IEventObserver* pEventListener );
	HRESULT RemoveEventObserver( INT nId, IEventObserver* pEventListener );
	HRESULT ClearEventObserver( void );

	HRESULT ThrowEvent(INT nId, void* pArg );

	CRect 	GetScreenRect();
	
	HRESULT SetVisible(BOOL bVisible);
	BOOL GetVisible();

	HRESULT SetName(LPCWSTR strName);
	CString GetName();

	HRESULT SetParentVisible(BOOL bVisible);
	HRESULT MarkRelayout();

	HRESULT SetData(void* pData);
	void* GetData();
	BOOL IsAncestry(CXFrame* pAncestry);

	HRESULT SetEnableDrag(BOOL bEnable);
	BOOL	GetEnableDrag();

	virtual HRESULT SetAlpha(BYTE cAlpha);
	BYTE	GetAlpha();
	HRESULT SetAutoHeight(BOOL bVal);
	BOOL	GetAutoHeight();

	HRESULT SetAutoWidth(BOOL bVal);
	BOOL	GetAutoWidth();

	HRESULT SetNCHittest(int nVal);
	int	GetNCHittest();

	HRESULT SetDockType(int nDockType);
	int		GetDockType();
	BOOL	GetNeedRelayout();
	HRESULT SetSensitiveChildSize(BOOL bVal);
	BOOL	GetSensitiveChildSize();
public:
	LRESULT OnSetCursor( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT OnMouseWheel( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT OnKeyDown( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
protected:
	HRESULT Internal_syncPaintRect();

protected:
	typedef	vector<CXFrame*>	VEC_CHILD;
	VEC_CHILD	m_vecChild;
protected:
	CXFrame*	m_pParentFrame;
	CXWindow*	m_pWindow;

	CRect	m_rcRect;	//相对父亲Frame
	CRect	m_rcPaint;	//相对顶层真实窗口
	CRect	m_rcMargin;

	CXDraw*	m_pBackground;
	CXDraw*	m_pBackground2;

	BOOL	m_bVisible;
	BOOL	m_bParentVisible;

#pragma warning(push)
#pragma warning(disable:4251)
	map<int,set<IEventObserver*>> m_mapEventListener;
#pragma warning(pop)

	CString m_strName;
	void*	m_pData;
	BOOL	m_bEnableDrag;
	BYTE	m_cAlpha;
	BOOL	m_bAutoHeight;
	BOOL	m_bAutoWidth;

	INT m_nDockType;
	BOOL	m_bNeedRelayout;
	BOOL	m_bSensitiveChildSize;//当child大小变化时得到通知
	int		m_nNCHittest;
};
