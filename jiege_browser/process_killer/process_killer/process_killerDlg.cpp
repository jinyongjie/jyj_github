// process_killerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "process_killer.h"
#include "process_killerDlg.h"
#include "winioctl.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "winsvc.h"
#include "tlhelp32.h"

// Cprocess_killerDlg dialog



//加载驱动

BOOL use_driver(LPCWSTR str_driverpath,LPCWSTR str_drivername,LPCWSTR str_servicename)
{
	SC_HANDLE myhandle;
	SC_HANDLE mycreate;
	SC_HANDLE hService;
	SERVICE_STATUS ss;
	//打开服务控制管理器
	myhandle=OpenSCManager(NULL,
		NULL,
		SC_MANAGER_ALL_ACCESS);
	if(myhandle==NULL)
		return FALSE;
	//创建service
	mycreate=CreateService(myhandle,
		str_servicename,
		str_drivername,
		SERVICE_ALL_ACCESS,
		SERVICE_KERNEL_DRIVER,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_IGNORE,
		str_driverpath,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL);
	if(mycreate==NULL)
		return FALSE;
	//打开service
	hService = OpenService(myhandle, str_servicename,
		SERVICE_START | DELETE | SERVICE_STOP);
	if(hService==NULL)
		return FALSE;
	BOOL ret;
	//开启service
	ret=StartService(hService,NULL, NULL);
	if(ret==FALSE)
		return FALSE ;
	CloseServiceHandle(hService);
	CloseServiceHandle(mycreate);
	CloseServiceHandle(myhandle);
	return TRUE ;

}



//卸载驱动

BOOL unload_driver(LPCWSTR str_drivername,LPCWSTR str_servicename)
{
	SC_HANDLE myhandle;
	SC_HANDLE hService;
	SERVICE_STATUS ss;

	//打开service control manager 
	myhandle=OpenSCManager(NULL,
		NULL,
		SC_MANAGER_ALL_ACCESS);
	if(myhandle==NULL)
		return FALSE;
	//打开service
	hService = OpenService(myhandle, str_servicename,
		SERVICE_START | DELETE | SERVICE_STOP);
	if(hService==NULL)
		return FALSE;
	BOOL ret;
	//停止驱动
	ret=ControlService(hService, SERVICE_CONTROL_STOP, &ss);
	//if(ret==FALSE)
	//	return FALSE;
	//删除服务
	DeleteService(hService);
	CloseServiceHandle(hService);
	CloseServiceHandle(myhandle);
	return TRUE ;
}


Cprocess_killerDlg::Cprocess_killerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Cprocess_killerDlg::IDD, pParent)
	, m_strPID(_T(""))
	, m_strPath(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cprocess_killerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listctrl);
	DDX_Text(pDX, IDC_EDIT_ID, m_strPID);
	DDX_Text(pDX, IDC_EDIT_PATH, m_strPath);
}

BEGIN_MESSAGE_MAP(Cprocess_killerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &Cprocess_killerDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_KILL, &Cprocess_killerDlg::OnBnClickedButtonKill)
	ON_BN_CLICKED(IDC_BUTTON_START, &Cprocess_killerDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &Cprocess_killerDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_KILL0, &Cprocess_killerDlg::OnBnClickedButtonKill0)
	ON_BN_CLICKED(IDC_BUTTON_RESTART, &Cprocess_killerDlg::OnBnClickedButtonRestart)
	ON_BN_CLICKED(IDC_BUTTON_360, &Cprocess_killerDlg::OnBnClickedButton360)
	ON_BN_CLICKED(IDC_BUTTON_JS, &Cprocess_killerDlg::OnBnClickedButtonJs)
	ON_BN_CLICKED(IDC_BUTTON_RISING, &Cprocess_killerDlg::OnBnClickedButtonRising)
END_MESSAGE_MAP()


// Cprocess_killerDlg message handlers

BOOL Cprocess_killerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	_Refresh();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void Cprocess_killerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR Cprocess_killerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void Cprocess_killerDlg::OnBnClickedOk()
{
	_Refresh();
}


HRESULT Cprocess_killerDlg::_Refresh()
{
	m_listctrl.RemoveAllGroups();
	m_listctrl.InsertColumn(0,L"id",0,100);
	m_listctrl.InsertColumn(2,L"name",0,200);

	CString   str,a,prcnum; 
	HANDLE   SnapShot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0); 
	SHFILEINFO   shSmall; 
	PROCESSENTRY32   ProcessInfo; 
	ProcessInfo.dwSize=sizeof(ProcessInfo);//

	BOOL   Status=Process32First(SnapShot,&ProcessInfo); 

	while(Status) 
	{
		CString str;
		str.Format(L"%d",ProcessInfo.th32ProcessID);
		Status=Process32Next(SnapShot,&ProcessInfo); 
		int nItem = m_listctrl.InsertItem(0,str);
		m_listctrl.SetItemText(nItem,1,ProcessInfo.szExeFile);
	} 
	
	return S_OK;
}

HRESULT Cprocess_killerDlg::_KillProcess( DWORD dwPID )
{
	// TODO: Add your control notification handler code here
	HANDLE hDevice = CreateFile(L"\\\\.\\process_killer_sl",
		GENERIC_READ|GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_SYSTEM,
		0);

	if(hDevice == NULL || hDevice == (HANDLE)INVALID_FILE_ATTRIBUTES)
	{
		OutputDebugString(L"Open Driver fail");
		return E_FAIL;
	}

	int n = dwPID;
	DWORD dwWritten = 0;
	WriteFile(hDevice,&n,sizeof(int),&dwWritten,NULL);

	CloseHandle(hDevice);
	
	return S_OK;
}

void Cprocess_killerDlg::OnBnClickedButtonKill()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	_KillProcess(_ttoi(m_strPID));

}
void Cprocess_killerDlg::OnBnClickedButtonKill0()
{
	UpdateData();
	HANDLE h = OpenProcess(PROCESS_ALL_ACCESS,NULL,_ttoi(m_strPID));
	if(h)
	{
		BOOL bRet = TerminateProcess(h,0);
		CString str;
		str.Format(L"TerminateProcess ret=%d err=%x",bRet,GetLastError());
		OutputDebugString(str);
	}
	else
	{
		OutputDebugString(L"OpenProcessFail");
	}

	// TODO: Add your control notification handler code here
}
void Cprocess_killerDlg::OnBnClickedButtonStart()
{
	UpdateData();
	CString strPath = L"C:\\code\\jiegebrowser\\process_killer\\killer_driver\\objchk_wnet_x86\\i386\\process_killer.sys";
	if(m_strPath.GetLength() > 0)
	{
		strPath = m_strPath;
	}
	BOOL bLoad =use_driver(strPath,L"process_killer",L"process_killer");
	CString str;
	str.Format(L"load=%d",bLoad);
	OutputDebugString(str);

	// TODO: Add your control notification handler code here
}

void Cprocess_killerDlg::OnBnClickedButtonStop()
{
		BOOL bUnload = unload_driver(L"process_killer",L"process_killer");

		CString str;
		str.Format(L"unload=%d ",bUnload);
		OutputDebugString(str);
	// TODO: Add your control notification handler code here
}



void Cprocess_killerDlg::OnBnClickedButtonRestart()
{
	OnBnClickedButtonStop();
	OnBnClickedButtonStart();
	// TODO: Add your control notification handler code here
}

void Cprocess_killerDlg::OnBnClickedButton360()
{
	// TODO: Add your control notification handler code here
	HANDLE hDevice = CreateFile(L"\\\\.\\process_killer_sl",
		GENERIC_READ|GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_SYSTEM,
		0);

	if(hDevice == NULL || hDevice == (HANDLE)INVALID_FILE_ATTRIBUTES)
	{
		OutputDebugString(L"Open Driver fail");
		return ;
	}

	DWORD dwRet = 0;
#define CTL_CODE_360	CTL_CODE(FILE_DEVICE_UNKNOWN, 0, METHOD_IN_DIRECT, FILE_ANY_ACCESS)

	BOOL bRet = DeviceIoControl(hDevice, CTL_CODE_360, NULL, NULL,\
		NULL, 0, &dwRet, NULL);
	CloseHandle(hDevice);
}

void Cprocess_killerDlg::OnBnClickedButtonJs()
{
	// TODO: Add your control notification handler code here
}

void Cprocess_killerDlg::OnBnClickedButtonRising()
{
	// TODO: Add your control notification handler code here
}
