#pragma once
#include "xcommon.h"
class ITimerCallback
{
public:
	virtual HRESULT OnTimerCallback(UINT id) = 0;
};

CPP_SINK_BEGIN( TTimerCallback )
CPP_SINK_FUNC(HRESULT,OnTimerCallback, ( UINT id), (id))
CPP_SINK_END()
DECLARE_CPP_SINK_OBJ(TTimerCallback,ITimerCallback)

class IIdleCallback
{
public:
	virtual HRESULT OnIdleCallback(UINT UINT) = 0;
};

CPP_SINK_BEGIN( TIdleCallback )
CPP_SINK_FUNC(HRESULT,OnIdleCallback, ( UINT UINT), (id))
CPP_SINK_END()
DECLARE_CPP_SINK_OBJ(TIdleCallback,IIdleCallback)


enum { TIMER_MIN = 50 };
struct tagTimerItem
{
	bool bValid;
	bool bOnce;
	ITimerCallback* pCallback;
	UINT uID;
	UINT uTimer;
	UINT uStart;
	PVOID pCallFuncAddr;
};
struct tagIdlerItem
{
	IIdleCallback* pCallback;
	PVOID pCallFuncAddr;
	UINT uID;
	bool operator < ( const tagIdlerItem & rhs) const
	{
		if (pCallback != rhs.pCallback)
		{
			return pCallback < rhs.pCallback;
		}
		return uID < rhs.uID;
	}
};

typedef vector< tagTimerItem >	CVecTimerItem;
typedef set< tagIdlerItem >		CSetIdleItem;

class ATL_NO_VTABLE CTimerService 
{
public:
	CTimerService();
	virtual	~CTimerService();

	static	HRESULT EraseOneTimerCallback(ITimerCallback * pCallback, UINT uID);	
	static	HRESULT NotifyIdle();	
	static	HRESULT SetIdleCallback(IIdleCallback * pCallback, UINT uID);	
	static	HRESULT DelIdleCallback(IIdleCallback * pCallback, UINT uID);	
	static	HRESULT SetOneTimer(UINT uElapse, ITimerCallback * pCallback, UINT uID);	
	static	HRESULT	OnTimer_Internal( WPARAM wParam, LPARAM lParam );
	static	HRESULT SetMulTimer(UINT uElapse, ITimerCallback * pCallback, UINT uID);

	static	HRESULT SetMulTimerExact(UINT uElapse, ITimerCallback* pCallback);
	static	HRESULT EraseMulTimerExact(ITimerCallback* pCallback);
	static	HRESULT	OnTimerExact_Internal( WPARAM wParam, LPARAM lParam );
protected:
	static	HRESULT InitWindow();
	static	BOOL	IsAddrValid( PVOID pInterface, PVOID pCallFuncAddr );
	static	void	CallTimer( CVecTimerItem & vecTimers );
	static	void	SetItems( CVecTimerItem *ptr, bool bBad );
	static	bool	SafeCallIdle( const tagIdlerItem & item );
	static	HRESULT	InsertTimerItem( tagTimerItem &item );

protected:
	static	LONG m_lMutex;
	static	LONG m_lTimerState;
	static	HWND m_hTimerWnd;

	static	CVecTimerItem	m_vTimers;
	static	CSetIdleItem	m_setIdlers;

	static	set< CVecTimerItem* > m_setTimersCalling;
};
