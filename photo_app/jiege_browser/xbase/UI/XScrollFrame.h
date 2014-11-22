#pragma once
#include "xframe.h"
#include "XVScrollBar.h"
class CXScrollFrame :
	public CXFrame
{
public:
	CXScrollFrame(void);
	~CXScrollFrame(void);

	HRESULT Create(CXFrame* pParent);
	HRESULT Relayout();

	HRESULT	SetContent(CXFrame* pFrame);


	virtual CXVScrollBar* GetVScrollBar();
	CSize GetContainerSize();

	HRESULT OnXEvent(INT nId, CXFrame* pEventSource, void* pArg) ;
	HRESULT OnScrollBarRoll(void* pArg);
	HRESULT Roll(int nStep);
private:
	CXVScrollBar* m_pVScrollBar;
	CXFrame*	m_pContent;
	CXFrame*	m_pContainer;
	TXEventObserverObj<CXScrollFrame> m_oEventListener;
};
