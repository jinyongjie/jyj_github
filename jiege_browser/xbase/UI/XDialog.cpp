#include "StdAfx.h"
#include "xdialog.h"

CXDialog::CXDialog(void)
{
	m_nRet = IDOK;
	m_bEndModalLoop = false; 
}

CXDialog::~CXDialog(void)
{
}

HRESULT CXDialog::SetText( CString strText )
{
	return S_OK;
}

LRESULT CXDialog::OnCreate( UINT u, WPARAM w, LPARAM l, BOOL& b)
{
	__super::OnCreate(u,w,l,b);

	

	return 0;
}


int CXDialog::DoModal(HWND hwParent, BOOL bCenterToParent)
{
	if (bCenterToParent && !hwParent)
	{
		CenterWindow();
	}

	if ( bCenterToParent && hwParent )
	{
		CRect	rcParent;
		CRect	rcSelf;
		CRect	rcWorkArea;
		CPoint	ptParentCenter;
		CPoint	ptSelfCenter;

		::GetWindowRect( hwParent, &rcParent );
		GetWindowRect(&rcSelf);
		ptParentCenter = rcParent.CenterPoint();
		ptSelfCenter = rcSelf.CenterPoint();
		rcSelf.left += ptParentCenter.x - ptSelfCenter.x;
		rcSelf.right += ptParentCenter.x - ptSelfCenter.x;
		rcSelf.top += ptParentCenter.y - ptSelfCenter.y;
		rcSelf.bottom += ptParentCenter.y - ptSelfCenter.y;

		int nTemp = 0;
		::SystemParametersInfo( SPI_GETWORKAREA, 0, &rcWorkArea, 0 );
		if ( rcSelf.left < rcWorkArea.left )
		{
			nTemp = rcWorkArea.left - rcSelf.left;
			rcSelf.left += nTemp;
			rcSelf.right += nTemp;
		}
		if ( rcSelf.top < rcWorkArea.top )
		{
			nTemp = rcWorkArea.top - rcSelf.top;
			rcSelf.top += nTemp;
			rcSelf.bottom += nTemp;
		}
		if ( rcSelf.right > rcWorkArea.right )
		{
			nTemp = rcWorkArea.right - rcSelf.right;
			rcSelf.left += nTemp;
			rcSelf.right += nTemp;
		}
		if ( rcSelf.bottom > rcWorkArea.bottom )
		{
			nTemp = rcWorkArea.bottom - rcSelf.bottom;
			rcSelf.top += nTemp;
			rcSelf.bottom += nTemp;
		}
		MoveWindow(rcSelf);
	}

	ShowWindow( SW_SHOW );

	HWND hwThisTop = m_hWnd;

	static std::map<HWND, int>	mapHoldCounter;		
	static std::stack<HWND>		stackModalWnd;		
	std::map<HWND, LONG> mapParentWindowAndExStyle;	

	HWND hwPrevModalWnd	= NULL;
	if ( !stackModalWnd.empty() )
	{
		hwPrevModalWnd	= stackModalWnd.top();
		::EnableWindow(hwPrevModalWnd, FALSE);
	}
	stackModalWnd.push(hwThisTop);

	if ( hwParent )
	{
		LONG lExStyle;
		lExStyle = ::GetWindowLong( hwParent, GWL_EXSTYLE );
		mapParentWindowAndExStyle[hwParent] = lExStyle;

		if ( mapHoldCounter.find(hwParent) != mapHoldCounter.end() )
		{
			mapHoldCounter[hwParent]++;
		}
		else
		{
			mapHoldCounter[hwParent]	= 1;
		}

		for ( std::map<HWND, LONG>::iterator itr = mapParentWindowAndExStyle.begin(); itr != mapParentWindowAndExStyle.end(); itr++ )
		{
			if ( itr->second & WS_EX_TOPMOST )
			{
				::SetWindowPos(hwThisTop, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE );
			}
			::EnableWindow( itr->first, FALSE );
		}
	}

	MSG msg;

	while ( ::GetMessage( &msg, NULL, 0, 0 ) )
	{
		HWND hwForeground = ::GetForegroundWindow();
		CString strWndText;
		::GetWindowText( hwForeground, strWndText.GetBuffer(1024), 1024 );
		strWndText.ReleaseBuffer();

		if ( mapParentWindowAndExStyle.find( hwForeground ) != mapParentWindowAndExStyle.end() )
		{			
			::SetForegroundWindow( hwThisTop);
			::ShowWindow( hwThisTop, SW_RESTORE );
		}		

		if ( msg.hwnd == hwThisTop	&&	( msg.message == WM_CLOSE || msg.message == WM_DESTROY || ( msg.message == WM_SYSCOMMAND && msg.wParam == SC_CLOSE ) ) )
		{
			m_bEndModalLoop = true;
		}
		if ( m_bEndModalLoop )			
		{
			break;
		}

		::TranslateMessage( &msg );
		::DispatchMessage( &msg );
	}

	stackModalWnd.pop();	

	if ( !stackModalWnd.empty() )
	{
		hwPrevModalWnd	= stackModalWnd.top();
		::EnableWindow( hwPrevModalWnd, TRUE );
	}

	std::map<HWND, LONG>::iterator iter;

	for ( iter = mapParentWindowAndExStyle.begin(); iter != mapParentWindowAndExStyle.end(); iter++ )
	{
		if ( mapHoldCounter.find(iter->first) != mapHoldCounter.end() )
		{
			mapHoldCounter[iter->first]--;
		}
		else
		{
			ATLASSERT(FALSE);
		}
	}

	std::map<HWND, int>::iterator iterCounter;

	for ( iterCounter = mapHoldCounter.begin(); iterCounter != mapHoldCounter.end();/* iterCounter++ */)
	{		
		if ( iterCounter->second <= 0 )
		{
			std::map<HWND, LONG>::iterator i = mapParentWindowAndExStyle.find( iterCounter->first );
			if ( i != mapParentWindowAndExStyle.end() )
			{
				::EnableWindow( i->first, TRUE );
			}

			mapHoldCounter.erase(iterCounter++);
		}
		else
		{
			iterCounter++;
		}
	}

	if ( msg.message == WM_QUIT )
	{
		::PostQuitMessage(0);
	}
	else
	{
		::TranslateMessage( &msg );
		::DispatchMessage( &msg );
	}


	return m_nRet;
}

HRESULT CXDialog::EndDialog( int nRet )
{
	m_nRet = nRet;
	m_bEndModalLoop = true;
	return S_OK;
}