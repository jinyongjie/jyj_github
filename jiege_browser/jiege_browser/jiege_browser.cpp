// jiege_browser.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "jiege_browser.h"
#include "ui/mainwnd.h"
#include "xbase/ShadeWnd.h"
#include "XBase/ResourceMgr.h"
// Global Variables:
CAppModule _Module;

// Forward declarations of functions included in this code module:


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	HRESULT hRes = CoInitialize(NULL);


	//初始化皮肤路径
	WCHAR buffer[MAX_PATH] = {0};
	GetModuleFileName(GetModuleHandle(0), buffer, MAX_PATH);
	PathRemoveFileSpec(buffer);
	CString strSkin;
	strSkin.Format(L"%s\\skin\\default\\",buffer);
	CResourceMgr::SetSkinPath(strSkin);


	//消息循环
	hRes = _Module.Init(NULL, hInstance, &LIBID_ATLLib);
	CMessageLoop theLoop;
	_Module.AddMessageLoop( &theLoop );

	//主窗口
	//CMainWnd wndMain;
	//wndMain.CreateMainWnd();

	//test shadewnd
	CShadeWnd wnd;
	
	CGpImage* pMask = new CGpImage;
	pMask->SetFile(L"shadow.png");
	pMask->SetDrawType(XDRAWTYPE_9PART);
	pMask->SetTileRect(CRect(20,20,850,610));
	wnd.SetMask(pMask);
	wnd.SetMaskMargin(CRect(15,15,15,15));

	wnd.CreateLayerdWnd();
	wnd.ShowWindow(SW_SHOW);
	wnd.CenterWindow();

	CGpImage* pBkg = new CGpImage;
	pBkg->SetFile(L"bar.png");
	pBkg->SetDrawType(XDRAWTYPE_9PART);
	pBkg->SetTileRect(CRect(20,20,50,50));
	wnd.GetRootFrame()->SetBackground(pBkg);



	wnd.SetCalcRgn(FALSE);
	wnd.GetRootFrame()->SetNCHittest(HTCAPTION);


	wnd.UpdateUI();

	//end test

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();

	CoUninitialize();

	return nRet;
}


