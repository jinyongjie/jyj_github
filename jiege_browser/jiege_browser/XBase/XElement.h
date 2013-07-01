#pragma once
#include "xcommon.h"
class CXElement :public IUnknown
{
public:
	CXElement(void);
	~CXElement(void);

	HRESULT STDMETHODCALLTYPE QueryInterface (REFIID riid,void **ppvObject);

	ULONG STDMETHODCALLTYPE AddRef ( );

	ULONG STDMETHODCALLTYPE Release ( );

private:
	ULONG m_uRef;
};
