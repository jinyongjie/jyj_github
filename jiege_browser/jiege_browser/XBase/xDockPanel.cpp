#include "StdAfx.h"
#include "xdockPanel.h"

CXDockPanel::CXDockPanel(void)
{

}

CXDockPanel::~CXDockPanel(void)
{

}

HRESULT CXDockPanel::Relayout()
{
	CRect rc = GetRect();

	CRect rcRemain(0,0,rc.Width(),rc.Height());
	int nCount = GetFrameCount();
	for(int i = 0; i< nCount ; i++)
	{
		CXFrame* pChild = GetFrameByIndex(i);
		if(pChild == NULL)
		{
			ATLASSERT(FALSE);
			continue;
		}

		if(!pChild->GetVisible())
		{
			continue;
		}
		CRect rcMargin = pChild->GetMargin();
		CRect rcChild = pChild->GetRect();
		int nDockType = pChild->GetDockType();
		BOOL bAutoHeight = pChild->GetAutoHeight();
		BOOL bAutoWidth = pChild->GetAutoWidth();
		CRect rcNew(0,0,0,0);//新位置

		switch (nDockType)
		{
		case XDOCK_LEFT:
			{
				rcNew.left = rcRemain.left + rcMargin.left;
				rcNew.right = rcNew.left + rcChild.Width();
				rcNew.top = rcRemain.top + rcMargin.top;
				if(bAutoHeight)
				{
					rcNew.bottom = rcNew.top + rcChild.Height();
				}
				else
				{
					rcNew.bottom = rcRemain.bottom - rcMargin.bottom;
				}
				

				rcRemain.left = rcNew.right;
			}
			break;
		case XDOCK_RIGHT:
			{
				rcNew.right = rcRemain.right - rcMargin.right;
				rcNew.left = rcNew.right - rcChild.Width();
				rcNew.top = rcRemain.top + rcMargin.top;
				if(bAutoHeight)
				{
					rcNew.bottom = rcNew.top + rcChild.Height();
				}
				else
				{
					rcNew.bottom = rcRemain.bottom - rcMargin.bottom;
				}

				rcRemain.right = rcNew.left;
			}
			break;
		case XDOCK_TOP:
			{
				rcNew.top = rcRemain.top + rcMargin.top;
				rcNew.bottom = rcNew.top + rcChild.Height();
				rcNew.left = rcRemain.left + rcMargin.left;

				if(bAutoWidth)
				{
					rcNew.right = rcNew.left + rcChild.Width();
				}
				else
				{
					rcNew.right = rcRemain.right - rcMargin.right;
				}

				rcRemain.top = rcNew.bottom;
			}
			break;
		case XDOCK_BOTTOM:
			{
				rcNew.bottom = rcRemain.bottom - rcMargin.bottom;
				rcNew.top = rcNew.bottom - rcChild.Height();
				rcNew.left = rcRemain.left + rcMargin.left;
				if(bAutoWidth)
				{
					rcNew.right = rcNew.left + rcChild.Width();
				}
				else
				{
					rcNew.right = rcRemain.right - rcMargin.right;
				}

				rcRemain.bottom = rcNew.top;
			}
			break;
		case XDOCK_FILL:
			{
				rcNew.left = rcRemain.left + rcMargin.left;
				rcNew.right = rcRemain.right - rcMargin.right;
				rcNew.top = rcRemain.top + rcMargin.top;
				rcNew.bottom = rcRemain.bottom - rcMargin.bottom;

				rcRemain = CRect(0,0,0,0);
			}
			break;
		default:
			ATLASSERT(FALSE);
			break;
		}
		pChild->SetRect(rcNew);

		//剩余空间不足，退出
		if(rcRemain.Width()<=0 || rcRemain.Height() <= 0)
		{
			break;
		}
	}
	__super::Relayout();
	return S_OK;
}

