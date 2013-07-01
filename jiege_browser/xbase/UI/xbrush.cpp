#include "StdAfx.h"
#include "xbrush.h"

CXBrush::CXBrush(void)
{
	m_hBrush = NULL;
	m_clr = RGB(107,199,137);
}

CXBrush::~CXBrush(void)
{
	if(m_hBrush)
	{
		DeleteObject(m_hBrush);
	}
}

HRESULT CXBrush::SetColor( COLORREF clr )
{
	m_clr = clr;
	return S_OK;
}

HRESULT CXBrush::Draw( HDC hdc,CRect rcPaint )
{
	if(m_hBrush == NULL)
	{
		m_hBrush = CreateSolidBrush(m_clr);
	}
	::FillRect(hdc,&rcPaint,m_hBrush);
	return S_OK;
}