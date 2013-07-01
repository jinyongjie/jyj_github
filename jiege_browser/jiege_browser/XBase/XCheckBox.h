#pragma once
#include "xdockpanel.h"
#include "xbutton.h"
#include "xStatic.h"
#define EVENT_CHECKBOX_CLICK 100
class CXCheckBox :
	public CXDockPanel
{
public:
	CXCheckBox(void);
	~CXCheckBox(void);
	
	HRESULT SetCheck(BOOL bCheck);
	BOOL	GetCheck();

	HRESULT SetRadio(BOOL bRadio);
	BOOL	GetRadio();

	HRESULT SetText(CString strText);

	HRESULT Create(CXFrame* pParent);
	HRESULT SetCheckImage(CXDraw* pNormal,CXDraw* pOn,CXDraw* pDown);
	HRESULT SetUnCheckImage(CXDraw* pNormal,CXDraw* pOn,CXDraw* pDown);

	HRESULT OnXEvent(INT nId, CXFrame* pEventSource, void* pArg) ;
	HRESULT OnBtnClick(void* pArg);
private:
	BOOL m_bCheck;
	BOOL m_bRadio;
	CXButton* m_pBtn;
	CXStatic* m_pStatic;

	CXDraw* m_pCheckNormal;
	CXDraw* m_pCheckOn;
	CXDraw* m_pCheckDown;
	CXDraw* m_pUnCheckNormal;
	CXDraw* m_pUnCheckOn;
	CXDraw* m_pUnCheckDown;

	TXEventObserverObj<CXCheckBox> m_oEventListener;
};
