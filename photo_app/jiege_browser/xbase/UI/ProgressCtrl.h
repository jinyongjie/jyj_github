#pragma once
#include "xframe.h"
#include "xtext.h"
class CXProgressCtrl :public CXFrame
{
public:
	CXProgressCtrl(void);
	~CXProgressCtrl(void);

	HRESULT SetProgressImage(CXDraw* pDraw);
	HRESULT SetRate(float fRate);

	HRESULT Paint(HDC hdc,CRect rcDrawRect);
	HRESULT SetText(CXText* pText);
private:
	CXDraw* m_pProgressImage;
	float m_fRate;
	CXText* m_pText;
};
