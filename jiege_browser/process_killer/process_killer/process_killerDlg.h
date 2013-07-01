// process_killerDlg.h : header file
//

#pragma once
#include "afxcmn.h"


// Cprocess_killerDlg dialog
class Cprocess_killerDlg : public CDialog
{
// Construction
public:
	Cprocess_killerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PROCESS_KILLER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
public:
	afx_msg void OnBnClickedOk();

private:
	HRESULT _Refresh();
	HRESULT _KillProcess(DWORD dwPID);
public:
	CListCtrl m_listctrl;
	CString m_strPID;
	afx_msg void OnBnClickedButtonKill();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonKill0();
	afx_msg void OnBnClickedButtonRestart();
	afx_msg void OnBnClickedButton360();
	afx_msg void OnBnClickedButtonJs();
	afx_msg void OnBnClickedButtonRising();
	CString m_strPath;
};
