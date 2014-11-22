#pragma once
#include "xcommon.h"
class CXDraw
{
public:
	CXDraw(void);
	~CXDraw(void);

public:
	virtual	HRESULT Draw(HDC hdc,CRect rcPaint) = 0;
	virtual	CSize GetSize();
};
