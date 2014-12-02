package com.example.browser_main;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.BlurMaskFilter;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.PaintFlagsDrawFilter;
import android.graphics.PorterDuff.Mode;
import android.graphics.PorterDuffXfermode;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.MotionEvent;
import android.view.View;
import android.view.animation.AccelerateDecelerateInterpolator;
import android.view.animation.AccelerateInterpolator;
import android.view.animation.DecelerateInterpolator;
import android.animation.Animator;
import android.animation.Animator.AnimatorListener;
import android.animation.ObjectAnimator;
import android.animation.ValueAnimator;
import android.animation.ValueAnimator.AnimatorUpdateListener;
import android.app.Activity;
import android.graphics.PorterDuff;

public class TabSwitch extends View implements OnGestureListener,
		AnimatorUpdateListener {
	private int mCount = 4;
	private Bitmap[] mBitmap = new Bitmap[mCount];
	private Matrix[] mMatrix = new Matrix[mCount];
	private int mCurrentIndex = 0;
	private int[] mIds = { R.drawable.image1, R.drawable.image2,
			R.drawable.image3, R.drawable.image4 };
	static private String TAG = "TabSwitch";
	private GestureDetector mGestureDetector;
	private String mTitle = "新建标签页";
	private int mOffsetY = 500;
	// private int mOffsetX = 200;
	private float mAngle = 0;
	private int mScreenX = 600;
	private int mScreenY = 800;
	private int mAngleSpace = 10;
	private int mCenterX = 0;
	private int mCenterY = 0;

	public TabSwitch(Context context, AttributeSet attrs) {
		super(context, attrs);
		// TODO Auto-generated constructor stub
		mGestureDetector = new GestureDetector(context, this);

	}
	public void init() {

		DisplayMetrics dm = new DisplayMetrics();
		((Activity) getContext()).getWindowManager().getDefaultDisplay()
				.getMetrics(dm);
		mScreenX = dm.widthPixels;
		mScreenY = dm.heightPixels;
		mCenterX = mScreenX/2;
		mCenterY = mScreenY*2;
		BitmapFactory.Options options = new BitmapFactory.Options();
		options.inSampleSize = 2;

		for (int i = 0; i < mCount; i++) {
			Bitmap bmp = (BitmapFactory.decodeResource(getResources(), mIds[i],
					options));
			Matrix m = new Matrix();
			float sx = (float) (mScreenX / (2.0 * bmp.getWidth()));
			m.setScale(sx, sx);
			mBitmap[i] = Bitmap.createBitmap(bmp, 0, 0, bmp.getWidth(),
					bmp.getHeight(), m, true);
		}

		calc();

	}
	public float getAngle() {
		return mAngle;
	}

	public void setAngle(float angle) {
		mAngle = angle;
		//Log.d(TAG,String.format("setAngle %f",angle));
		calc();
		invalidate();
	}

	private Bitmap drawImageDropShadow(Bitmap originalBitmap) {

		BlurMaskFilter blurFilter = new BlurMaskFilter(3,
				BlurMaskFilter.Blur.NORMAL);
		Paint shadowPaint = new Paint();
		shadowPaint.setAlpha(50);
		shadowPaint.setColor(Color.BLACK);
		shadowPaint.setMaskFilter(blurFilter);

		int[] offsetXY = new int[2];
		Bitmap shadowBitmap = originalBitmap
				.extractAlpha(shadowPaint, offsetXY);

		Bitmap shadowImage32 = shadowBitmap.copy(Bitmap.Config.RGB_565, true);
		Canvas c = new Canvas(shadowImage32);
		c.drawBitmap(originalBitmap, offsetXY[0], offsetXY[1], null);

		return shadowImage32;
	}

	@Override
	protected void onDraw(Canvas canvas) {
		// TODO Auto-generated method stub
		// super.onDraw(canvas);

		PaintFlagsDrawFilter pfd = new PaintFlagsDrawFilter(0,
				Paint.ANTI_ALIAS_FLAG | Paint.FILTER_BITMAP_FLAG);
		canvas.setDrawFilter(pfd);

		Paint paint = new Paint();
		paint.setShadowLayer(5, 8, 7, Color.DKGRAY);

		// paint.setBitmapFilter(true);
		for (int i = 0; i < mCount; i++)
			canvas.drawBitmap(mBitmap[i], mMatrix[i], paint);

		Paint textPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
		textPaint.setTextSize(50);
		textPaint.setColor(Color.WHITE);
		canvas.drawText(mTitle, 400, 300, textPaint);
		
		textPaint.setColor(Color.rgb(123, 148, 174));
		Rect r = new Rect(0,getHeight() - 20,mScreenX,getHeight());
		canvas.drawRect(r, textPaint);

		textPaint.setColor(Color.rgb(134, 157, 181));
		Rect r1 = new Rect(0,getHeight() - 2,getWidth(),getHeight());
		canvas.drawRect(r1, textPaint);
		
		int w = getWidth()/mCount;
		
		textPaint.setColor(Color.WHITE);
		int x = (int) (-getWidth()*mAngle/(mAngleSpace*mCount));
		Rect r2 = new Rect(x,getHeight() - 2,x + w,getHeight());
		canvas.drawRect(r2, textPaint);
	}



	public void calc() {
		for (int i = 0; i < mCount; i++) {

			float angle = mAngle + mAngleSpace * i;
			mMatrix[i] = new Matrix();
			mMatrix[i].setTranslate((mScreenX - mBitmap[i].getWidth()) / 2,
					mOffsetY);
			mMatrix[i].postRotate(angle, mCenterX, mCenterY);

		}
	}

	private ValueAnimator mRestoreAnim;
	private ValueAnimator mFlingAnim;

	private void restorePos() {
		int index = 0;
		float minangle = 360;
		for (int i = 0; i < mCount; i++) {
			float angle = mAngle + mAngleSpace * i;
			if (Math.abs(angle) < Math.abs(minangle)) {
				index = i;
				minangle = angle;
			}
		}
		mCurrentIndex = index;
		/*
		 * mAngle = -mAngleSpace*index; calc(); invalidate();
		 */
		if (mRestoreAnim != null)
			mRestoreAnim.cancel();
		float dest = (float)(-mAngleSpace* index);
		animRestore(dest);
	//	Log.d(TAG,String.format("begin anim %f %f", mAngle,dest));
	}
	private void cancelAllAnim()
	{
		if(mRestoreAnim != null && mRestoreAnim.isRunning())
			mRestoreAnim.cancel();
		if(mFlingAnim != null && mFlingAnim.isRunning())
			mFlingAnim.cancel();
	}
	private void animRestore(float dest)
	{
		cancelAllAnim();
		mRestoreAnim = ObjectAnimator.ofFloat(this, "angle", mAngle,dest );
		mRestoreAnim.setDuration(200);
		
		mRestoreAnim.addUpdateListener(this);
		mRestoreAnim.start();
	}
	
	private void animFling(float dest,long duration)
	{
		cancelAllAnim();
		mFlingAnim = ObjectAnimator.ofFloat(this, "angle", mAngle,dest );
		mFlingAnim.setInterpolator(new DecelerateInterpolator());
		mFlingAnim.setDuration(duration);
		
		mFlingAnim.addUpdateListener(this);
		mFlingAnim.addListener(new AnimatorListener(){

			@Override
			public void onAnimationStart(Animator animation) {
				// TODO Auto-generated method stub
				
			}

			@Override
			public void onAnimationEnd(Animator animation) {
				// TODO Auto-generated method stub
				restorePos();
			}

			@Override
			public void onAnimationCancel(Animator animation) {
				// TODO Auto-generated method stub
				
			}

			@Override
			public void onAnimationRepeat(Animator animation) {
				// TODO Auto-generated method stub
				
			}
			
		});
		mFlingAnim.start();
	}


	@Override
	public boolean onTouchEvent(MotionEvent event) {
		// TODO Auto-generated method stub
		// return super.onTouchEvent(event);
		int action = event.getAction();
		switch (action) {
		case MotionEvent.ACTION_DOWN:

			if (mRestoreAnim != null && mRestoreAnim.isRunning()) 
				mRestoreAnim.cancel();
			break;
		case MotionEvent.ACTION_UP:
		
			restorePos();
			break;
		case MotionEvent.ACTION_MOVE:
	
			break;
		case MotionEvent.ACTION_CANCEL:
			restorePos();
			break;
		}
		return mGestureDetector.onTouchEvent(event);

	}

	@Override
	public boolean onDown(MotionEvent e) {
		// TODO Auto-generated method stub
		return true;
	}

	@Override
	public void onShowPress(MotionEvent e) {
		// TODO Auto-generated method stub

	}

	@Override
	public boolean onSingleTapUp(MotionEvent e) {
		// TODO Auto-generated method stub
		return false;
	}
	
	@Override
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX,
			float distanceY) {
		// TODO Auto-generated method stub
		//Log.d(TAG, String.format("onScroll %f %f", distanceX, distanceY));
		float deltx = -distanceX;
		

		float angle = (float) Math.toDegrees(Math.atan(deltx/(mCenterY-e2.getY())));
		mAngle += angle;
		calc();

		invalidate();
		return false;
	}

	@Override
	public void onLongPress(MotionEvent e) {
		// TODO Auto-generated method stub

	}

	@Override
	public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX,
			float velocityY) {
		// TODO Auto-generated method stub
		Log.d(TAG, String.format("onFling %f %f", velocityX, velocityY));
		float speed = velocityX;
		if(speed > 0 &&  speed < 4000)
			speed = 4000;
		if(speed < 0 && speed > -4000)
			speed = -4000;
		
		float angle = speed*mAngleSpace/4000;
		animFling(mAngle+angle,200);
			
	
		return false;
	}

	@Override
	public void onAnimationUpdate(ValueAnimator animation) {
		// TODO Auto-generated method stub
		//Log.d(TAG,String.format("onAnimationUpdate %f", mAngle));

	}

}
