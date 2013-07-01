#include "StdAfx.h"
#include "progressctrl.h"

CXProgressCtrl::CXProgressCtrl(void)
{
	m_pProgressImage = NULL;
	m_fRate = 0;
	m_pText = NULL;
}

CXProgressCtrl::~CXProgressCtrl(void)
{
	if(m_pProgressImage)
	{
		delete m_pProgressImage;
	}
	if(m_pText)
	{
		delete m_pText;
	}
}

HRESULT CXProgressCtrl::SetProgressImage( CXDraw* pDraw )
{
	if(m_pProgressImage != pDraw)
	{
		m_pProgressImage = pDraw;
	}
	return S_OK;
}

HRESULT CXProgressCtrl::SetRate( float fRate )
{
	if(fRate > 1)
	{
		fRate = 1;
	}
	if(fRate < 0)
	{
		fRate = 0;
	}
	m_fRate = fRate;
	CString str;
	str.Format(L"%d%%",(int)(100*m_fRate));
	if(m_pText)
	{
		m_pText->SetText(str);
	}
	MarkRelayout();

	return S_OK;
}

HRESULT CXProgressCtrl::Paint( HDC hdc ,CRect rcDrawRect)
{
	__super::Paint(hdc,rcDrawRect);
	
	CRect rc=rcDrawRect;
	int nLenProgress = rc.Width()*m_fRate;
	if(m_pProgressImage && nLenProgress)
	{
		m_pProgressImage->Draw(hdc,CRect(rc.left,rc.top,rc.left + nLenProgress,rc.bottom));
	}
	if(m_pText)
	{
		m_pText->Draw(hdc,CRect(rc.left,rc.top - 3,rc.right,rc.bottom));
	}

	return S_OK;
}

HRESULT CXProgressCtrl::SetText( CXText* pText )
{
	m_pText = pText;
	return S_OK;
}