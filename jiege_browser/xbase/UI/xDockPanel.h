#pragma once
#include "xframe.h"



class CXDockPanel :public CXFrame
{
public:
	CXDockPanel(void);
	~CXDockPanel(void);

	HRESULT Relayout();


};
