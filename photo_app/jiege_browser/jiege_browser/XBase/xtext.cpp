#include "StdAfx.h"
#include "xtext.h"
#include "Util.h"
CXText::CXText(void)
{
	m_dwFormat = DT_NOPREFIX;
	m_clr = RGB(0,0,0);
	m_bSuportAlpha = FALSE;
	m_hFont = Util::GetDefaultFont();
}

CXText::~CXText(void)
{
	//if(m_hFont)
	//{
	//	DeleteObject(m_hFont);
	//}
}
int DrawTextWithAlpha(
				  HDC hDC,          // handle to DC
				  LPCTSTR lpString, // text to draw
				  int nCount,       // text length
				  LPRECT lpRect,    // formatting dimensions
				  UINT uFormat      // text-drawing options
				  )
{
	// 如果只是计算大小，直接返回大小
	if (uFormat & DT_CALCRECT)
		return DrawText(hDC, lpString, nCount, lpRect, uFormat);

	// 参数检查
	if (!lpRect)
		return 0;

	// 创建容纳文字的位图
	unsigned long *pBits = NULL;
	HBITMAP hBitmap = NULL;

	BITMAPINFO bmInfo; 
	ZeroMemory(&bmInfo, sizeof(bmInfo));
	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biWidth = lpRect->right - lpRect->left;
	bmInfo.bmiHeader.biHeight = lpRect->bottom - lpRect->top;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biBitCount = 32;

	hBitmap = CreateDIBSection(NULL, &bmInfo, DIB_RGB_COLORS, (void**)&pBits, NULL, 0);
	if (!pBits)
		return 0;

	COLORREF	clrText = GetTextColor(hDC);
	// 首先将位图的每个像素置成屏蔽值 (0x01010101)
	unsigned long uPix = 0;
	unsigned char* pix = (unsigned char*)&uPix;
	BYTE byte = 255 - (GetRValue(clrText) + GetGValue(clrText) + GetBValue(clrText))/3;
	pix[0] = byte ; pix[1] = 1; pix[2] = 1; pix[3] = 1;
	memset( pBits, uPix, sizeof(unsigned long) * bmInfo.bmiHeader.biWidth * bmInfo.bmiHeader.biHeight);


	// 向位图中绘制文字
	HDC dcMem = CreateCompatibleDC(hDC);
	if (!dcMem)
	{
		DeleteObject(hBitmap);
		return 0;
	}
	HGDIOBJ hOldBitmap = SelectObject(dcMem, (HGDIOBJ)hBitmap);
	int nBkModeOld = SetBkMode(dcMem, TRANSPARENT);
	int nColorOld = SetTextColor(dcMem, clrText);
	HGDIOBJ hOldFont = SelectObject(dcMem, SelectObject(hDC, (HFONT)GetStockObject(SYSTEM_FIXED_FONT)));

	RECT rectToDraw(*lpRect);
	rectToDraw.right = rectToDraw.right - rectToDraw.left;
	rectToDraw.bottom = rectToDraw.bottom - rectToDraw.top;
	rectToDraw.top = rectToDraw.left = 0;
	int nRtn = DrawText(dcMem, lpString, nCount, &rectToDraw, uFormat);

	SelectObject(hDC, SelectObject(dcMem, hOldFont));
	SetTextColor(dcMem, nColorOld);
	SetBkMode(dcMem, nBkModeOld);



	// 对生成的文字图片进行透明度处理
	// 对像素为屏蔽值区域进行全透明，
	// 对像素为全透明的区域，变成全不透明，
	// 其它区域不进行任何处理
	int nTextColor =  GetTextColor(hDC);
	int r = GetRValue(nTextColor);
	int g = GetGValue(nTextColor);
	int b = GetBValue(nTextColor);


	int nSize = bmInfo.bmiHeader.biWidth * bmInfo.bmiHeader.biHeight;
	for (int i = 0; i < nSize; i++)
	{
		pix = (unsigned char*)&pBits[i];
		if (pix[0] == byte && pix[1] == byte && pix[2] == byte && pix[3] == byte)
		{ pix[0] = 0; pix[1] = 0; pix[2] = 0; pix[3] = 0; }
		else if (pix[3] == 0)
		{ /*pix[0] = b; pix[1] = g; pix[2] = r;*/ pix[3] = 255; }
	}

	// 将图像最终绘制到DC上
	BLENDFUNCTION blend;
	blend.BlendOp               = AC_SRC_OVER;
	blend.BlendFlags            = 0;
	blend.SourceConstantAlpha   = 255;
	blend.AlphaFormat           = AC_SRC_ALPHA;
	AlphaBlend(
		hDC, 
		lpRect->left, lpRect->top, 
		lpRect->right - lpRect->left,
		lpRect->bottom - lpRect->top,
		dcMem, 0, 0, 
		lpRect->right - lpRect->left,
		lpRect->bottom - lpRect->top,
		blend);


	SelectObject(dcMem, hOldBitmap);
	DeleteDC(dcMem);
	DeleteObject(hBitmap);

	return nRtn;
}
HRESULT CXText::Draw( HDC hdc,CRect rcPaint )
{
	HGDIOBJ hOldFont = NULL;
	HFONT hFont = m_hFont;
	if(hFont == NULL)
	{
		hFont = Util::GetDefaultFont();
	}

	hOldFont = ::SelectObject(hdc,hFont);
	
	int nBkMode =	::SetBkMode(hdc,TRANSPARENT);
	COLORREF clrOld = ::SetTextColor(hdc,m_clr);
	
	if(m_bSuportAlpha)
	{
		DrawTextWithAlpha(hdc,m_strText,m_strText.GetLength(),&rcPaint,m_dwFormat);
	}
	else
	{
		::DrawText(hdc,m_strText,m_strText.GetLength(),&rcPaint,m_dwFormat);
	}
	
	
	::SetTextColor(hdc,clrOld);
	::SetBkMode(hdc,nBkMode);
	if(hOldFont)
	{
		::SelectObject(hdc,hOldFont);
	}

	return S_OK;
}

HRESULT CXText::SetFont( HFONT hFont )
{
	m_hFont = hFont;
	return S_OK;
}

HRESULT CXText::SetText(LPCWSTR strText)
{
	m_strText = strText;
	return S_OK;
}
HRESULT CXText::SetFormat(DWORD dwFormat)
{
	m_dwFormat = dwFormat;
	return S_OK;
}

CString CXText::GetText()
{
	return m_strText;
}

HFONT CXText::GetFont()
{
	return m_hFont;
}

DWORD CXText::GetFormat()
{
	return m_dwFormat;
}

HRESULT CXText::SetColor( COLORREF clr )
{
	m_clr = clr;
	return S_OK;
}

COLORREF CXText::GetColor()
{
	return m_clr;
}

HRESULT CXText::SetSurportAlpha( BOOL bVal )
{
	m_bSuportAlpha = bVal;
	return S_OK;
}

CSize CXText::ComputeSize()
{
	HDC     memdc = CreateCompatibleDC(NULL);

	HGDIOBJ hOldFont = NULL;
	if(m_hFont)
	{
		hOldFont = ::SelectObject(memdc,m_hFont);
	}
	CRect rc(0,0,500,50);
	::DrawText(memdc,m_strText,m_strText.GetLength(),&rc,DT_CALCRECT | DT_SINGLELINE);
	
	if(hOldFont)
	{
		::SelectObject(memdc,hOldFont);
	}
	DeleteDC(memdc);
	return rc.Size();
}

HRESULT CXText::SetBold( BOOL bBold )
{
	
	return S_OK;
}