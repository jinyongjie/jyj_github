#include "stdafx.h"
#include "GeneralNotify.h"

#include <atlsafe.h>
CMMapGCallBack	CGeneralObserverMgr::m_mmapGCallBack;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CGeneralObserverMgr::CGeneralObserverMgr()
{

}

CGeneralObserverMgr::~CGeneralObserverMgr()
{
	Destroy();
}

HRESULT	CGeneralObserverMgr::RegisterGeneralObserver( CString guidExt, IGeneralObserver* pExt )
{
	std::pair< CMMapGCallBack::iterator, CMMapGCallBack::iterator >	prGCallBack;

	prGCallBack = m_mmapGCallBack.equal_range( guidExt );
	if ( prGCallBack.first != prGCallBack.second )
	{
		for ( CMMapGCallBack::iterator itr = prGCallBack.first; itr != prGCallBack.second; itr++ )
		{
			if ( itr->second == pExt )
			{
				return E_FAIL;
			}
		}
	}

	m_mmapGCallBack.insert( std::make_pair( guidExt, pExt ) );
	return S_OK;
}

HRESULT	CGeneralObserverMgr::UnregisterGeneralObserver( CString guidExt, IGeneralObserver* pExt )
{
	std::pair< CMMapGCallBack::iterator, CMMapGCallBack::iterator >	prGCallBack;

	prGCallBack = m_mmapGCallBack.equal_range( guidExt );
	if ( prGCallBack.first != prGCallBack.second )
	{
		for ( CMMapGCallBack::iterator itr = prGCallBack.first; itr != prGCallBack.second; itr++ )
		{
			if ( itr->second == pExt )
			{
				m_mmapGCallBack.erase( itr );
				return S_OK;;
			}
		}
	}

	return E_FAIL;
}

HRESULT CGeneralObserverMgr::CallGeneralObserver( CString guidExt, void* pArg )
{
	std::pair< CMMapGCallBack::iterator, CMMapGCallBack::iterator >	prGCallBack;

	prGCallBack = m_mmapGCallBack.equal_range( guidExt );
	if ( prGCallBack.first != prGCallBack.second )
	{
		for ( CMMapGCallBack::iterator itr = prGCallBack.first; itr != prGCallBack.second; itr++ )
		{
			(itr->second)->OnGeneralEvent(guidExt,pArg);
		}
	}

	return S_OK;
}

HRESULT	CGeneralObserverMgr::Destroy()
{
	m_mmapGCallBack.clear();
	return S_OK;
}