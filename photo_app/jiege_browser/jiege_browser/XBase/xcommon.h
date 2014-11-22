#pragma once


#include <atlbase.h>
#include <atlstr.h>
#include <atlcom.h>
#include <atltypes.h>


#include <atlapp.h>
#include <atlctrls.h>

#include <map>
#include <vector>
#include <set>
#include <stack>
using namespace std;

#define CPP_SINK_BEGIN( SinkName ) \
	template< class CHost, class ISink > \
class SinkName : public ISink \
{ \
public: \
	SinkName( CHost *pHost ) \
{ \
	memset((BYTE*)&m_pHost, 0, sizeof(*this) - ((BYTE*)&m_pHost - (BYTE*)this)); \
	m_pHost = pHost; \
} \
	virtual	~SinkName() \
{ \
} \
	void UnhookAll() \
{ \
	CHost*	pHost = m_pHost; \
	memset((BYTE*)&m_pHost, 0, sizeof(*this) - ((BYTE*)&m_pHost - (BYTE*)this)); \
	m_pHost = pHost; \
} \
public: \
	CHost*	m_pHost; \

#define CPP_SINK_END() \
}; \

/*	memset((BYTE*)&m_pHost, 0, sizeof(*this) - ((BYTE*)&m_pHost - (BYTE*)this)); 
的作用是将所有成员变量赋0, m_pHost 位于地址最小端,声明于其后的成员变量地址依次增大
*/

#define CPP_SINK_FUNC_VOID_RET(func, params, values) \
public: \
	typedef void (CHost::*F##func)params; \
	\
	void Hook_##func(F##func pf##func) \
{ \
	m_pf##func		= pf##func; \
} \
	\
	void Unhook_##func() \
{ \
	m_pf##func	= NULL; \
} \
	\
	virtual void (func)params \
{ \
	if (m_pHost && m_pf##func) \
{ \
	( (m_pHost)->*(m_pf##func))values; \
} \
} \
	\
private: \
	F##func	m_pf##func; \


#define CPP_SINK_FUNC( RetType, FuncName, ParamDeclare, CallArg ) \
public: \
	typedef RetType (CHost::*F##FuncName)ParamDeclare; \
	\
	void Hook_##FuncName(F##FuncName pf##FuncName) \
{ \
	m_pf##FuncName		= pf##FuncName; \
} \
	\
	void Unhook_##FuncName() \
{ \
	m_pf##FuncName	= NULL; \
} \
	\
	virtual RetType  FuncName ParamDeclare \
{ \
	if (m_pHost && m_pf##FuncName) \
{ \
	return ( (m_pHost)->*(m_pf##FuncName))CallArg; \
} \
	return (RetType)(-1); \
} \
	\
private: \
	F##FuncName	m_pf##FuncName; \

#define DECLARE_CPP_SINK_OBJ( SinkName, ISink ) \
	template< class CHost > \
class SinkName##Obj : public SinkName< CHost, ISink > \
{ \
public: \
	SinkName##Obj( CHost *pHost ) : SinkName< CHost, ISink >( pHost ) \
{ \
} \
	virtual ~SinkName##Obj() \
{ \
} \
}; \


class CXFrame;

// event begin
class IEventObserver
{
public:
	virtual HRESULT OnXEvent(INT nId, CXFrame* pEventSource, void* pArg) = 0;
};

CPP_SINK_BEGIN( TXEventObserver )
CPP_SINK_FUNC(HRESULT,OnXEvent, (INT nId, CXFrame* pEventSource, void* pArg), (nId, pEventSource, pArg))
CPP_SINK_END()
DECLARE_CPP_SINK_OBJ(TXEventObserver,IEventObserver)


// 事件挂接
#define ADD_EVENT_OBSERVER( frame, id, listener ) \
{ \
	if ( (frame) == NULL || (listener) == NULL ) \
{ \
	ATLASSERT(FALSE); \
} \
	else  \
{ \
	frame->AddEventObserver( (id), (listener) ); \
} \
}


#define DEL_EVENT_OBSERVER( frame, id, listener ) \
{ \
	if ( (frame) == NULL || (listener) == NULL ) \
{ \
	ATLASSERT(FALSE); \
} \
	else \
{ \
	frame->RemoveEventObserver( (id), (listener) ); \
} \
}


#define CLEAR_EVENT_OBSERVER( frame ) \
{ \
	if ( (frame) == NULL ) \
{ \
	ATLASSERT(FALSE); \
} \
	else  \
{ \
	frame->ClearEventObserver(); \
} \
}

#define BEGIN_EVENT_MAP(pEventSource, Id, Arg) \
{ \
	if (pEventSource) \
{ \
	INT id = Id; \
	void* arg = Arg;

#define ON_EVENT(control, func) \
{ \
	if (control) \
{ \
	if (pEventSource == control) \
{ \
	func(id, arg); \
	goto _finish0;\
} \
} \
} 

#define ON_EVENT_BY_ID(control, nId, func) \
{ \
	if (control) \
{ \
	if (pEventSource == control && nId == id) \
{ \
	func(arg); \
	goto _finish0;\
} \
} \
}

#define END_EVENT_MAP() \
_finish0:\
	;\
} \
}

enum EStackPanelType
{
	e_stackpanel_v = 0,
	e_stackpanel_h,
};


class CClipDC
{
protected:
	HDC m_hDC;
	int m_nSavedDC;
public:
	CClipDC::CClipDC( HDC hdc, RECT rcClip ) 
	{
		m_hDC	   = hdc;
		m_nSavedDC = SaveDC(hdc);

		HRGN hRgn = ::CreateRectRgnIndirect( &rcClip );

		::ExtSelectClipRgn( hdc, hRgn, RGN_AND );
		::DeleteObject(hRgn);
	}

	CClipDC::CClipDC( HDC hdc, HRGN hRgn ) 
	{
		m_hDC	   = hdc;
		m_nSavedDC = SaveDC(hdc);

		::ExtSelectClipRgn( hdc, hRgn, RGN_AND );
	}

	CClipDC::~CClipDC()
	{
		RestoreDC( m_hDC, m_nSavedDC );
	}
};

#define WM_X_BEGIN			(WM_USER + 500)
#define WM_X_RELAYOUT		(WM_X_BEGIN + 1)
#define WM_X_LBUTTONCLICK	(WM_X_BEGIN + 2)
#define WM_X_MOUSEENTER		(WM_X_BEGIN + 3)
#define WM_X_MOUSELEAVE		(WM_X_BEGIN + 4)
#define WM_X_DRAGBEGIN		(WM_X_BEGIN + 5)
#define WM_X_DRAG			(WM_X_BEGIN + 6)
#define WM_X_DRAGEND		(WM_X_BEGIN + 7)
#define WM_X_LONGPUSH		(WM_X_BEGIN + 8)

enum EXButtonState
{
	e_button_state_normal = 0,
	e_button_state_hover,
	e_button_state_down,
};


#define XBEGIN_MSG_MAP( uMsg,  wParam,  lParam, bHandled) \
{\
LRESULT lResult = 0;

#define XMESSAGE_HANDLER(msg, func) \
	if(uMsg == msg) \
{ \
	bHandled = TRUE; \
	lResult = func(uMsg, wParam, lParam, bHandled); \
	if(bHandled) \
	return lResult; \
	}


#define XEND_MSG_MAP() \
	return __super::OnXMessage(uMsg,  wParam,  lParam, bHandled);\
	}


#define EVENT_BUTTON_LBUTTON_CLICK	0
#define EVENT_BUTTON_MOUSELEAVE	1

enum EXScrollType
{
	e_xscrolltype_h =0,
	e_xscrolltype_v,
};

#define XDOCK_LEFT	0
#define XDOCK_TOP	1
#define XDOCK_RIGHT	2
#define XDOCK_BOTTOM	3
#define XDOCK_FILL	4

#define XDRAWTYPE_NORMAL	0
#define XDRAWTYPE_STRETCH	1
#define XDRAWTYPE_9PART		2
#define XDRAWTYPE_3PARTH	3
#define XDRAWTYPE_3PARTV	4
#define XDRAWTYPE_TITLE		5
