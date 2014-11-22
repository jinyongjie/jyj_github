#include "StdAfx.h"
#include "xstatic.h"

CXStatic::CXStatic(void)
{
	m_pText = NULL;
}

CXStatic::~CXStatic(void)
{

}



HRESULT CXStatic::Paint( HDC hdc,CRect rcRect )
{
	__super::Paint(hdc,rcRect);

	if(m_pText)
	{
		m_pText->Draw(hdc,rcRect);
	}

	return S_OK;
}

HRESULT CXStatic::SetText( CString str )
{
	if(m_pText == NULL)
	{
		m_pText = new CXText;
		m_pText->SetFormat(DT_WORDBREAK);
	}
	m_pText->SetText(str);
	InvalidateFrame();
	return S_OK;
}

HRESULT CXStatic::SetFont( HFONT hFont )
{
	if(m_pText)
	{
		m_pText->SetFont(hFont);
	}
	return S_OK;
}

HRESULT CXStatic::SetNormalColor( COLORREF clr )
{
	if(m_pText)
	{
		m_pText->SetColor(clr);
	}
	return S_OK;
}