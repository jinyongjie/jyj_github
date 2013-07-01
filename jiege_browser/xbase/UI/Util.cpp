#include "StdAfx.h"
#include "Util.h"
#include "XFrame.h"
#include "XMessageBox.h"
#include "strsafe.h"
struct tagPaintSearchingNode 
{
	int					nIdx;
	CXFrame*			pNode;
	RECT				rcClip;
	double				dblAlphaAncestor; 

	tagPaintSearchingNode( int _nIdx, CXFrame* _pNode, const RECT &_rcClip, double _dblAlphaAncestor )
	{
		nIdx = _nIdx;
		pNode = _pNode;
		rcClip = _rcClip;
		dblAlphaAncestor = _dblAlphaAncestor;
	}
};
typedef std::stack< tagPaintSearchingNode >	CStkPaintSearchingNode;


BOOL Util::CheckNeedPaint(CXFrame* pTargetFrame, RECT rcClip,CRect rcPaint, RECT *prcNewClip )
{
	CXFrame* 	pParentFrame;
	RECT				rcParentPaint;

	BOOL				bOwnerDraw;

	BOOL				bVisible;

	if ( pTargetFrame == NULL  )
	{
		ATLASSERT(FALSE);
		return FALSE;
	}

	bVisible = pTargetFrame->GetVisible(  );

	if (!bVisible )
	{
		return FALSE;
	}

	if ( IntersectRect( prcNewClip, &rcPaint, &rcClip ) != 0 )
	{
		return TRUE;
	}


	return FALSE;
}

HRESULT	Util::PaintFrame(CXFrame *pTargetFrame, HDC hdc, CRect rcPaint,CRect rcClip)
{
	if(pTargetFrame == NULL)
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}
	CClipDC			*pClipDC = NULL;

	pClipDC = new CClipDC( hdc, rcClip ); 

	BYTE cAlpha = pTargetFrame->GetAlpha();
	if(cAlpha == 255)
	{
		pTargetFrame->Paint(hdc,rcPaint);
	}
	else
	{
		HDC		hdcMem;
		HBITMAP	hBmp, hBmpOld;

		hdcMem = ::CreateCompatibleDC(hdc);
		hBmp = ::CreateCompatibleBitmap( hdc, rcPaint.Width(),rcPaint.Height() );

		if ( hdcMem && hBmp)
		{
			hBmpOld = ( HBITMAP )::SelectObject( hdcMem, hBmp );

			pTargetFrame->Paint(hdcMem,CRect(0,0,rcPaint.Width(),rcPaint.Height()));
			BLENDFUNCTION bf = {AC_SRC_OVER, 0, cAlpha, AC_SRC_ALPHA};

			::AlphaBlend(hdc,rcPaint.left,rcPaint.top,rcPaint.Width(),rcPaint.Height(),
				hdcMem,0,0,rcPaint.Width(),rcPaint.Height(),
				bf);

			::SelectObject( hdcMem, hBmpOld );
			::DeleteObject( hBmp );
			::DeleteDC( hdcMem );
		}

	}
	delete pClipDC;

	return S_OK;
}
HRESULT Util::BroadcastPaint(CXFrame *pTargetFrame, HDC hdc, CRect rcPaint,CRect rcClip)
{
	CStkPaintSearchingNode	stkBacktrace; 
	tagPaintSearchingNode		*pSearchingNode;
	CXFrame* 		pNodeCurrent;

	INT					nFrameCount;

	BYTE					cAlpha = 255;

	if ( !pTargetFrame )
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}
	CRect rcNewClip;
	if (  CheckNeedPaint( pTargetFrame, rcClip,rcPaint, &rcNewClip  )  )
	{
		stkBacktrace.push( tagPaintSearchingNode( -1, pTargetFrame, rcNewClip, 1.0 ) );

		PaintFrame(  pTargetFrame, hdc,rcPaint,rcNewClip ); 
	}
	CRect rcTargetRect = pTargetFrame->GetPaintRect();//²ÎÕÕÏµ
	while (stkBacktrace.empty() == false )	
	{
		pSearchingNode = &( stkBacktrace.top() );

		pSearchingNode->nIdx++;
		nFrameCount = pSearchingNode->pNode->GetFrameCount( );
		if( pSearchingNode->nIdx < nFrameCount)
		{
			pNodeCurrent = pSearchingNode->pNode->GetFrameByIndex( pSearchingNode->nIdx ); 
			if (  pNodeCurrent )
			{
				CRect rcChildPaint = pNodeCurrent->GetPaintRect();
				CRect rcChildNewPaint(rcChildPaint.left - rcTargetRect.left,
					rcChildPaint.top - rcTargetRect.top,
					rcChildPaint.right - rcTargetRect.left,
					rcChildPaint.bottom - rcTargetRect.top);
				if ( CheckNeedPaint( pNodeCurrent, pSearchingNode->rcClip,rcChildNewPaint,  &rcNewClip )   )
				{
					stkBacktrace.push( tagPaintSearchingNode( -1, pNodeCurrent, rcNewClip, pSearchingNode->dblAlphaAncestor * cAlpha / 255.0 ) );

					PaintFrame( pNodeCurrent,hdc,rcChildNewPaint,pSearchingNode->rcClip );
				}

				pNodeCurrent = NULL;
			}
		}
		else
		{
			stkBacktrace.pop();
		}
	}
	return S_OK;
}


HFONT Util::GetDefaultFont()
{
	static HFONT hFont = NULL;

	if(hFont == NULL)
	{
		CLogFont lf = ((HFONT)GetStockObject(DEFAULT_GUI_FONT));
		lf.lfHeight = -12;


		DWORD dwVersion = 0; 
		DWORD dwMajorVersion = 0;
		DWORD dwMinorVersion = 0; 
		DWORD dwBuild = 0;

		dwVersion = GetVersion();

		dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
		dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));


		if(dwMajorVersion >= 6)
		{
			StringCbCopy(lf.lfFaceName,sizeof(lf.lfFaceName),L"Î¢ÈíÑÅºÚ");
		}
		else
		{
			StringCbCopy(lf.lfFaceName,sizeof(lf.lfFaceName),L"ËÎÌå");
		}
		hFont = ::CreateFontIndirect(&lf);
	}
	return hFont;
}
HFONT Util::GetBoldFont()
{
	static HFONT hFont = NULL;

	if(hFont == NULL)
	{
		CLogFont lf = ((HFONT)GetStockObject(DEFAULT_GUI_FONT));
		lf.lfHeight = -12;
		lf.lfWeight = FW_BOLD;

		DWORD dwVersion = 0; 
		DWORD dwMajorVersion = 0;
		DWORD dwMinorVersion = 0; 
		DWORD dwBuild = 0;

		dwVersion = GetVersion();

		dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
		dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));


		if(dwMajorVersion >= 6)
		{
			StringCbCopy(lf.lfFaceName,sizeof(lf.lfFaceName),L"Î¢ÈíÑÅºÚ");
		}
		else
		{
			StringCbCopy(lf.lfFaceName,sizeof(lf.lfFaceName),L"ËÎÌå");
		}
		hFont = ::CreateFontIndirect(&lf);
	}
	return hFont;
}
HRESULT Util::XMessageBox(HWND hWndParent, CString strText,CString strCaption /*= L""*/ )
{
	CXMessageBox msgbox;
	msgbox.Create(hWndParent,NULL,NULL,WS_POPUP);
	msgbox.SetText(strText);
	msgbox.DoModal(hWndParent,TRUE);

	return S_OK;
}