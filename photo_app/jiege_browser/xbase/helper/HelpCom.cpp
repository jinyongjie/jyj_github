#include "stdafx.h"
#include "HelpCom.h"
#include <map>
#include <atlstr.h>

struct _tagObjectInfo
{
	_tagObjectInfo():func(NULL)
	{

	};
	IID iid;
	CLSID clsid;
	CString strDll;
	ATL::_ATL_CREATORFUNC* func;
};

bool operator<(const IID& left,const IID& right)
{
	return memcmp(&left,&right,sizeof(GUID)) < 0;
}

typedef std::map<IID,_tagObjectInfo> CMapObjectInfo;
CMapObjectInfo* GetObjectFactory()
{
	static CMapObjectInfo* p = NULL;
	if(p == NULL)
	{
		p = new CMapObjectInfo;
	}
	return p;
}
CMapObjectInfo* GetSingleObjectFactory()
{
	static CMapObjectInfo* p = NULL;
	if(p == NULL)
	{
		p = new CMapObjectInfo;
	}
	return p;
}

XCOM_API HRESULT Help::Com::RegisterObject( IID iid,CLSID clsid,LPCWSTR pstrDll )
{
	if(GetObjectFactory()->find(iid) != GetObjectFactory()->end())
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}

	_tagObjectInfo info;
	info.iid = iid;
	info.clsid = clsid;
	info.strDll = pstrDll;
	(*GetObjectFactory())[iid] = info;

	return S_OK;
}

XCOM_API HRESULT Help::Com::RegisterObject( IID iid,ATL::_ATL_CREATORFUNC* func )
{
	if(GetObjectFactory()->find(iid) != GetObjectFactory()->end())
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}

	_tagObjectInfo info;
	info.iid = iid;
	info.func = func;
	(*GetObjectFactory())[iid] = info;
	return S_OK;
}
XCOM_API HRESULT Help::Com::RegisterSingleObject( IID iid,CLSID clsid,LPCWSTR pstrDll )
{
	if(GetSingleObjectFactory()->find(iid) != GetSingleObjectFactory()->end())
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}

	_tagObjectInfo info;
	info.iid = iid;
	info.clsid = clsid;
	info.strDll = pstrDll;
	(*GetSingleObjectFactory())[iid] = info;
	return S_OK;
}

XCOM_API HRESULT Help::Com::RegisterSingleObject( IID iid,ATL::_ATL_CREATORFUNC* func )
{
	if(GetSingleObjectFactory()->find(iid) != GetSingleObjectFactory()->end())
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}

	_tagObjectInfo info;
	info.iid = iid;
	info.func = func;
	(*GetSingleObjectFactory())[iid] = info;
	return S_OK;
}
XCOM_API HRESULT Help::Com::CreateObject( IID iid,void** ppInterface )
{
	if(ppInterface == NULL)
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}
	CMapObjectInfo::iterator it = GetObjectFactory()->find(iid);
	if(it == GetObjectFactory()->end())
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}
	if(it->second.func)
	{
		return it->second.func(NULL,iid,ppInterface);
	}
	else 
	{
		return CreateObjectFromDll(iid,it->second.clsid,ppInterface,it->second.strDll);
	}

	return S_OK;
}

XCOM_API HRESULT Help::Com::GetSingleObject(IID iid,void** ppInterface)
{
	if(ppInterface == NULL)
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}
	static std::map<IID,CComPtr<IUnknown>> mapSingleObject;
	std::map<IID,CComPtr<IUnknown>>::iterator it = mapSingleObject.find(iid);
	if(it != mapSingleObject.end())
	{
		CComPtr<IUnknown> pSingleObject = it->second;
		if(pSingleObject)
		{
			pSingleObject.CopyTo(reinterpret_cast<IUnknown**>(ppInterface));
		}
		else
		{
			ATLASSERT(FALSE);
			return E_FAIL;
		}
	
	}

	CMapObjectInfo::iterator it2 = GetSingleObjectFactory()->find(iid);
	if(it2 == GetSingleObjectFactory()->end())
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}
	HRESULT hr ;
	CComPtr<IUnknown> pUnk;
	if(it2->second.func)
	{
		hr = it2->second.func(NULL,iid,(void**)&pUnk);
	}
	else 
	{
		hr = CreateObjectFromDll(iid,it2->second.clsid,(void**)&pUnk,it2->second.strDll);
	}
	if(SUCCEEDED(hr))
	{
		pUnk.CopyTo(reinterpret_cast<IUnknown**>(ppInterface));
		mapSingleObject[iid] = pUnk;
		return S_OK;
	}
	return E_FAIL;
}

XCOM_API HRESULT Help::Com::CreateObjectFromDll( IID iid,CLSID clsid,void** ppInterface ,LPCWSTR pstrDll)
{
	HMODULE hModule = ::LoadLibrary(pstrDll);
	typedef HRESULT (__stdcall* Type_DllGetClassObject)(REFIID iid,REFCLSID clsid,void** ppFactory);
	Type_DllGetClassObject pDllGetClassObject = (Type_DllGetClassObject)::GetProcAddress(hModule,"DllGetClassObject");
	if(pDllGetClassObject)
	{
		IClassFactory* pFactory = NULL;
		pDllGetClassObject(iid,clsid,(void**)&pFactory);
		if(pFactory)
		{
			HRESULT hr = pFactory->CreateInstance(NULL,iid,ppInterface);
			pFactory->Release();
			return hr;
		}
	}
	return E_FAIL;
}