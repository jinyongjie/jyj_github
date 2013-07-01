#include "StdAfx.h"
#include "XScrollFrame.h"

CXScrollFrame::CXScrollFrame(void):m_oEventListener(this)
{
	m_pVScrollBar = NULL;
	m_pContent = NULL;
	m_oEventListener.Hook_OnXEvent(&CXScrollFrame::OnXEvent);
	m_pContainer = NULL;

}

CXScrollFrame::~CXScrollFrame(void)
{
	m_oEventListener.UnhookAll();

	if(m_pVScrollBar)
	{
		DEL_EVENT_OBSERVER(m_pVScrollBar,EVENT_SCROLLBAR_ROLL,&m_oEventListener);
	}
}


HRESULT CXScrollFrame::Relayout()
{
	if(m_pVScrollBar == NULL || m_pContainer == NULL)
	{

		return E_FAIL;
	}

	if(m_pContent == NULL)
	{
		m_pVScrollBar->SetVisible(FALSE);
		__super::Relayout();
	}
	else
	{
		for(int i=0;i<2;i++)
		{
			CRect rcScroll = m_pVScrollBar->GetRect();
			CRect rcRemain = GetRect();
			CRect rcContent = m_pContent->GetRect();
			if(rcContent.Height() <= rcRemain.Height())
			{
				m_pVScrollBar->SetVisible(FALSE);
				m_pContainer->SetRect(CRect(0,0,rcRemain.Width(),rcRemain.Height()));
			}
			else
			{
				m_pVScrollBar->SetVisible(TRUE);

				m_pVScrollBar->SetRect(CRect(rcRemain.Width() - rcScroll.Width(),0,rcRemain.Width(),rcRemain.Height()));
				m_pContainer->SetRect(CRect(0,0,rcRemain.Width() - rcScroll.Width(),rcRemain.Height()));

				m_pVScrollBar->SetRange(rcContent.Height());

				if(rcContent.top > 0)
				{
					rcContent.bottom = rcContent.Height();
					rcContent.top = 0;
					m_pVScrollBar->SetPos(0);
					m_pContent->SetRect(rcContent);
				}
				if(rcContent.bottom < rcRemain.Height())
				{
					rcContent.top = rcRemain.Height() - rcContent.Height();
					rcContent.bottom = rcRemain.Height();
					m_pVScrollBar->SetPos(-rcContent.top);
					m_pContent->SetRect(rcContent);
				}
			}
			__super::Relayout();
			if(rcContent != m_pContent->GetRect())
			{
				continue;
			}
			break;
		}
	}

	return S_OK;
}

HRESULT CXScrollFrame::Create( CXFrame* pParent )
{
	__super::Create(pParent);

	if(m_pVScrollBar == NULL)
	{
		m_pVScrollBar = new CXVScrollBar;
		AddFrame(m_pVScrollBar);
		ADD_EVENT_OBSERVER(m_pVScrollBar,EVENT_SCROLLBAR_ROLL,&m_oEventListener);
	}
	if(m_pContainer == NULL)
	{
		m_pContainer = new CXFrame;
		AddFrame(m_pContainer);
	}
	SetSensitiveChildSize(TRUE);
	m_pContainer->SetSensitiveChildSize(TRUE);

	return S_OK;
}


CXVScrollBar* CXScrollFrame::GetVScrollBar()
{
	return m_pVScrollBar;
}

HRESULT CXScrollFrame::SetContent( CXFrame* pFrame )
{
	if(m_pContainer == NULL)
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}
	if(m_pContent == pFrame)
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}
	if(m_pContent)
	{
		DelFrame(m_pContent);
	}
	m_pContent = pFrame;
	if(m_pContent)
	{
		m_pContainer->AddFrame(m_pContent);
		CRect rcFrame = m_pContent->GetRect();
		if(m_pVScrollBar)
		{
			m_pVScrollBar->SetRange(rcFrame.Height());
			m_pVScrollBar->SetPos(0);
		}
		m_pContent->SetPosition(CPoint(0,0));
	}

	MarkRelayout();
	return S_OK;
}

CSize CXScrollFrame::GetContainerSize()
{
	if(m_pContainer)
	{
		return m_pContainer->GetRect().Size();
	}
	return CSize(0,0);
}

HRESULT CXScrollFrame::OnXEvent( INT nId, CXFrame* pEventSource, void* pArg )
{
	BEGIN_EVENT_MAP(pEventSource,nId,pArg)
		ON_EVENT_BY_ID(m_pVScrollBar,EVENT_SCROLLBAR_ROLL,OnScrollBarRoll)
	END_EVENT_MAP()
	return S_OK;
}

HRESULT CXScrollFrame::OnScrollBarRoll( void* pArg )
{
	int nPos = m_pVScrollBar->GetPos();
	if(m_pContent)
	{
		CRect rc = m_pContent->GetRect();
		if(rc.top != -nPos)
		{
			m_pContent->SetPosition(CPoint(rc.left,-nPos));
			MarkRelayout();
			InvalidateFrame();
		}
	}
	return S_OK;
}

HRESULT CXScrollFrame::Roll( int nStep )
{
	return m_pVScrollBar->Roll(nStep);
}