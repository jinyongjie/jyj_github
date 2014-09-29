package com.example.myimage.ctrl;

import java.util.Map;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.graphics.PointF;
import android.graphics.RectF;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.opengl.GLES10;
import android.util.AttributeSet;
import android.util.FloatMath;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.ProgressBar;

import com.example.myimage.ctrl.PhotoView2.IMaxSizeListener;
import com.example.myimage.data.ImageLoader2;
import com.example.myimage.data.ImageMgr;
import com.example.myimage.data.ImageLoader2.ImageCallback;
import com.example.myimage.data.ImageMgr.ImageInfo;
import com.example.myimage.data.ImageMgr.ImageMgrListener;
import com.example.myimage.photoview.PhotoViewAttacher;
import com.example.myimage.photoview.PhotoViewAttacher.AnimatedZoomRunnable;
import com.example.myimage.photoview.PhotoViewAttacher.OnMatrixChangedListener;
import com.example.myimage.photoview.PhotoViewAttacher.OnViewTapListener;
import com.example.myimage.R;

public class ViewSingle extends FrameLayout implements ImageCallback ,IMaxSizeListener,OnMatrixChangedListener{
	private final static String TAG="ViewSingle";
	public ViewSingle(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
	}

	public ViewSingle(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
	}

	public ViewSingle(android.content.Context context,
			android.util.AttributeSet attrs) {
		super(context, attrs);
	}

	public ViewSingle(Context context, AttributeSet attrs, Map inflateParams) {
		super(context, attrs);
	}

	private PhotoView2 mPhotoView2;
	private ProgressBar mProgressBar;
	private String  mPath = "";
	private int mDefaultWidth = 0;
	private int mDefaultHeight = 0;
	private int mMaxSize = 0;
	private int mQuality = ImageLoader2.quality_low;
	private int mCurWidth = 0;
	private int mCurHeight = 0;
	private int mRaww = 0;
	private int mRawh = 0;

	public void Init(OnViewTapListener viewtaplistener,int defw,int defh,String path ) {

		mPhotoView2 = (PhotoView2) findViewById(R.id.imageview);
		mProgressBar = (ProgressBar) findViewById(R.id.progressbar);


		SingleAttacher attacher = new SingleAttacher(mPhotoView2);
		mPhotoView2.SetAttacher(attacher);
		
		mPhotoView2.SetMaxSizeListener(this);
		mPhotoView2.setOnMatrixChangeListener(this);
		
		mPhotoView2.setOnViewTapListener(viewtaplistener);
		// mImageView.setOnTouchListener(new TouchListener());
		mDefaultWidth = defw;
		mDefaultHeight = defh;
		mPath = path;
		
		BitmapFactory.Options options = new BitmapFactory.Options();
		options.inJustDecodeBounds = true;
		BitmapFactory.decodeFile(path, options);
		mRaww = options.outWidth;
		mRawh = options.outHeight;
		
		CheckIfNeedUpdateImage();
	}

	public void Uninit() {
		mPhotoView2.setOnViewTapListener(null);
		mPath = "";
		
		mPhotoView2.SetMaxSizeListener(null);
		mPhotoView2.setOnMatrixChangeListener(null);
		

		this.ReleaseBitmap();
	}

	private class SingleAttacher extends PhotoViewAttacher {

		public SingleAttacher(ImageView imageView) {
			super(imageView);
			// TODO Auto-generated constructor stub
		}

		@Override
		public void zoomTo(float scale, float focalX, float focalY) {
			ImageView imageView = getImageView();

			super.zoomTo(scale, focalX, focalY);
		
		}
	}
	private void requestImage()
	{

		ImageLoader2.GetInstance().requestImage(mPath, mCurWidth, mCurHeight, mQuality,
				this);
	}


	private void CheckIfNeedUpdateImage()
	{
		int width = mDefaultWidth;
		int height = mDefaultHeight;
		if(mQuality == ImageLoader2.quality_high && mMaxSize > 0)
		{
			width = mMaxSize;
			height = mMaxSize;
		}
		else if(mQuality == ImageLoader2.quality_low)
		{
			width = mDefaultWidth/4;
			height= mDefaultHeight/4;
		}
		if(width == mCurWidth && height == mCurHeight)
		{
			return;
		}
		mCurWidth = width;
		mCurHeight = height;

		this.requestImage();
	}


	public boolean HaveImage() {
		if (mPhotoView2 == null || mPhotoView2.getDrawable() == null)
			return false;
		return true;
	}

	public void UpdateImage() {
	//	mProgressBar.setVisibility(View.VISIBLE);

		this.requestImage();
	}


	@Override
	public void onImageLoadSuccess(Bitmap bitmap,String path,int width,int height) {
		// TODO Auto-generated method stub

		if (mPath!=null &&path.equals(mPath) && bitmap != null && width == mCurWidth && height == mCurHeight) {
			mPhotoView2.setImageBitmapEx(bitmap,mRaww,mRawh);
		//	mProgressBar.setVisibility(View.GONE);
			
			
			if(mQuality == ImageLoader2.quality_low)
			{
				mQuality = ImageLoader2.quality_normal;
				CheckIfNeedUpdateImage();
			}
		}	
	}
	@Override
	public void onImageLoadFail(String path, int width, int height) {
		// TODO Auto-generated method stub
		if (mPath!=null &&path.equals(mPath) && width == mCurWidth && height == mCurHeight) {
			
		//	mProgressBar.setVisibility(View.GONE);
		}	
	}

	public void ReleaseBitmap() {
		Drawable d = mPhotoView2.getDrawable();
		if(d != null)
		{			
			mPhotoView2.setImageDrawable(null);
		}
		
	}

	@Override
	public void onImageRotate(String path) {
		// TODO Auto-generated method stub
		if (path.equals(mPath)) {
			this.requestImage();
		}
	}

	public void RotateImage() {
		mProgressBar.setVisibility(View.VISIBLE);

		ImageLoader2.GetInstance().rotateImage(mPath, this);
	}

	@Override
	public void OnMaxSizeChange() {
		// TODO Auto-generated method stub
		if(mPhotoView2 != null)
		{
			mMaxSize = mPhotoView2.GetMaxBitmapSize();
	
			this.CheckIfNeedUpdateImage();
		}
	}

	@Override
	public void onMatrixChanged(RectF rect) {
		// TODO Auto-generated method stub

		if(mQuality == ImageLoader2.quality_normal)
		{
			int w = new Float(rect.width()).intValue();
			int h = new Float(rect.height()).intValue();	
			if(w > mCurWidth || h> mCurHeight)
			{
				Log.d(TAG,String.format("onMatrixChanged %d %d  %d %d",w,h,mCurWidth,mCurHeight));
				mQuality = ImageLoader2.quality_high;
				this.CheckIfNeedUpdateImage();
			}
		}
	}



}