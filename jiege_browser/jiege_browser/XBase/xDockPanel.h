#pragma once
#include "../XBase/xframe.h"



class CXDockPanel :public CXFrame
{
public:
	CXDockPanel(void);
	~CXDockPanel(void);

	HRESULT Relayout();


};
