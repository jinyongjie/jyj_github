package com.example.browser_main;

import java.util.ArrayList;

import java.util.List;

import android.content.Context;

import android.graphics.Bitmap;

import android.graphics.Bitmap.Config;

import android.graphics.BitmapFactory;

import android.graphics.BlurMaskFilter;

import android.graphics.Canvas;

import android.graphics.Color;

import android.graphics.Matrix;

import android.graphics.NinePatch;

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

public class TabSwitchRotateCtrl extends View implements OnGestureListener,

AnimatorUpdateListener {

	private int mCurrentIndex = 0;

	static private String TAG = "TabSwitch";

	private GestureDetector mGestureDetector;

	// private int mOffsetX = 200;

	private float mAngle = 0;

	private int mAngleSpace = 10;

	private float mCenterX = 0;

	private float mCenterY = 0;

	private int mBitmapOffsetY;
	private int mTitleOffsetY;

	private int mBottomLineHeight = 2;

	private int mBottomBarHeight = 20;

	private int mBottomShadowHeight = 3;

	private Bitmap mBottomShadow;

	private Bitmap mShadow;

	private Bitmap mClose;

	private List<Data> mData = new ArrayList<Data>();

	private Observer mObserver;

	private List<Bitmap> mBmps;

	private List<String> mTitles;

	public TabSwitchRotateCtrl(Context context, AttributeSet attrs) {

		super(context, attrs);

		// TODO Auto-generated constructor stub

		mGestureDetector = new GestureDetector(context, this);

	}

	public void setObserver(Observer observer)

	{

		mObserver = observer;

	}

	interface Observer

	{

		void onSwitch(int index);

		void onClose(int index);

	}

	class Data

	{

		Bitmap bmp;

		String title;

		Matrix matrix;

	}

	@Override
	protected void onLayout(boolean changed, int left, int top, int right,

	int bottom) {

		// TODO Auto-generated method stub

		super.onLayout(changed, left, top, right, bottom);

		if (changed)

		{

			updateBmps();

		}

	}

	public void setData(List<Bitmap> bmps, List<String> titles, int index) {

		mData.clear();

		/*
		 * 
		 * DisplayMetrics dm = new DisplayMetrics();
		 * 
		 * ((Activity) getContext()).getWindowManager().getDefaultDisplay()
		 * 
		 * .getMetrics(dm);
		 */

		mBmps = bmps;

		mTitles = titles;

		mCurrentIndex = index;

		updateBmps();

	}

	private void updateBmps() {
		int view_width = getWidth();

		int view_height = getHeight();
		if (view_width == 0 || view_height == 0)
			return;
		mData.clear();

		mBitmapOffsetY = (int) (view_height - view_width * 1.5 / 2);
		mTitleOffsetY = mBitmapOffsetY - Helper.dp2px(getContext(), 30);

		mCenterX = view_width / 2;

		mCenterY = (float) (view_width * 3.5);

		mBottomLineHeight = Helper.dp2px(getContext(), 1);

		mBottomBarHeight = Helper.dp2px(getContext(), 8);

		mBottomShadowHeight = Helper.dp2px(getContext(), 3);

		BitmapFactory.Options options = new BitmapFactory.Options();

		options.inSampleSize = 1;

		mBottomShadow = BitmapFactory.decodeResource(getResources(),

		R.drawable.multi_tab_bottom_shadow);

		mShadow = BitmapFactory.decodeResource(getResources(),

		R.drawable.multi_tab_shadow);

		mClose = BitmapFactory.decodeResource(getResources(),
				R.drawable.multi_tab_close);

		for (int i = 0; i < mBmps.size(); i++) {

			Data data = new Data();

			Bitmap bmp = mBmps.get(i);

			data.title = mTitles.get(i);

			float sx = (float) (view_width / (2.0 * bmp.getWidth()));

			int width = (int) (view_width / 2.0);

			int height = (int) (bmp.getHeight() * sx);

			Rect rcNine = new Rect(30, 20, 32, 41);

			Bitmap newbmp = Bitmap.createBitmap(width + rcNine.left
					+ rcNine.right,

			height + rcNine.top + rcNine.bottom, Config.ARGB_8888);

			Canvas cv = new Canvas(newbmp);

			RectF rectf = new RectF(0, 0, newbmp.getWidth(), newbmp.getHeight());

			NinePatch np = new NinePatch(mShadow, mShadow.getNinePatchChunk(),

			null);

			np.draw(cv, rectf);

			Rect src = new Rect(0, 0, bmp.getWidth(), bmp.getHeight());

			Rect dst = new Rect(rcNine.left, rcNine.top, width + rcNine.left,
					height + rcNine.top);

			cv.drawBitmap(bmp, src, dst, null);

			cv.drawBitmap(mClose, rcNine.left + width - mClose.getWidth(),
					rcNine.top, null);

			// cv.save(Canvas.ALL_SAVE_FLAG);

			// store

			// cv.restore();

			data.bmp = newbmp;

			mData.add(data);

		}
		setCurrentIndex(mCurrentIndex);
	}

	public float getAngle() {

		return mAngle;

	}

	public void setAngle(float angle) {

		mAngle = angle;

		// Log.d(TAG,String.format("setAngle %f",angle));

		calc();

		invalidate();

	}

	public void setCurrentIndex(int index)

	{

		mCurrentIndex = index;

		float angle = -mAngleSpace * index;

		setAngle(angle);

	}

	@Override
	protected void onDraw(Canvas canvas) {

		// TODO Auto-generated method stub

		// super.onDraw(canvas);
		if (mData.size() == 0)
			return;

		PaintFlagsDrawFilter pfd = new PaintFlagsDrawFilter(0,

		Paint.ANTI_ALIAS_FLAG | Paint.FILTER_BITMAP_FLAG);

		canvas.setDrawFilter(pfd);

		Paint paint = new Paint();

		// paint.setShadowLayer(5, 8, 7, Color.DKGRAY);

		BlurMaskFilter blurFilter = new BlurMaskFilter(3,

		BlurMaskFilter.Blur.NORMAL);

		paint.setAlpha(50);

		paint.setColor(Color.GRAY);

		paint.setMaskFilter(blurFilter);

		// paint.setBitmapFilter(true);

		for (int i = 0; i < mData.size(); i++) {

			Data data = mData.get(i);

			canvas.drawBitmap(data.bmp, data.matrix, paint);

			Paint textPaint = new Paint(Paint.ANTI_ALIAS_FLAG);

			textPaint.setTextSize(50);

			textPaint.setColor(Color.WHITE);

			float textW = (int) textPaint.measureText(data.title);

			float angle = mAngle + mAngleSpace * i;

			float left = (float) (Math.tan(Math.toRadians(angle)) * getWidth() * 8)

					+ getWidth() / 2 - textW / 2;

			canvas.drawText(data.title, left, mTitleOffsetY, textPaint);

		}

		Paint textPaint = new Paint(Paint.ANTI_ALIAS_FLAG);

		textPaint.setColor(Color.rgb(123, 148, 174));

		Rect r = new Rect(0, getHeight() - mBottomBarHeight, getWidth(),

		getHeight());

		canvas.drawRect(r, textPaint);

		Rect src = new Rect(0, 0, mBottomShadow.getWidth(),

		mBottomShadow.getHeight());

		Rect dst = new Rect(0, getHeight() - mBottomBarHeight

		- mBottomShadowHeight, getWidth(), getHeight() - mBottomBarHeight);

		canvas.drawBitmap(mBottomShadow, src, dst, null);

		textPaint.setColor(Color.rgb(134, 157, 181));

		Rect r1 = new Rect(0, getHeight() - mBottomLineHeight, getWidth(),

		getHeight());

		canvas.drawRect(r1, textPaint);

		int w = getWidth() / mData.size();

		textPaint.setColor(Color.WHITE);

		int x = (int) (-getWidth() * mAngle / (mAngleSpace * mData.size()));

		Rect r2 = new Rect(x, getHeight() - mBottomLineHeight, x + w,

		getHeight());

		canvas.drawRect(r2, textPaint);

	}

	public void calc() {

		for (int i = 0; i < mData.size(); i++) {

			Data data = mData.get(i);

			float angle = mAngle + mAngleSpace * i;

			data.matrix = new Matrix();

			data.matrix.setTranslate((getWidth() - data.bmp.getWidth()) / 2,

			mBitmapOffsetY);

			data.matrix.postRotate(angle, mCenterX, mCenterY);

		}

	}

	private ValueAnimator mRestoreAnim;

	private ValueAnimator mFlingAnim;

	private void restorePos() {

		int index = 0;

		float minangle = 360;

		for (int i = 0; i < mData.size(); i++) {

			float angle = mAngle + mAngleSpace * i;

			if (Math.abs(angle) < Math.abs(minangle)) {

				index = i;

				minangle = angle;

			}

		}

		mCurrentIndex = index;

		/*
		 * 
		 * mAngle = -mAngleSpace*index; calc(); invalidate();
		 */

		if (mRestoreAnim != null)

			mRestoreAnim.cancel();

		float dest = (float) (-mAngleSpace * index);

		animRestore(dest);

		// Log.d(TAG,String.format("begin anim %f %f", mAngle,dest));

	}

	private void cancelAllAnim() {

		if (mRestoreAnim != null && mRestoreAnim.isRunning())

			mRestoreAnim.cancel();

		if (mFlingAnim != null && mFlingAnim.isRunning())

			mFlingAnim.cancel();

	}

	private boolean isAnimRunning()

	{

		if (mRestoreAnim != null && mRestoreAnim.isRunning())

			return true;

		if (mFlingAnim != null && mFlingAnim.isRunning())

			return true;

		return false;

	}

	private void animRestore(float dest) {

		cancelAllAnim();

		mRestoreAnim = ObjectAnimator.ofFloat(this, "angle", mAngle, dest);

		mRestoreAnim.setDuration(100);

		mRestoreAnim.addUpdateListener(this);

		mRestoreAnim.start();

	}

	private void animFling(float dest, long duration) {

		cancelAllAnim();

		mFlingAnim = ObjectAnimator.ofFloat(this, "angle", mAngle, dest);

		mFlingAnim.setInterpolator(new DecelerateInterpolator());

		mFlingAnim.setDuration(duration);

		mFlingAnim.addUpdateListener(this);

		mFlingAnim.addListener(new AnimatorListener() {

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

		// if(isAnimRunning())

		// return false;

		int x = (int) e.getX();

		int y = (int) e.getY();

		Bitmap bmp = mData.get(mCurrentIndex).bmp;

		if (y < mBitmapOffsetY)

			return false;

		int offsetX = getWidth() / 2;

		int left = (offsetX - bmp.getWidth() / 2);

		int right = (offsetX + bmp.getWidth() / 2);
		int close_width = mClose.getWidth();
		if (x > left && x < right)

		{

			// Log.d(TAG,"singletab 0");

			if (mObserver != null)
			{
				Rect rcClose = new Rect(right - close_width,mBitmapOffsetY,right,mBitmapOffsetY + close_width);
				if(rcClose.contains(x, y))
				{
					mObserver.onClose(mCurrentIndex);
				}
				else
				{
					mObserver.onSwitch(mCurrentIndex);
				}
				
			}
				

		}

		else if (x < left)

		{

			if (mObserver != null && mCurrentIndex - 1 >= 0)
			{
				Rect rcClose = new Rect(left - close_width,mBitmapOffsetY, left,mBitmapOffsetY + close_width);
				if(rcClose.contains(x, y))
				{
					mObserver.onClose(mCurrentIndex);
				}
				else
				{
					mObserver.onSwitch(mCurrentIndex -1);
				}
				
			
			}
			

			// Log.d(TAG,"singletab -1");

		}

		else if (x > right)

		{

			if (mObserver != null && mCurrentIndex + 1 < mData.size())
			{

				{
					mObserver.onSwitch(mCurrentIndex + 1);
				}
				
			}


			// Log.d(TAG,"singletab 1");

		}

		return false;

	}

	@Override
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX,

	float distanceY) {

		// TODO Auto-generated method stub

		// Log.d(TAG, String.format("onScroll %f %f", distanceX, distanceY));

		float deltx = -distanceX;

		float angle = (float) Math.toDegrees(Math.atan(deltx

		/ (mCenterY - e2.getY())));

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

		float speed = velocityX;

		int titem = 2000;

		if (speed > 0 && speed < titem)

			speed = titem;

		if (speed < 0 && speed > -titem)

			speed = -titem;

		float angle = mAngleSpace * (speed / titem);

		if (Math.abs(speed) >= 3000) {

			angle = angle * (speed / 3000);

			if (speed < 0)

				angle = -angle;

		}

		float angle_max = 0;

		if (angle > 0) {

			angle_max = (float) ((mCurrentIndex + 0.5) * mAngleSpace);

			if (angle > angle_max)

				angle = angle_max;

		} else {

			angle_max = (float) (-(mData.size() - mCurrentIndex - 0.5) * mAngleSpace);

			if (angle < angle_max)

				angle = angle_max;

		}

		animFling(mAngle + angle, 100);

		Log.d(TAG, String.format("onFling x=%f angle=%f max=%f", velocityX,

		angle, angle_max));

		return false;

	}

	@Override
	public void onAnimationUpdate(ValueAnimator animation) {

		// TODO Auto-generated method stub

		// Log.d(TAG,String.format("onAnimationUpdate %f", mAngle));

	}

}
