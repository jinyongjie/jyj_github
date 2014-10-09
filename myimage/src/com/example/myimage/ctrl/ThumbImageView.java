package com.example.myimage.ctrl;

import java.util.ArrayList;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.PointF;
import android.graphics.Rect;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.opengl.GLES10;
import android.util.FloatMath;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnLayoutChangeListener;
import android.view.View.OnTouchListener;
import android.widget.AdapterView;
import android.widget.ImageView;
import android.widget.AdapterView.OnItemClickListener;






import com.example.myimage.ctrl.AdapterTime.IImageItem;
import com.example.myimage.data.Helper;
import com.example.myimage.data.ImageLoader2;
import com.example.myimage.data.ImageMgr;
import com.example.myimage.data.ImageLoader2.ImageCallback;
import com.example.myimage.data.ImageMgr.ImageData;
import com.example.myimage.data.ImageMgr.ImageInfo;
import com.example.myimage.photoview.PhotoViewAttacher;
import com.example.myimage.R;


public class ThumbImageView extends ImageView implements ImageCallback,
		IImageItem {
	private String mPath = "";

	private int mImageWidth;
	private int mImageHeight;
	private boolean mSelect = false;

	private static Bitmap bmp_sel;
	private boolean mSelMode = false;
	private static Bitmap mBmpSelYes = null;
	private static Bitmap mBmpSelNo = null;
	public ThumbImageView(Context context) {
		super(context);

		// TODO Auto-generated constructor stub
		// this.addOnLayoutChangeListener(this);
		if(mBmpSelYes == null)
		{
			mBmpSelYes = BitmapFactory.decodeResource(getResources(), R.drawable.sel_yes);
		}
		if(mBmpSelNo == null)
		{
			mBmpSelNo = BitmapFactory.decodeResource(getResources(), R.drawable.sel_no);
		}
		Init(context);
	}

	public ThumbImageView(final android.content.Context context,
			android.util.AttributeSet attrs) {
		super(context, attrs);

		Init(context);
	}

	private int getImageWidth() {
		return mImageWidth;
	}

	private int getImageHeight() {
		return mImageHeight;
	}



	private void Init(Context context) {


	}

	public String GetImagePath() {
		return mPath;
	}
	public void setImageSize(int width,int height)
	{
		mImageWidth = width;
		mImageHeight = height;
	}
	public void setImagePath(String path) {

		if(path != null && mPath != null && path.equals(mPath))
		{
			return;
		}
		mPath = path;

		updateImage();

		setImageDrawable(null);

	}

	/*
	 * protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
	 * super.onMeasure(widthMeasureSpec, heightMeasureSpec);
	 * 
	 * setMeasuredDimension(mImageWidth, mImageHeight); }
	 */

	protected void onDraw(Canvas canvas) {
		if (this.getDrawable() != null) {
			super.onDraw(canvas);

		} else {
			canvas.drawRGB(50, 50, 50);
		}
		if (mSelMode) {
			//int n = Helper.dp2px(getContext(), 3);
			if (mSelect)
			{	
				canvas.drawBitmap(mBmpSelYes, getImageWidth()-mBmpSelYes.getWidth(), getImageHeight()-mBmpSelYes.getHeight(), null);
			}				
			else
			{
				canvas.drawBitmap(mBmpSelNo, getImageWidth()-mBmpSelNo.getWidth(), getImageHeight()-mBmpSelNo.getHeight(), null);
			}
			
		}
	}

	@Override
	public void onImageLoadSuccess(Bitmap bitmap, String path, int width,
			int height) {
		// TODO Auto-generated method stub

		if (mPath != null &&mPath.equals(path) && bitmap != null) {

			setImageBitmap(bitmap);
		}

	}

	@Override
	public void onImageLoadFail(String path, int width, int height) {
		// TODO Auto-generated method stub

	}

	@Override
	public void onImageRotate(String path) {
		// TODO Auto-generated method stub

	}

	public void setSel(boolean select) {
		if (mSelect != select) {
			mSelect = select;
			invalidate();
		}

	}

	public void setSelMode(boolean mode) {
		if (mSelMode != mode) {
			mSelMode = mode;
			invalidate();
		}
	}

	public boolean getSel() {
		return mSelect;
	}

	public void updateImage() {
		if (mPath != null && mPath.length() > 0)
			ImageLoader2.GetInstance().requestImage(mPath, getImageWidth(),
					getImageHeight(), ImageLoader2.quality_thumb, this);

		// ImageLoader.getInstance().displayImage("file://" + mPath, this,
		// mDisplayOptions, null);
	}

	@Override
	public void init(int width, OnClickListener l1, OnLongClickListener l2) {
		// TODO Auto-generated method stub
		this.setOnClickListener(l1);
		this.setOnLongClickListener(l2);
		this.setImageSize(width, width);
	}
	private ImageData mData;
	public void setData(ImageData data)
	{
		mData = data;
	}
	public ImageData getData()
	{
		return mData;
	}
}
