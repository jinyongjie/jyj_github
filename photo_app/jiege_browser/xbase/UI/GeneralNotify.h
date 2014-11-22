#pragma once
#include "xcommon.h"
class IGeneralObserver
{
public:
	virtual HRESULT OnGeneralEvent(CString strKey,void* pArg) = 0;
};

CPP_SINK_BEGIN( TGeneralObserver )
CPP_SINK_FUNC(HRESULT,OnGeneralEvent, (CString strKey, void* pArg), (strKey,pArg))
CPP_SINK_END()
DECLARE_CPP_SINK_OBJ(TGeneralObserver,IGeneralObserver)


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef std::multimap< CString, IGeneralObserver* >	CMMapGCallBack;

class CGeneralObserverMgr 
{
public:
	CGeneralObserverMgr();
	virtual	~CGeneralObserverMgr();

public:
	static	HRESULT	Destroy();

	static	HRESULT	RegisterGeneralObserver( CString guidExt, IGeneralObserver* pExt );
	static	HRESULT	UnregisterGeneralObserver( CString guidExt, IGeneralObserver* pExt );
	static	HRESULT CallGeneralObserver( CString guidExt, void* pArg );


protected:
	static	CMMapGCallBack	m_mmapGCallBack;
};
