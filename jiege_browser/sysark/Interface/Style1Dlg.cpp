// Style1Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "Sysark.h"
#include "Style1Dlg.h"
#include "LoadServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStyle1Dlg dialog


CStyle1Dlg::CStyle1Dlg(CWnd* pParent /*=NULL*/, HANDLE hDevice)
	: CDialog(CStyle1Dlg::IDD, pParent)
	, m_hParentWnd(pParent->m_hWnd)
	, m_pDlgInit(NULL)
	, m_CurSel(0)	//��ʾ���ǵ�һ��
	, m_hDevSrv(hDevice)
	, m_pStyle1Dlg(NULL)
	, m_pFileDlg(NULL)
	, m_pRegDlg(NULL)
{
	//{{AFX_DATA_INIT(CStyle1Dlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CStyle1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStyle1Dlg)
	DDX_Control(pDX, IDC_STYLE_1_TAB, m_tab);
	//}}AFX_DATA_MAP
}

void CStyle1Dlg::Clear()
{
	if (NULL != m_pRegDlg)
	{
		delete m_pRegDlg;
		m_pRegDlg = NULL;
	}

	if (NULL != m_pFileDlg)
	{
		delete m_pFileDlg;
		m_pFileDlg = NULL;
	}
	
	if (m_pStyle1Dlg != NULL)
	{
		delete m_pStyle1Dlg;
		m_pStyle1Dlg = NULL;
	}

	if (NULL != m_pDlgInit)
	{
		delete m_pDlgInit;
		m_pDlgInit = NULL;
	}

	
	for (int i = 0; i < m_vChildDlg.size(); i++)
	{
		delete m_vChildDlg.at(i);
	}
	m_vChildDlg.clear();
}


BEGIN_MESSAGE_MAP(CStyle1Dlg, CDialog)
	//{{AFX_MSG_MAP(CStyle1Dlg)
	ON_NOTIFY(TCN_SELCHANGE, IDC_STYLE_1_TAB, OnSelchangeStyle1Tab)
	ON_WM_SIZE()
	ON_COMMAND(IDM_REFLASH, OnReflash)
	ON_COMMAND(IDM_KILL_PROCESS, OnKillProcess)
	ON_COMMAND(IDM_FORCE_KILL_PROCESS, OnForceKillProcess)
	ON_COMMAND(IDM_PROCESS_MODULE, OnProcessModule)
	ON_COMMAND(IDM_PROCESS_THREAD, OnProcessThread)
	ON_COMMAND(IDM_FORCE_KILL_PROCESS_AND_DELETE_FILE, OnForceKillProcessAndDeleteFile)
	ON_WM_PAINT()
	ON_COMMAND(IDM_UNLOAD_DRIVER, OnUnloadDriver)
	ON_COMMAND(IDM_GOTO_FILE, OnGotoFile)
	ON_COMMAND(IDM_EXPORT_TO_FILE, OnExportToFile)
	ON_COMMAND(IDM_FILE_PROPETY, OnFilePropety)
	ON_COMMAND(IDM_DELETE_FILE, OnDeleteFile)
	ON_COMMAND(IDM_COPY_NAME, OnCopyName)
	ON_COMMAND(IDM_COPY_PATH, OnCopyPath)
	ON_COMMAND(IDM_ONLINE_SEARCH_NAME, OnOnlineSearchName)
	ON_COMMAND(IDM_ONLINE_ANALYSIS, OnOnlineAnalysis)
	ON_COMMAND(IDM_UNHOOK_SSDT, OnUnhookSsdt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStyle1Dlg message handlers
void CStyle1Dlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CDialog::OnPaint() for painting messages
}


BOOL CStyle1Dlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN\
		&& (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE))
	{
		return TRUE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}


void CStyle1Dlg::OnSelchangeStyle1Tab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	m_CurSel = m_tab.GetCurSel();
	switch (m_CurSel)
	{
	case 2://�ں�
		{
			if (NULL == m_pStyle1Dlg)
			{
				m_pStyle1Dlg = new CStyle1Dlg(this, m_hDevSrv);
				m_pStyle1Dlg->Create(IDD_STYLE_1_DLG, this);
				CRect rect;
				m_tab.GetClientRect(&rect);
				rect.top += 25;
				m_pStyle1Dlg->MoveWindow(&rect);
				
				LPCTSTR ItemTitle[] = {"SSDT", "ShadowSSDT"};
				m_pStyle1Dlg->InitChildDialog(2, ItemTitle, KernelHookLevel);
			}
			ShowDlg(-1);//�������Ĵ�������
			m_pStyle1Dlg->BringWindowToTop();
			m_pStyle1Dlg->ShowWindow(SW_SHOW);
		}
		break;
	case 3://ע���
		{
			if (NULL == m_pRegDlg)
			{
				m_pRegDlg = new CFileDlg(m_hDevSrv, this);
				m_pRegDlg->Create(IDD_FILE_DLG, this);
				m_pRegDlg->InitListColumn("ע���");
				m_pRegDlg->InitTreeView();
				CRect rect;
				m_tab.GetClientRect(&rect);
				rect.top += 25;
				m_pRegDlg->MoveWindow(&rect);
			}
			ShowDlg(-1);
			m_pRegDlg->BringWindowToTop();
			m_pRegDlg->ShowWindow(SW_SHOW);

		}
		break;
	case 4://�ļ�
		{
			if (NULL == m_pFileDlg)
			{
				m_pFileDlg = new CFileDlg(m_hDevSrv, this);
				m_pFileDlg->Create(IDD_FILE_DLG, this);
				m_pFileDlg->InitListColumn("�ļ�");
				m_pFileDlg->InitTreeView();
				CRect rect;
				m_tab.GetClientRect(&rect);
				rect.top += 25;
				m_pFileDlg->MoveWindow(&rect);
			}
			ShowDlg(-1);
			m_pFileDlg->BringWindowToTop();
			m_pFileDlg->ShowWindow(SW_SHOW);
		}
		break;
	default:
		{
			ShowDlg(m_CurSel);
		}
		break;
	}

	UpdateWindow();
	
	*pResult = 0;
}

BOOL CStyle1Dlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//��ʾĳ��DLG��ֻ������ͨ��DLG����CCommondlg
VOID CStyle1Dlg::ShowDlg(int nDlg)
{
	//��ȫ������
	for (int i = 0; i < m_vChildDlg.size(); i++)
	{
		m_vChildDlg.at(i)->ShowWindow(SW_HIDE);
	}

	if (m_pStyle1Dlg != NULL)
	{
		m_pStyle1Dlg->ShowWindow(SW_HIDE);
	}
	if (m_pRegDlg != NULL)
	{
		m_pRegDlg->ShowWindow(SW_HIDE);
	}
	if (m_pFileDlg != NULL)
	{
		m_pFileDlg->ShowWindow(SW_HIDE);
	}
	
	//������ڶ�������������Ƕ������ⵥ����DLG����ʹ�ù���CCommongDlg
	if ( nDlg < 0 || m_vChildDlg.size() <= nDlg ||\
		nDlg == 2 || nDlg == 3 | nDlg == 4)
	{
		return;
	}

	CRect rect;
	m_tab.GetClientRect(&rect);
	rect.top += 25;
	CCommonDlg* pCommonDlg = m_vChildDlg.at(nDlg);
	m_pDlgInit->InitDialog(nDlg, pCommonDlg);
	pCommonDlg->MoveWindow(&rect);
    pCommonDlg->BringWindowToTop();//��������Ҫ
    pCommonDlg->ShowWindow(SW_SHOWNORMAL);
    pCommonDlg->UpdateWindow();
	
	return;
}

BOOL CStyle1Dlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	Clear();
	
	return CDialog::DestroyWindow();
}

void CStyle1Dlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (::IsWindow(m_tab.m_hWnd))
	{
		RECT rect;
		GetClientRect(&rect);
		m_tab.MoveWindow(&rect);
		m_tab.GetClientRect(&rect);
		rect.top += 25;
		
		if (m_CurSel >= 0 && m_CurSel < m_vChildDlg.size() && m_CurSel != 2 && m_CurSel !=3 && m_CurSel != 4)
		{
			CCommonDlg* pCommonDlg = m_vChildDlg.at(m_CurSel);
			pCommonDlg->MoveWindow(&rect);
			pCommonDlg->ShowWindow(SW_SHOW);
			pCommonDlg->UpdateWindow();
			//m_pDlgInit->InitDialog(m_CurSel, pCommonDlg);
		}
		else
		{
			//�ں˹���
			if (m_CurSel == 2 && m_pStyle1Dlg != NULL && \
				::IsWindow(m_pStyle1Dlg->m_hWnd))
			{
				m_pStyle1Dlg->MoveWindow(&rect);
				m_pStyle1Dlg->ShowWindow(SW_SHOW);
				m_pStyle1Dlg->UpdateWindow();
			}
			//ע���
			if (m_CurSel == 3 && m_pRegDlg != NULL && \
				::IsWindow(m_pRegDlg->m_hWnd))
			{
				m_pRegDlg->MoveWindow(&rect);
				m_pRegDlg->ShowWindow(SW_SHOW);
				m_pRegDlg->UpdateWindow();
			}
			//�ļ�
			if (m_CurSel == 4 && m_pFileDlg != NULL && \
				::IsWindow(m_pFileDlg->m_hWnd))
			{
				m_pFileDlg->MoveWindow(&rect);
				m_pFileDlg->ShowWindow(SW_SHOW);
				m_pFileDlg->UpdateWindow();
			}
		}
	}
	//UpdateWindow();
}

//���������ӿؼ�
BOOL CStyle1Dlg::InitChildDialog(int nDlg, LPCTSTR* pItemArray, DlgLevel dlgLevel)
{
	BOOL bResult = TRUE;
	CCommonDlg* pCommonDlg = NULL;
	
	for (int i = 0; i < nDlg; i++)
	{
		m_tab.InsertItem(i, pItemArray[i]);
		pCommonDlg = NULL;
		switch (dlgLevel)
		{
		case FirstLevel:
			{
				switch (i)
				{
				case 0: //����
					pCommonDlg = new CCommonDlg(this, IDR_MENU_PROCESS);
					break;
				case 1: //����ģ��
					pCommonDlg = new CCommonDlg(this, IDR_MENU_DRIVER_MODULE);
					break;
				case 2: //δ��
					pCommonDlg = new CCommonDlg(this);
					break;
				case 3: //δ��
					pCommonDlg = new CCommonDlg(this);
					break;
				case 4: //δ��
					pCommonDlg = new CCommonDlg(this);
					break;
				case 5: //δ��
					pCommonDlg = new CCommonDlg(this);
					break;
				case 6: //������
					pCommonDlg = new CCommonDlg(this);
					break;
				case 7: //����
					pCommonDlg = new CCommonDlg(this);
					break;
				default:
					break;
				}
			}
			break;
		case KernelHookLevel:
			{
				pCommonDlg = new CCommonDlg(this, IDR_MENU_SSDT);
			}
			break;
		default:
			break;
		}
		pCommonDlg->Create(IDD_COMMON_DLG, this);
		pCommonDlg->InitListColumn(pItemArray[i]);
		m_vChildDlg.push_back(pCommonDlg);
	}
	
	
	m_pDlgInit = new CDlgInit(m_hDevSrv, m_hParentWnd, m_vChildDlg, dlgLevel);
	
	ShowDlg(0);
	
	return bResult;
}

//�Ҽ��˵�����

//ˢ��
VOID CStyle1Dlg::OnReflash()
{
	// TODO: Add your command handler code here
	int CurSel = m_tab.GetCurSel();
	ShowDlg(CurSel);
}

//����������ļ�
void CStyle1Dlg::OnExportToFile() 
{
	// TODO: Add your command handler code here
	
}

//���ļ�����
void CStyle1Dlg::OnFilePropety() 
{
	// TODO: Add your command handler code here
	SHELLEXECUTEINFO sei;
	CHAR szPathOfFileToLaunchPropertiesOf[MAX_PATH] = {0};
	
	m_vChildDlg.at(m_CurSel)->GetSelectItemFilePath(szPathOfFileToLaunchPropertiesOf, MAX_PATH);
	
	if (strlen(szPathOfFileToLaunchPropertiesOf) > 0)
	{
		ZeroMemory(&sei,sizeof(sei));
		sei.cbSize = sizeof(sei);
		sei.lpFile = szPathOfFileToLaunchPropertiesOf;
		sei.lpVerb = "properties";
		sei.fMask  = SEE_MASK_INVOKEIDLIST;
		ShellExecuteEx(&sei); 
	}
	
}

//����**���ƣ��������ơ��������Ƶȣ���������
void CStyle1Dlg::OnCopyName() 
{
	// TODO: Add your command handler code here
	HGLOBAL hClip; 
	CHAR szName[MAX_PATH] = {0};
	m_vChildDlg.at(m_CurSel)->GetSelectItemValue(szName, MAX_PATH, "����");
	//����һ��HGLOBAL�����������ָ�������ڴ��
	if (OpenClipboard())
	{
		EmptyClipboard();                            //���������������
		hClip=GlobalAlloc(GMEM_MOVEABLE, strlen(szName) + 1); 
		//�ڶ��Ϸ�����ƶ����ڴ�飬���򷵻�һ���ڴ���
		char * buff;                                 //����ָ���ַ��͵�ָ�����
		buff=(char*)GlobalLock(hClip);
		//�Է�����ڴ����м��������ڴ����ת����һ��ָ��,������Ӧ�����ü�������1
		strcpy(buff, szName);
		//���û���������ݸ��Ƶ�ָ������У�ʵ���Ͼ��Ǹ��Ƶ�������ڴ����
		GlobalUnlock(hClip);
		//����д����ϣ����н����������������ü��������ּ�1
		SetClipboardData(CF_TEXT,hClip);
		//����������ݵ��ڴ�������������Դ������
		CloseClipboard();
		//�رռ����壬�ͷż�������Դ��ռ��Ȩ
   }
}

//����**·��������·��������·���ȣ���������
void CStyle1Dlg::OnCopyPath() 
{
	// TODO: Add your command handler code here
	
	HGLOBAL hClip; 
	CHAR szName[MAX_PATH] = {0};
	m_vChildDlg.at(m_CurSel)->GetSelectItemFilePath(szName, MAX_PATH);
	//����һ��HGLOBAL�����������ָ�������ڴ��
	if (OpenClipboard())
	{
		EmptyClipboard();                            //���������������
		hClip=GlobalAlloc(GMEM_MOVEABLE, strlen(szName) + 1); 
		//�ڶ��Ϸ�����ƶ����ڴ�飬���򷵻�һ���ڴ���
		char * buff;                                 //����ָ���ַ��͵�ָ�����
		buff=(char*)GlobalLock(hClip);
		//�Է�����ڴ����м��������ڴ����ת����һ��ָ��,������Ӧ�����ü�������1
		strcpy(buff, szName);
		//���û���������ݸ��Ƶ�ָ������У�ʵ���Ͼ��Ǹ��Ƶ�������ڴ����
		GlobalUnlock(hClip);
		//����д����ϣ����н����������������ü��������ּ�1
		SetClipboardData(CF_TEXT,hClip);
		//����������ݵ��ڴ�������������Դ������
		CloseClipboard();
		//�رռ����壬�ͷż�������Դ��ռ��Ȩ
   }
}


//�����������ƣ���������ơ��������Ƶȣ�
void CStyle1Dlg::OnOnlineSearchName() 
{
	// TODO: Add your command handler code here
	CHAR szName[MAX_PATH] = {0};
	char pUrl[300] = "http://www.google.com.hk/search?hl=en&newwindow=1&output=search&q=";
	m_vChildDlg.at(m_CurSel)->GetSelectItemValue(szName, MAX_PATH, "����");
	strcat(pUrl, szName);

	ShellExecute(NULL, _T("open"), pUrl, NULL,NULL, SW_SHOW);
	
}
//���߷���,����ҳhttps://fireeye.ijinshan.com/
void CStyle1Dlg::OnOnlineAnalysis() 
{
	// TODO: Add your command handler code here
	ShellExecute(NULL, _T("open"), _T("https://fireeye.ijinshan.com/"), NULL,NULL, SW_SHOW);
	
}



//**********************************�����Ҽ��˵�����***************************************/
void CStyle1Dlg::OnKillProcess() 
{
	// TODO: Add your command handler code here
	
}
//ǿ����ֹ����
void CStyle1Dlg::OnForceKillProcess() 
{
	// TODO: Add your command handler code here
	 	ASSERT(m_CurSel >= 0);
	 	DWORD dwRet;
	 	DWORD dwInput = (DWORD)m_vChildDlg.at(m_CurSel)->GetSelectItemPid();
	 	
	 	BOOL bRet = DeviceIoControl(m_hDevSrv, IOCTL_KILL_PROCESS, &dwInput, sizeof(DWORD),\
	 		NULL, NULL, &dwRet, NULL);
	 	OnReflash();
}

//ǿ����ֹ���̺�ɾ���ļ�
void CStyle1Dlg::OnForceKillProcessAndDeleteFile() 
{
	// TODO: Add your command handler code here
	 	ASSERT(m_CurSel >= 0);
	 	DWORD dwRet;
	 	CHAR lpFilePath[MAX_PATH] = {0};
	 	DWORD dwInput = (DWORD)m_vChildDlg.at(m_CurSel)->GetSelectItemPid();
	 	
	 	BOOL bRet = DeviceIoControl(m_hDevSrv, IOCTL_KILL_PROCESS, &dwInput, sizeof(DWORD),\
	 		NULL, NULL, &dwRet, NULL);
	 	if (bRet)
	 	{
	 
	 		m_vChildDlg.at(m_CurSel)->GetSelectItemFilePath(lpFilePath, MAX_PATH);
	 		bRet = DeviceIoControl(m_hDevSrv, IOCTL_DELETE_FILE, lpFilePath, strlen(lpFilePath),\
	 		NULL, NULL, &dwRet, NULL);
	 	}
	 	OnReflash();
	
}

//�򿪽��̵�ģ�鴰��
void CStyle1Dlg::OnProcessModule() 
{
	// TODO: Add your command handler code here
	 	ASSERT(m_CurSel >= 0);
	 	ALL_PROCESS_MODULE_INFO AllProcessModuleInf;
	 	DWORD dwRet;
	 	DWORD dwInput = (DWORD)m_vChildDlg.at(m_CurSel)->GetSelectItemPid();
	 	
	 	BOOL bRet = DeviceIoControl(m_hDevSrv, IOCTL_ENUM_PROCESS_MODULE, &dwInput, sizeof(DWORD),\
	 		&AllProcessModuleInf, sizeof(ALL_PROCESS_MODULE_INFO), &dwRet, NULL);
	 	if (bRet)
	 	{
	 		CFirstLittleDlg fld;
	 		fld.SetDlgInitData(11, PROCESS_MODULE_DLG, (PVOID)&AllProcessModuleInf);
	 		fld.DoModal();
	 	}
	
}

//�򿪽��̵��̴߳���
void CStyle1Dlg::OnProcessThread() 
{
	// TODO: Add your command handler code here
	 	ASSERT(m_CurSel >= 0);
	 	ALL_PROCESS_THREAD_INFO AllProcessThreadInf;
	 	DWORD dwRet;
		DWORD dwInput = (DWORD)m_vChildDlg.at(m_CurSel)->GetSelectItemPid();
	 	
	 	BOOL bRet = DeviceIoControl(m_hDevSrv, IOCTL_ENUM_PROCESS_THREAD, &dwInput, sizeof(DWORD),\
	 		&AllProcessThreadInf, sizeof(ALL_PROCESS_THREAD_INFO), &dwRet, NULL);
	 	if (bRet)
	 	{
	 		CFirstLittleDlg fld;
	 		fld.SetDlgInitData(12, PROCESS_THREAD_DLG, (PVOID)&AllProcessThreadInf);
	 		fld.DoModal();
	 	}
	
}


//**********************************����ģ���Ҽ��˵�����***************************************/
//ж������
void CStyle1Dlg::OnUnloadDriver() 
{
	// TODO: Add your command handler code here
	BOOL bRet = FALSE;
	CHAR szSrvName[MAX_PATH] = {0};
	m_vChildDlg.at(m_CurSel)->GetSelectItemValue(szSrvName, MAX_PATH, "����");
	bRet = UnloadNTDriver( szSrvName );
	
}
//��λ���ļ�����Ŀ¼��ͨ���ļ�����ѡ���ļ�
void CStyle1Dlg::OnGotoFile() 
{
	// TODO: Add your command handler code here
	CHAR szPathOfFile[MAX_PATH] = {0};
	BOOL bRet = FALSE;
	DWORD dwRet;
	m_vChildDlg.at(m_CurSel)->GetSelectItemFilePath(szPathOfFile, MAX_PATH);
	char szParam[_MAX_PATH+64]={0};
	strcpy(szParam, "/e,/select, ");
	strcat(szParam, szPathOfFile);
	ShellExecute(NULL,"open","explorer",szParam,NULL,SW_SHOW);
	
}

//ɾ���ļ�
void CStyle1Dlg::OnDeleteFile() 
{
	// TODO: Add your command handler code here
	CHAR szPathOfFile[MAX_PATH] = {0};
	BOOL bRet = FALSE;
	DWORD dwRet;
	m_vChildDlg.at(m_CurSel)->GetSelectItemFilePath(szPathOfFile, MAX_PATH);
	bRet = DeviceIoControl(m_hDevSrv, IOCTL_DELETE_FILE, szPathOfFile, strlen(szPathOfFile),\
	 		NULL, NULL, &dwRet, NULL);
	
}


//**********************************�������Ҽ��˵�����***************************************/

//**********************************�����Ҽ��˵�����***************************************/

//**********************************SSDT�Ҽ��˵�����***************************************/

void CStyle1Dlg::OnUnhookSsdt() 
{
	// TODO: Add your command handler code here
	CHAR szOriAddr[MAX_PATH] = {0};
	CHAR szSerialNum[MAX_PATH] = {0};
	ULONG vInput[2];
	BOOL bRet = FALSE;
	DWORD dwRet;
	m_vChildDlg.at(m_CurSel)->GetSelectItemValue(szSerialNum, MAX_PATH, "���");
	m_vChildDlg.at(m_CurSel)->GetSelectItemValue(szOriAddr, MAX_PATH, "ԭʼ������ַ");

	vInput[0] = strtol(szSerialNum, NULL, 16);
	sscanf(szOriAddr, "%x", &vInput[1]);

	
	bRet = DeviceIoControl(m_hDevSrv, IOCTL_SET_SSDT, vInput, 2* sizeof(int),\
	 		NULL, NULL, &dwRet, NULL);
}
