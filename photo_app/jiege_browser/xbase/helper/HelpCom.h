#include <unknwn.h>

#ifdef _USRDLL
#define XCOM_API 	__declspec(dllexport)
#else
#define XCOM_API	
#endif

#define X_REGISTER_OBJECT(class,interface) HRESULT hr##class = Help::Com::RegisterObject(__uuidof(interface),class::_CreatorClass::CreateInstance);

namespace Help
{
	namespace Com
	{
		XCOM_API HRESULT RegisterObject(IID iid,CLSID clsid,LPCWSTR pstrDll);
		XCOM_API HRESULT RegisterObject(IID iid,ATL::_ATL_CREATORFUNC* func);
		XCOM_API HRESULT RegisterSingleObject(IID iid,CLSID clsid,LPCWSTR pstrDll);
		XCOM_API HRESULT RegisterSingleObject(IID iid,ATL::_ATL_CREATORFUNC* func);

		XCOM_API HRESULT CreateObject(IID iid,void** ppInterface);
		XCOM_API HRESULT GetSingleObject(IID iid,void** ppInterface);

		XCOM_API HRESULT CreateObjectFromDll(IID iid,CLSID clsid,void** ppInterface,LPCWSTR pstrDll);
	};
};

