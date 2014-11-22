#include "StdAfx.h"
#include "XElement.h"

CXElement::CXElement(void)
{
	m_uRef = 0;
}

CXElement::~CXElement(void)
{
}

HRESULT CXElement::QueryInterface( REFIID riid,void **ppvObject )
{
	ATLASSERT(FALSE);
	return S_OK;
}

ULONG CXElement::AddRef()
{
	return ++m_uRef;
}

ULONG CXElement::Release()
{
	if(m_uRef)
	{
		m_uRef--;
		if(m_uRef == 0)
		{
			delete this;
		}
	}
	return m_uRef;
}