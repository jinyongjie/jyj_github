#include "stdafx.h"
#include "Timer.h"
#include <algorithm>

LONG CTimerService::m_lMutex = -1;
LONG CTimerService::m_lTimerState = -1;
HWND CTimerService::m_hTimerWnd = NULL;

CVecTimerItem	CTimerService::m_vTimers;
CSetIdleItem	CTimerService::m_setIdlers;

set< CVecTimerItem* > CTimerService::m_setTimersCalling;

CTimerService::CTimerService()
{
	m_lMutex = -1;
	m_lTimerState = -1;
	m_hTimerWnd = NULL;

	InitWindow();
}

CTimerService::~CTimerService()
{
}

BOOL CTimerService::IsAddrValid(PVOID pInterface, PVOID pCallFuncAddr)
{
	__try
	{
		return **(PVOID**)pInterface == pCallFuncAddr;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}



HRESULT	CTimerService::OnTimer_Internal( WPARAM wParam, LPARAM lParam )
{
	DWORD dwTick = GetTickCount();
	CVecTimerItem vecTimersToCall;

	for(CVecTimerItem::iterator p = m_vTimers.begin(); p!=m_vTimers.end(); )
	{
		if (dwTick - p->uStart >=  p->uTimer)
		{
			p->uStart = dwTick;

			if (!IsAddrValid(p->pCallback, p->pCallFuncAddr))
			{
				p = m_vTimers.erase(p);
			}
			else
			{
				vecTimersToCall.push_back(*p);

				if (!p->bOnce)
				{
					++p;
				}
				else
				{
					p = m_vTimers.erase(p);
				}
			}
		}
		else
		{
			++ p;
		}
	}
	CallTimer(vecTimersToCall);
	return S_OK;
}


static LRESULT CALLBACK TimerWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_TIMER:
		if(wParam == 1)
		{
			CTimerService::OnTimer_Internal( wParam, lParam );
		}
		else
		{
			CTimerService::OnTimerExact_Internal( wParam, lParam );
		}
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


void CTimerService::CallTimer( CVecTimerItem & vecTimers )
{
	__try
	{
		SetItems(&vecTimers, true);
		size_t nsz = vecTimers.size();
		for (size_t i=0; i<nsz; ++i)
		{
			tagTimerItem & ti = vecTimers[i];
			if (ti.bValid)
			{
				ti.pCallback->OnTimerCallback(vecTimers[i].uID);
			}
		}
	}
	__finally
	{
		SetItems(&vecTimers, false);
	}
}


HRESULT CTimerService::SetIdleCallback(IIdleCallback * pCallback, UINT uID)
{
	if( !pCallback ) 
	{
		return E_FAIL;
	}

	CSetIdleItem::iterator itr;
	for ( itr = m_setIdlers.begin(); itr != m_setIdlers.end(); itr++ )
	{
		if ( ( *itr ).pCallback == pCallback )
		{
			if (uID == 0 || ( *itr ).uID == 0 || ( *itr ).uID == uID) return FALSE;
		}
	}

	tagIdlerItem item = {pCallback, **(PVOID**)pCallback, uID};
	m_setIdlers.insert(item);
	return S_OK;
}


HRESULT CTimerService::SetMulTimer(UINT uElapse, ITimerCallback * pCallback, UINT uID)
{
	if(!pCallback) return FALSE;

	uElapse += TIMER_MIN-1;
	uElapse -= uElapse % TIMER_MIN;
	tagTimerItem item = {true, false, pCallback, uID, uElapse, GetTickCount(), **(PVOID**)pCallback};

	return InsertTimerItem( item );
}


void CTimerService::SetItems(CVecTimerItem * ptr, bool bBad)
{
	if (!bBad)
	{
		m_setTimersCalling.erase(ptr);
	}
	else
	{
		m_setTimersCalling.insert(ptr);
	}
}


bool CTimerService::SafeCallIdle(const tagIdlerItem & item)
{
	if (IsAddrValid(item.pCallback, item.pCallFuncAddr))
	{
		item.pCallback->OnIdleCallback(item.uID);
		return true;
	}
	return false;
}


HRESULT CTimerService::InitWindow()
{
	WNDCLASSEX wc    = {sizeof(wc)};
	wc.hInstance     = GetModuleHandle(0);
	wc.lpszClassName = _T("x_timer");
	wc.lpfnWndProc   = &TimerWndProc;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	RegisterClassEx(&wc);
	m_hTimerWnd = CreateWindowEx(0, wc.lpszClassName, _T("TimerWin"), WS_OVERLAPPED, 0, 0, 100, 100, 
		HWND_MESSAGE, NULL, wc.hInstance, NULL);
	ShowWindow(m_hTimerWnd, SW_HIDE);
	m_lTimerState = 0;
	if (m_vTimers.size())
	{
		SetTimer(m_hTimerWnd, 1, TIMER_MIN, NULL);
		m_lTimerState = 1;
	}

	return S_OK;
}



HRESULT	CTimerService::InsertTimerItem( tagTimerItem &item )
{
	for(CVecTimerItem::iterator p = m_vTimers.begin(); p != m_vTimers.end(); ++p)
	{
		if (p->pCallback == item.pCallback)
		{
			if(item.uID == 0 || p->uID == 0 || item.uID == p->uID) return FALSE;
		}
	}
	m_vTimers.push_back(item);

	if ( m_lTimerState == -1 )
	{
		InitWindow();
		m_lTimerState = 0;
	}

	if (m_lTimerState == 0)
	{
		SetTimer(m_hTimerWnd,1,TIMER_MIN,NULL);
		m_lTimerState = 1;
	}

	return TRUE;
}

HRESULT CTimerService::DelIdleCallback(IIdleCallback * pCallback, UINT uID)
{
	HRESULT hr = E_FAIL;
	for(CSetIdleItem::iterator p = m_setIdlers.begin(); p!=m_setIdlers.end(); )
	{
		if (p->pCallback == pCallback && (uID == 0 || uID == p->uID))
		{
			p = m_setIdlers.erase(p);
			hr = S_OK;
		}
		else
		{
			++ p;
		}
	}
	return hr;
}


HRESULT CTimerService::SetOneTimer(UINT uElapse, ITimerCallback * pCallback, UINT uID)
{
	if(!pCallback) return FALSE;

	uElapse += TIMER_MIN-1;
	uElapse -= uElapse % TIMER_MIN;
	tagTimerItem item = {true, true, pCallback, uID, uElapse, GetTickCount(), **(PVOID**)pCallback};

	return InsertTimerItem(item);
}


HRESULT CTimerService::EraseOneTimerCallback(ITimerCallback * pCallback, UINT uID)
{
	HRESULT hr = E_FAIL;

	for (CVecTimerItem::iterator itr = m_vTimers.begin(); itr!=m_vTimers.end(); )
	{
		if (itr->pCallback == pCallback && (uID == 0 || uID == itr->uID))
		{
			itr = m_vTimers.erase(itr);
			hr = S_OK;
		}
		else
		{
			++itr;
		}
	}

	for (std::set< CVecTimerItem* >::iterator itr = m_setTimersCalling.begin(); itr!=m_setTimersCalling.end(); ++itr)
	{
		CVecTimerItem & vt = **itr;
		for (CVecTimerItem::iterator p1=vt.begin(); p1!=vt.end(); ++p1)
		{
			if (p1->pCallback == pCallback && (uID == 0 || uID == p1->uID))
			{
				p1->bValid = false;
			}
		}
	}

	if (m_vTimers.size() == 0)
	{
		KillTimer(m_hTimerWnd,1);
		m_lTimerState = 0;
	}

	return hr;
}


HRESULT CTimerService::NotifyIdle()
{
	if( m_hTimerWnd == 0 )
	{
		InitWindow();
		DWORD dw = GetTickCount();
		for(CVecTimerItem::iterator p = m_vTimers.begin(); p!=m_vTimers.end(); ++p )
		{
			p->uStart = dw;
		}
	}
	if (! InterlockedIncrement(&m_lMutex))
	{
		CSetIdleItem Idlers = m_setIdlers;
		m_setIdlers.clear();

		for ( CSetIdleItem::iterator itr = Idlers.begin(); itr != Idlers.end(); itr++ )
		{
			SafeCallIdle( *itr );
		}
	}
	InterlockedDecrement(&m_lMutex);

	return S_OK;
}

HRESULT CTimerService::SetMulTimerExact(UINT uElapse, ITimerCallback* pCallback)
{
	if (pCallback == NULL)
	{
		return E_INVALIDARG;
	}
	
	UINT_PTR uRet = SetTimer(m_hTimerWnd, (UINT_PTR)pCallback, uElapse, NULL);
	if (uRet == 0)
	{
		
		return E_FAIL;
	}
	return S_OK;
}

HRESULT	CTimerService::OnTimerExact_Internal( WPARAM wParam, LPARAM lParam )
{
	ITimerCallback* pCallback = (ITimerCallback*)wParam;
	if (pCallback == NULL)
	{
		return E_FAIL;
	}
	__try
	{
	
		pCallback->OnTimerCallback(0);
	
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CTimerService::EraseMulTimerExact(ITimerCallback* pCallback)
{
	if (pCallback == NULL)
	{
		return E_INVALIDARG;
	}
	BOOL bSuc = KillTimer(m_hTimerWnd, (UINT_PTR)pCallback);
	if (bSuc)
	{
	
	}
	else
	{
		return E_FAIL;
	}

	return S_OK;
}