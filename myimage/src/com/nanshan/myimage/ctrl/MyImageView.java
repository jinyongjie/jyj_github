package com.nanshan.myimage.ctrl;

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
import android.util.FloatMath;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnLayoutChangeListener;
import android.view.View.OnTouchListener;
import android.widget.AdapterView;
import android.widget.ImageView;
import android.widget.AdapterView.OnItemClickListener;

import com.nanshan.myimage.R;
import com.nanshan.myimage.data.ImageLoader;
import com.nanshan.myimage.data.ImageMgr;
import com.nanshan.myimage.data.ImageLoader.ImageCallback;

class MyImageView extends ImageView implements ImageCallback {
	String mPath;
	int mImageId = -1;
	int mImageWidth;
	int mImageHeight;
	boolean mThumb = true;
	Bitmap mBmpSel;
	boolean mSelect = false;

	static Bitmap bmp_sel;

	public MyImageView(Context context) {
		super(context);

		// TODO Auto-generated constructor stub
		// this.addOnLayoutChangeListener(this);
		Init(context);
	}

	public MyImageView(final android.content.Context context,
			android.util.AttributeSet attrs) {
		super(context, attrs);

		Init(context);
	}

	void Init(Context context) {
		if (mBmpSel == null)
			mBmpSel = BitmapFactory.decodeResource(context.getResources(),
					R.drawable.main_selected_icon);
		// mBmpSel = ImageMgr.GetInstance().GetBmpSel();
	}

	public int GetImageId() {
		return mImageId;
	}

	public void SetImageId(int id) {
		if (mImageId != id) {
			if (mImageId != -1)
				ImageLoader.GetInstance().Cancel(mImageId, GetImageWidth(),
						GetImageHeight(), this);

			mImageId = id;

			if (id != -1) {
				mPath = ImageMgr.GetInstance().GetImage(id).path;
				ImageLoader.GetInstance().GetImage(id, mPath, mImageWidth,
						mImageHeight, mThumb, this);

			} else {

				mPath = null;
			}
			this.setImageDrawable(null);
		}
	}

	public int GetImageWidth() {
		return mImageWidth;
	}

	public int GetImageHeight() {
		return mImageHeight;
	}

	protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
		super.onMeasure(widthMeasureSpec, heightMeasureSpec);

		setMeasuredDimension(mImageWidth, mImageHeight);
	}

	void SetImageSize(int width, int height, boolean thumb) {
		mImageWidth = width;
		mImageHeight = height;
		mThumb = thumb;
	}

	String GetImagePath() {
		return mPath;
	}

	protected void onDraw(Canvas canvas) {
		if (this.getDrawable() != null) {
			super.onDraw(canvas);

		}

		else {

			canvas.drawRGB(50, 50, 50);
		}
		if (mSelect)
			canvas.drawBitmap(mBmpSel, mImageWidth - mBmpSel.getWidth(),
					mImageHeight - mBmpSel.getHeight(), null);
	}

	@Override
	public void OnImageLoad(Bitmap bitmap, String path) {
		// TODO Auto-generated method stub
		// 后请求的先执行 先请求的后到 覆盖正确结果
		if (path == mPath && bitmap != null) {

			setImageBitmap(bitmap);
		}

	}

	public void SetSelect(boolean select) {
		mSelect = select;
		this.invalidate();
	}

	public boolean GetSelect() {
		return mSelect;
	}

	@Override
	public void OnImageRotate(int id) {
		// TODO Auto-generated method stub
		
	}

	public void UpdateImage()
	{
		ImageLoader.GetInstance().GetImage(mImageId, mPath, mImageWidth,
				mImageHeight, mThumb, this);
	}


}
