#pragma once
#include "XFrame.h"
#include "xcommon.h"
class CXStackPanel :public	CXFrame
{
public:
	CXStackPanel(void);
	~CXStackPanel(void);


	HRESULT SetStackType(EStackPanelType eType);
private:
	EStackPanelType	m_eStackType;//ºáÏòor×ÝÏò
};
