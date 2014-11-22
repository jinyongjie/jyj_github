#pragma once
#include "../XBase/childwnd.h"
class CBrowserCore :public CXChildWnd
{
public:
	CBrowserCore(void);
	~CBrowserCore(void);

	
public:
	virtual	HRESULT Navigate(CString strUrl);
private:
};
