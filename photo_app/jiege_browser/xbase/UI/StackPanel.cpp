#include "StdAfx.h"
#include "stackpanel.h"

CXStackPanel::CXStackPanel(void)
{

}

CXStackPanel::~CXStackPanel(void)
{

}

HRESULT CXStackPanel::SetStackType( EStackPanelType eType )
{
	if(m_eStackType != eType)
	{
		m_eStackType = eType;
		Relayout();
	}
	return S_OK;
}