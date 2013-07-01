#pragma once
#include "XFrame.h"
#include "xtext.h"
class CXButton :public CXFrame
{
public:
	CXButton(void);
	~CXButton(void);

public:
	HRESULT SetBkgNormal(CXDraw* pDraw);
	CXDraw* GetBkgNormal();

	HRESULT SetBkgOn(CXDraw* pDraw);
	CXDraw* GetBkgOn();


	HRESULT SetBkgDown(CXDraw* pDraw);
	CXDraw* GetBkgDown();


	HRESULT SetText(CString strText);
	CString GetText();

	HRESULT SetTextColor( COLORREF color);

	HRESULT SetTextOffset(CPoint pt);

	HRESULT Paint(HDC hdc,CRect rcDrawRect);
	virtual LRESULT OnXMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	HRESULT SetState(EXButtonState eState);
	HRESULT Relayout();
public:
	
	LRESULT OnLButtonClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseEnter(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
private:
	CXDraw* m_pBkgNormal;
	CXDraw* m_pBkgOver;
	CXDraw* m_pBkgPush;

	EXButtonState	m_eButtonState;
	CXText* m_pText;
	CPoint m_ptTextOffset;
};
