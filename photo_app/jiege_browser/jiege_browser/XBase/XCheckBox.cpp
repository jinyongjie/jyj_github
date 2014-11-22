#include "StdAfx.h"
#include "XCheckBox.h"

CXCheckBox::CXCheckBox(void):m_oEventListener(this)
{
	m_bCheck = FALSE;
	m_bRadio = FALSE;

	m_pStatic = NULL;
	m_pBtn = NULL;

	 m_pCheckNormal = NULL;
	 m_pCheckOn= NULL;
	 m_pCheckDown= NULL;
	 m_pUnCheckNormal= NULL;
	 m_pUnCheckOn= NULL;
	 m_pUnCheckDown= NULL;
	 m_oEventListener.Hook_OnXEvent(&CXCheckBox::OnXEvent);
}

CXCheckBox::~CXCheckBox(void)
{
}

HRESULT CXCheckBox::SetCheck( BOOL bCheck )
{
	if(m_bCheck != bCheck)
	{
		m_bCheck = bCheck;
		if(m_bCheck)
		{
			m_pBtn->SetBkgNormal(m_pCheckNormal);
			m_pBtn->SetBkgOn(m_pCheckOn);
			m_pBtn->SetBkgDown(m_pCheckDown);
		}
		else
		{
			m_pBtn->SetBkgNormal(m_pUnCheckNormal);
			m_pBtn->SetBkgOn(m_pUnCheckOn);
			m_pBtn->SetBkgDown(m_pUnCheckDown);
		}
		InvalidateFrame();
	}

	return S_OK;
}

BOOL CXCheckBox::GetCheck()
{
	return m_bCheck;
}

HRESULT CXCheckBox::SetRadio( BOOL bRadio )
{
	m_bRadio = bRadio;
	return S_OK;
}

BOOL CXCheckBox::GetRadio()
{
	return m_bRadio;
}

HRESULT CXCheckBox::SetText( CString strText )
{
	if(m_pStatic)
	{
		m_pStatic->SetText(strText);
	}
	return S_OK;
}

HRESULT CXCheckBox::Create( CXFrame* pParent )
{
	__super::Create(pParent);
	m_pBtn = new CXButton;
	m_pBtn->Create(this);
	m_pBtn->SetDockType(XDOCK_LEFT);
	m_pBtn->SetRect(CRect(0,0,13,14));
	m_pBtn->SetAutoHeight(TRUE);
	m_pBtn->SetMargin(CRect(0,1,0,0));
	ADD_EVENT_OBSERVER(m_pBtn,EVENT_BUTTON_LBUTTON_CLICK,&m_oEventListener);

	m_pStatic = new CXStatic;
	m_pStatic->Create(this);
	m_pStatic->SetDockType(XDOCK_FILL);
	m_pStatic->SetMargin(CRect(5,0,0,0));

	return S_OK;
}

HRESULT CXCheckBox::SetCheckImage( CXDraw* pNormal,CXDraw* pOn,CXDraw* pDown )
{
	if(m_pBtn)
	{
		m_pCheckNormal = pNormal;
		m_pCheckOn = pOn;
		m_pCheckDown = pDown;
		if(m_bCheck)
		{
			m_pBtn->SetBkgNormal(pNormal);
			m_pBtn->SetBkgOn(pOn);
			m_pBtn->SetBkgDown(pDown);
		}
	}
	return S_OK;
}

HRESULT CXCheckBox::SetUnCheckImage( CXDraw* pNormal,CXDraw* pOn,CXDraw* pDown )
{
	if(m_pBtn)
	{
		m_pUnCheckNormal = pNormal;
		m_pUnCheckOn = pOn;
		m_pUnCheckDown = pDown;
		if(!m_bCheck)
		{
			m_pBtn->SetBkgNormal(pNormal);
			m_pBtn->SetBkgOn(pOn);
			m_pBtn->SetBkgDown(pDown);
		}
	}
	return S_OK;
}

HRESULT CXCheckBox::OnXEvent( INT nId, CXFrame* pEventSource, void* pArg )
{
	BEGIN_EVENT_MAP(pEventSource,nId,pArg)
		ON_EVENT_BY_ID(m_pBtn,EVENT_BUTTON_LBUTTON_CLICK,OnBtnClick)
	END_EVENT_MAP()
	return S_OK;
}

HRESULT CXCheckBox::OnBtnClick( void* pArg )
{
	if(m_bRadio)
	{
		SetCheck(!m_bCheck);
		ThrowEvent(EVENT_CHECKBOX_CLICK,NULL);
	}
	else
	{
		if(!m_bCheck)
		{
			SetCheck(TRUE);
			ThrowEvent(EVENT_CHECKBOX_CLICK,NULL);
		}
	}
	
	return S_OK;
}