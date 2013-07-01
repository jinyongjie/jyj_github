#pragma once
#include "XFrame.h"
namespace Util
{
	BOOL CheckNeedPaint(CXFrame* pTargetFrame, RECT rcClip,CRect rcPaint, RECT *prcPaint );
	HRESULT PaintFrame(CXFrame *pTargetFrame, HDC hdc, CRect rcPaint,CRect rcClip);
	HRESULT BroadcastPaint(CXFrame* pTargetFrame,HDC hdc,CRect rcPaint,CRect rcClip);
	HFONT GetDefaultFont();
	HFONT GetBoldFont();//todo

	HRESULT XMessageBox(HWND hwndParent,CString strText,CString strCaption = L"");
}