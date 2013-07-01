#if !defined(AFX_COMMONDLG_H__2F5BAF64_7059_4791_B5BC_B7BCBB9195BD__INCLUDED_)
#define AFX_COMMONDLG_H__2F5BAF64_7059_4791_B5BC_B7BCBB9195BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CommonDlg.h : header file
//
#include "Import/tinystr.h"
#include "Import/tinyxml.h"
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CCommonDlg dialog

class CCommonDlg : public CDialog
{
// Construction
public:
	CCommonDlg(CWnd* pParent = NULL);   // standard constructor
	~CCommonDlg()
	{
		if (NULL != m_pListCtrl)
		{
			delete m_pListCtrl;
			m_pListCtrl = NULL;
		}
		
		if (NULL != m_pDoc)
		{
			delete m_pDoc;
			m_pDoc = NULL;
		}
	}

// Dialog Data
	//{{AFX_DATA(CCommonDlg)
	enum { IDD = IDD_COMMON_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCommonDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCommonDlg)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	VOID InitListColumn(int nDlg);
	VOID SetListItemText(int nItem, int nSubItem, LPCTSTR lpszText);
	VOID DeleteListAllItems();
	VOID UpdateStatusBar(int nDlg, HWND hWnd);
private:
	void InitListCtrl();
	void GBKToUTF8(char* &szOut);
	void UTF8ToGBK( char *&szOut );

private:
	CListCtrl* m_pListCtrl;
	TiXmlDocument *m_pDoc;	// ����һ��TiXmlDocument��ָ��
	vector<FLOAT> m_vColumnsWidth;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMMONDLG_H__2F5BAF64_7059_4791_B5BC_B7BCBB9195BD__INCLUDED_)
