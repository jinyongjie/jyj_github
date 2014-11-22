#pragma once
#include "XWindow.h"
class CXGLWindow:public CXWindow
{
public:
	CXGLWindow(void);
	~CXGLWindow(void);


	BEGIN_MSG_MAP(CXWindow)
		MESSAGE_HANDLER(WM_PAINT,OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND,OnEraseBkgrnd)
		MESSAGE_HANDLER(WM_SIZE,OnSize)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		MESSAGE_HANDLER(WM_DESTROY,OnDestroy)
	END_MSG_MAP()

	int InitGL();
	HRESULT Draw(HDC hdc);


	LRESULT OnCreate(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnPaint( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkgrnd( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};
