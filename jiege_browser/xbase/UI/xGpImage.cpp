#include "StdAfx.h"
#include "xgpimage.h"
#include "ResourceMgr.h"

ULONG_PTR CGpImage::m_gdiplusToken = NULL;

GdiplusStartupInput gdiplusStartupInput;


CGpImage::CGpImage(void)
{
	m_pImage = NULL;
	m_nDrawType = XDRAWTYPE_STRETCH;
	m_rcCut = CRect(0,0,0,0);
	m_rcTile = CRect(0,0,0,0);
	m_bEnableGL = FALSE;
}

CGpImage::~CGpImage(void)
{
	if(m_pImage)
	{
		delete m_pImage;
		m_pImage = NULL;
	}
}

HRESULT CGpImage::Draw( HDC hdc,CRect rcPaint )
{
	CRect rcImage = CRect(0,0,m_pImage->GetWidth(),m_pImage->GetHeight());
	if(!m_rcCut.IsRectEmpty())
	{
		::IntersectRect(&rcImage,rcImage,m_rcCut);
	}
	switch(m_nDrawType)
	{
	case XDRAWTYPE_NORMAL:
		Internal_DrawNormal(hdc,rcPaint,rcImage);
		break;
	case XDRAWTYPE_STRETCH:
		Internal_DrawStretch(hdc,rcPaint,rcImage);
		break;
	case XDRAWTYPE_TITLE:
		Internal_DrawTile(hdc,rcPaint,rcImage);
		break;
	case XDRAWTYPE_9PART:
		Internal_Draw9Part(hdc,rcPaint,rcImage);
		break;
	case XDRAWTYPE_3PARTH:
		Internal_Draw3PartH(hdc,rcPaint,rcImage);
		break;
	case XDRAWTYPE_3PARTV:
		Internal_Draw3PartV(hdc,rcPaint,rcImage);
		break;
	default:
		break;
	}


	return S_OK;
}

HRESULT CGpImage::InitGdiplus()
{
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	return S_OK;
}

HRESULT CGpImage::SetFile( LPCWSTR strFile )
{
	m_strFile = strFile;
	m_pImage = CResourceMgr::GetBitmap(strFile);
	return S_OK;
}


int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT num= 0;
	UINT size= 0;

	ImageCodecInfo* pImageCodecInfo= NULL;

	GetImageEncodersSize(&num, &size);
	if(size== 0)
	{
		return -1;
	}
	pImageCodecInfo= (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo== NULL)
	{
		return -1;
	}

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j=0; j< num; ++j)
	{
		if(wcscmp(pImageCodecInfo[j].MimeType, format)== 0)
		{
			*pClsid= pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}

	free(pImageCodecInfo);
	return -1;
}

HRESULT CGpImage::SaveFile( CString strPath )
{
	GUID encoder;
	GetEncoderClsid(L"image/png",&encoder);

	m_pImage->Save(strPath,&encoder);
	return S_OK;
}

HRESULT CGpImage::SetDrawType( int nType )
{
	m_nDrawType = nType;
	return S_OK;
}

HRESULT CGpImage::SetCutRect( CRect rcCut )
{
	m_rcCut = rcCut;
	return S_OK;
}

CSize CGpImage::GetSize()
{
	if(m_pImage)
	{
		return CSize(m_pImage->GetWidth(),m_pImage->GetHeight());
	}
	return CSize(0,0);
}

HRESULT CGpImage::SetTileRect( CRect rc )
{
	m_rcTile = rc;
	return S_OK;
}

HRESULT CGpImage::Internal_DrawNormal(HDC hdc,CRect rcPaint,CRect rcImage)
{
	if(m_pImage == NULL)
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}
	Graphics g(hdc);

	RectF dest(rcPaint.left,rcPaint.top,rcPaint.Width(),rcPaint.Height());
	if(rcPaint.Width() > rcImage.Width())
	{
		dest.Width = rcImage.Width();
	}
	if(rcPaint.Height() > rcImage.Height())
	{
		dest.Height = rcImage.Height();
	}

	CRect rc = rcImage;
	if(rcPaint.Width() < rcImage.Width())
	{
		rc.right = rc.left + rcPaint.Width();
	}
	if(rcPaint.Height() < rcImage.Height())
	{
		rc.bottom = rc.top + rcPaint.Height();
	}

	//g.DrawImage(m_pImage,dest,rc.left,rc.top,rc.Width(),rc.Height(),UnitPixel);

	Point destPoints[]={
		Point(dest.GetLeft(),dest.GetTop() + 80),
		Point(dest.GetRight() - 50,dest.GetTop()),
		Point(dest.GetLeft(),dest.GetBottom()+80),
		Point(dest.GetRight(),dest.GetBottom()-20)
	};

	g.DrawImage(m_pImage,destPoints,3);



	
	return S_OK;
}
HRESULT CGpImage::Internal_DrawStretch(HDC hdc,CRect rcPaint,CRect rcImage)
{
	if(m_pImage == NULL)
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}
	Graphics g(hdc);

	RectF dest(rcPaint.left,rcPaint.top,rcPaint.Width(),rcPaint.Height());
	g.DrawImage(m_pImage,dest,rcImage.left,rcImage.top,rcImage.Width(),rcImage.Height(),UnitPixel);
	
	return S_OK;
}
HRESULT CGpImage::Internal_Draw9Part(HDC hdc,CRect rcPaint,CRect rcImage)
{
	if(m_pImage == NULL)
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}
	CRect rcArrayImage[9];
	CRect rcArrayDest[9];
	rcArrayImage[0] = CRect(rcImage.left,rcImage.top,rcImage.left + m_rcTile.left,rcImage.top+m_rcTile.top);
	rcArrayImage[1] = CRect(rcImage.left + m_rcTile.left,rcImage.top,rcImage.left + m_rcTile.right,rcImage.top+m_rcTile.top);
	rcArrayImage[2] = CRect(rcImage.left + m_rcTile.right,rcImage.top,rcImage.right,rcImage.top+m_rcTile.top);
	
	rcArrayImage[3] = CRect(rcImage.left,rcImage.top + m_rcTile.top,rcImage.left + m_rcTile.left,rcImage.top+m_rcTile.bottom);
	rcArrayImage[4] = CRect(rcImage.left + m_rcTile.left,rcImage.top+ m_rcTile.top,rcImage.left + m_rcTile.right,rcImage.top+m_rcTile.bottom);
	rcArrayImage[5] = CRect(rcImage.left + m_rcTile.right,rcImage.top+ m_rcTile.top,rcImage.right,rcImage.top+m_rcTile.bottom);
	
	rcArrayImage[6] = CRect(rcImage.left,rcImage.top + m_rcTile.bottom,rcImage.left + m_rcTile.left,rcImage.bottom);
	rcArrayImage[7] = CRect(rcImage.left + m_rcTile.left,rcImage.top + m_rcTile.bottom,rcImage.left + m_rcTile.right,rcImage.bottom);
	rcArrayImage[8] = CRect(rcImage.left + m_rcTile.right,rcImage.top + m_rcTile.bottom,rcImage.right,rcImage.bottom);

	rcArrayDest[0] = CRect(rcPaint.left,rcPaint.top,rcPaint.left + m_rcTile.left,rcPaint.top+m_rcTile.top);//
	rcArrayDest[1] = CRect(rcPaint.left + m_rcTile.left,rcPaint.top,rcPaint.right - (rcImage.Width() - m_rcTile.right),rcPaint.top+m_rcTile.top);
	rcArrayDest[2] = CRect(rcPaint.right - (rcImage.Width() - m_rcTile.right),rcPaint.top,rcPaint.right,rcPaint.top+m_rcTile.top);//

	rcArrayDest[3] = CRect(rcPaint.left,rcPaint.top + m_rcTile.top,rcPaint.left + m_rcTile.left,rcPaint.bottom - (rcImage.Height() - m_rcTile.bottom));//
	rcArrayDest[4] = CRect(rcPaint.left + m_rcTile.left,rcPaint.top + m_rcTile.top,rcPaint.right - (rcImage.Width() - m_rcTile.right),rcPaint.bottom - (rcImage.Height() - m_rcTile.bottom));
	rcArrayDest[5] = CRect(rcPaint.right - (rcImage.Width() - m_rcTile.right),rcPaint.top + m_rcTile.top,rcPaint.right,rcPaint.bottom - (rcImage.Height() - m_rcTile.bottom));//

	rcArrayDest[6] = CRect(rcPaint.left,rcPaint.bottom - (rcImage.Height() - m_rcTile.bottom),rcPaint.left + m_rcTile.left,rcPaint.bottom);//
	rcArrayDest[7] = CRect(rcPaint.left + m_rcTile.left,rcPaint.bottom - (rcImage.Height() - m_rcTile.bottom),rcPaint.right - (rcImage.Width() - m_rcTile.right),rcPaint.bottom);
	rcArrayDest[8] = CRect(rcPaint.right - (rcImage.Width() - m_rcTile.right),rcPaint.bottom - (rcImage.Height() - m_rcTile.bottom),rcPaint.right,rcPaint.bottom);//

	for(int i = 0;i<9;i++)
	{
		Internal_DrawTile(hdc,rcArrayDest[i],rcArrayImage[i]);
	}


	return S_OK;
}
HRESULT CGpImage::Internal_Draw3PartH(HDC hdc,CRect rcPaint,CRect rcImage)
{
	if(m_pImage == NULL)
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}
	CRect rcArrayImage[3];
	CRect rcArrayDest[3];
	rcArrayImage[0] = CRect(rcImage.left,rcImage.top,rcImage.left + m_rcTile.left,rcImage.bottom);
	rcArrayImage[1] = CRect(rcImage.left + m_rcTile.left,rcImage.top,rcImage.left + m_rcTile.right,rcImage.bottom);
	rcArrayImage[2] = CRect(rcImage.left + m_rcTile.right,rcImage.top,rcImage.right,rcImage.bottom);

	rcArrayDest[0] = CRect(rcPaint.left,rcPaint.top,rcPaint.left + m_rcTile.left,rcPaint.bottom);//
	rcArrayDest[1] = CRect(rcPaint.left + m_rcTile.left,rcPaint.top,rcPaint.right - (rcImage.Width() - m_rcTile.right),rcPaint.bottom);
	rcArrayDest[2] = CRect(rcPaint.right - (rcImage.Width() - m_rcTile.right),rcPaint.top,rcPaint.right,rcPaint.bottom);//

	for(int i = 0;i<3;i++)
	{
		Internal_DrawStretch(hdc,rcPaint[i],rcArrayImage[i]);
	}
	return S_OK;
}
HRESULT CGpImage::Internal_Draw3PartV(HDC hdc,CRect rcPaint,CRect rcImage)
{
	if(m_pImage == NULL)
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}
	CRect rcArrayImage[3];
	CRect rcArrayDest[3];
	rcArrayImage[0] = CRect(rcImage.left,rcImage.top,rcImage.right,rcImage.top + m_rcTile.top);
	rcArrayImage[1] = CRect(rcImage.left,rcImage.top + m_rcTile.top,rcImage.right,rcImage.top + m_rcTile.bottom);
	rcArrayImage[2] = CRect(rcImage.left,rcImage.top + m_rcTile.bottom,rcImage.right,rcImage.bottom);

	rcArrayDest[0] = CRect(rcPaint.left,rcPaint.top,rcPaint.right,rcPaint.top + m_rcTile.top);//
	rcArrayDest[1] = CRect(rcPaint.left,rcPaint.top+m_rcTile.top,rcPaint.right,rcPaint.bottom - (rcImage.Height() - m_rcTile.bottom));
	rcArrayDest[2] = CRect(rcPaint.left,rcPaint.bottom - (rcImage.Height() - m_rcTile.bottom),rcPaint.right,rcPaint.bottom);//

	for(int i = 0;i<3;i++)
	{
		Internal_DrawStretch(hdc,rcPaint[i],rcArrayImage[i]);
	}
	return S_OK;
}
HRESULT CGpImage::Internal_DrawTile(HDC hdc,CRect rcPaint,CRect rcImage)
{
	if(m_pImage == NULL)
	{
		ATLASSERT(FALSE);
		return E_FAIL;
	}

	int y = rcPaint.top;
	HDC hdcmem = CreateCompatibleDC(hdc);
	HBITMAP hbmp = CreateCompatibleBitmap(hdc,rcImage.Width(),rcImage.Height());
	HGDIOBJ hOld = SelectObject(hdcmem,hbmp);
	Internal_DrawNormal(hdcmem,CRect(0,0,rcImage.Width(),rcImage.Height()),rcImage);

	while(y < rcPaint.bottom)
	{
		int x = rcPaint.left;
		while(x < rcPaint.right)
		{
			CRect rcDest(x,y,x + rcImage.Width(),y+rcImage.Height());
			CRect rcSrc(rcImage);
			if(rcDest.right > rcPaint.right)
			{
				rcDest.right = rcPaint.right;
			}
			if(rcDest.bottom > rcPaint.bottom)
			{
				rcDest.bottom = rcPaint.bottom;
			}
			if(rcSrc.right > rcImage.right)
			{
				rcSrc.right = rcImage.right;
			}
			if(rcSrc.bottom > rcImage.bottom)
			{
				rcSrc.bottom = rcImage.bottom;
			}
		
			::BitBlt(hdc,rcDest.left,rcDest.top,rcDest.Width(),rcDest.Height(),hdcmem,0,0,SRCCOPY);	

			//BLENDFUNCTION bf = {AC_SRC_OVER,0,255,AC_SRC_ALPHA};
		
			//::AlphaBlend(hdc,rcDest.left,rcDest.top,rcDest.Width(),rcDest.Height(),hdcmem,0,0,rcDest.Width(),rcDest.Height(),bf);
			
			x += rcImage.Width();
		}
		y += rcImage.Height();
	}
	SelectObject(hdcmem,hOld);
	DeleteObject(hbmp);
	DeleteDC(hdcmem);

	return S_OK;
}

//enum RotateFlipType
//{
//	RotateNoneFlipNone = 0,
//	Rotate90FlipNone   = 1,
//	Rotate180FlipNone  = 2,
//	Rotate270FlipNone  = 3,
//
//	RotateNoneFlipX    = 4,
//	Rotate90FlipX      = 5,
//	Rotate180FlipX     = 6,
//	Rotate270FlipX     = 7,
//
//	RotateNoneFlipY    = Rotate180FlipX,
//	Rotate90FlipY      = Rotate270FlipX,
//	Rotate180FlipY     = RotateNoneFlipX,
//	Rotate270FlipY     = Rotate90FlipX,
//
//	RotateNoneFlipXY   = Rotate180FlipNone,
//	Rotate90FlipXY     = Rotate270FlipNone,
//	Rotate180FlipXY    = RotateNoneFlipNone,
//	Rotate270FlipXY    = Rotate90FlipNone
//};

HRESULT CGpImage::RotateFlip( int nVal )
{
	if(m_pImage)
	{
		//m_pImage->RotateFlip(RotateNoneFlipX);


		//REAL   PosX = dest.GetLeft(),PosY = dest.GetTop();	 //图像的初始位置 
		//REAL   centerX = 10,centerY = 10;	 //旋转中心 
		//REAL   Rangle=30;	 //旋转角度 
		//PointF   CenterPoint(centerX,centerY); 

		//Matrix   RotateMatrix;	 //旋转矩阵	
		//RotateMatrix.RotateAt(Rangle,CenterPoint,MatrixOrderPrepend) ;
		//g.SetTransform(&RotateMatrix);
		//g.DrawImage   (m_pImage,PosX,PosY); 
	}
	return S_OK;
}

HRESULT CGpImage::SetEnableGL( BOOL bEnable )
{
	m_bEnableGL = TRUE;
	return S_OK;
}

