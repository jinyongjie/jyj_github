#pragma once
#include "xstandardwin.h"
class CXDialog:public CXStandardWin
{
public:
	CXDialog(void);
	~CXDialog(void);

	HRESULT SetText(CString strText);
	LRESULT OnCreate( UINT, WPARAM, LPARAM, BOOL& );

	virtual	int DoModal(HWND hwParent, BOOL bCenterToParent = TRUE);
	HRESULT EndDialog(int nRet);
protected:
	int m_nRet;
	bool m_bEndModalLoop;
};
