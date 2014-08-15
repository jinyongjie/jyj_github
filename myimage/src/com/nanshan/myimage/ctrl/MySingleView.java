package com.nanshan.myimage.ctrl;

import java.util.Map;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.graphics.PointF;
import android.util.AttributeSet;
import android.util.FloatMath;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.ProgressBar;

import com.nanshan.myimage.R;
import com.nanshan.myimage.ctrl.PhotoViewAttacher.AnimatedZoomRunnable;
import com.nanshan.myimage.ctrl.PhotoViewAttacher.OnViewTapListener;
import com.nanshan.myimage.data.ImageLoader;
import com.nanshan.myimage.data.ImageLoader.ImageCallback;
import com.nanshan.myimage.data.ImageMgr.ImageMgrListener;
import com.nanshan.myimage.data.ImageMgr.change_type;

public class MySingleView extends FrameLayout implements ImageCallback {
	public MySingleView(Context context) {
		super(context);
		// TODO Auto-generated constructor stub

	}

	public MySingleView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
	}

	public MySingleView(android.content.Context context,
			android.util.AttributeSet attrs) {
		super(context, attrs);
	}

	public MySingleView(Context context, AttributeSet attrs, Map inflateParams) {
		super(context, attrs);
	}

	private PhotoView mImageView;
	private ProgressBar mProgressBar;
	private String mPath;
	private int mWidth;
	private int mHeight;
	private int mImageId;
	int mIndex;

	public void Init(int index,OnViewTapListener viewtaplistener) {

		mImageView = (PhotoView) findViewById(R.id.imageview);
		mProgressBar = (ProgressBar) findViewById(R.id.progressbar);
		mIndex = index;
		
		SingleAttacher attacher = new SingleAttacher(mImageView);
		mImageView.SetAttacher(attacher);
		mImageView.setOnViewTapListener(viewtaplistener);
	//	mImageView.setOnTouchListener(new TouchListener());

	}
	public void Uninit()
	{
		mImageView.setOnViewTapListener(null);
		SetPath(-1,null,0,0);
	}
	
	class SingleAttacher extends PhotoViewAttacher
	{

		public SingleAttacher(ImageView imageView) {
			super(imageView);
			// TODO Auto-generated constructor stub
		}
		@Override
		public void zoomTo(float scale, float focalX, float focalY) {
			ImageView imageView = getImageView();

			super.zoomTo(scale, focalX, focalY);
			//if(scale > 1)
			//UpdateImageBySize((int)(mWidth*scale),(int)(mHeight*scale));
		}
	}

	public void SetPath(int id, String path, int width, int height) {
		if (path != null) {
			mPath = path;
			mWidth = width;
			mHeight = height;
			mImageId = id;

			ImageLoader.GetInstance().GetImage(id, path, width, height, false,
					this);

		} else {
			// ImageLoader.GetInstance().RemoveFromCache(mImageId, mWidth,
			// mHeight);
			ImageLoader.GetInstance().Cancel(mImageId, mWidth, mHeight, this);
			SetBitmap(null);
		}

	}

	public boolean HaveImage() {
		if (mImageView == null || mImageView.getDrawable() == null)
			return false;
		return true;
	}

	public void UpdateImage() {
		mProgressBar.setVisibility(View.VISIBLE);

		ImageLoader.GetInstance().GetImage(mImageId, mPath, mWidth, mHeight,
				false, this);
	}
	public void UpdateImageBySize(int width,int height) {

		ImageLoader.GetInstance().GetImage(mImageId, mPath, width, height,
				false, this);
	}
	@Override
	public void OnImageLoad(Bitmap bitmap, String path) {
		// TODO Auto-generated method stub

		if (path == mPath && bitmap != null) {
			SetBitmap(bitmap);

		}
		mProgressBar.setVisibility(View.GONE);
	}

	private void SetBitmap(Bitmap bitmap) {

		if (bitmap != null) {
			mImageView.setImageBitmap(bitmap);
		} else {
			mImageView.setImageDrawable(null);
		}

	}
	public void ReleaseBitmap()
	{
		mImageView.setImageDrawable(null);
	}

	@Override
	public void OnImageRotate(int id) {
		// TODO Auto-generated method stub
		if(id == mImageId)
		{
			ImageLoader.GetInstance().GetImage(mImageId, mPath, mWidth, mHeight,
					false, this);
		}
	}
	public void RotateImage()
	{
		mProgressBar.setVisibility(View.VISIBLE);
		ImageLoader.GetInstance().RotateImage(mImageId, mPath, this);
	}
}