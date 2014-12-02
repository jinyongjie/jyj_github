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
import android.view.animation.AccelerateInterpolator;
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

	private int mOffsetY = 500;
	// private int mOffsetX = 200;
	private float mAngle = 0;
	private int mScreenX = 600;
	private int mScreenY = 800;
	private int mAngleSpace = 10;

	public TabSwitch(Context context, AttributeSet attrs) {
		super(context, attrs);
		// TODO Auto-generated constructor stub
		mGestureDetector = new GestureDetector(context, this);

	}

	public float getAngle() {
		return mAngle;
	}

	public void setAngle(float angle) {
		mAngle = angle;
		Log.d(TAG,String.format("setAngle %f",angle));
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
		canvas.drawText("新建窗口", 400, 300, textPaint);
	}

	public void init() {

		DisplayMetrics dm = new DisplayMetrics();
		((Activity) getContext()).getWindowManager().getDefaultDisplay()
				.getMetrics(dm);
		mScreenX = dm.widthPixels;
		mScreenY = dm.heightPixels;
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

	public void calc() {
		for (int i = 0; i < mCount; i++) {

			float angle = mAngle + mAngleSpace * i;
			mMatrix[i] = new Matrix();
			mMatrix[i].setTranslate((mScreenX - mBitmap[i].getWidth()) / 2,
					mOffsetY);
			mMatrix[i].postRotate(angle, mScreenX / 2, mScreenY * 2);

		}
	}

	private ValueAnimator mAnim;

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
		/*
		 * mAngle = -mAngleSpace*index; calc(); invalidate();
		 */
		if (mAnim != null)
			mAnim.cancel();
		float dest = (float)(-mAngleSpace* index);
		mAnim = ObjectAnimator.ofFloat(this, "angle", mAngle,dest );
		mAnim.setDuration(200);
		//mAnim.setInterpolator(new AccelerateInterpolator());
		mAnim.addUpdateListener(this);
		mAnim.start();
		Log.d(TAG,String.format("begin anim %f %f", mAngle,dest));
	}

	private boolean mDraging = false;
	private float mLastX;
	private float mLastY;

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		// TODO Auto-generated method stub
		// return super.onTouchEvent(event);

		// return mGestureDetector.onTouchEvent(event);

		int action = event.getAction();
		switch (action) {
		case MotionEvent.ACTION_DOWN:
			mDraging = true;
			mLastX = event.getX();
			mLastY = event.getY();
			if (mAnim != null)
				mAnim.cancel();
			break;
		case MotionEvent.ACTION_UP:
			mDraging = false;
			restorePos();
			break;
		case MotionEvent.ACTION_MOVE:
			float deltx = event.getX() - mLastX;
			float deltY = event.getY() - mLastY;

			int angle = (int) (deltx * 90 / mScreenX);
			mAngle += angle;
			calc();
			this.invalidate();

			mLastX = event.getX();
			mLastY = event.getY();
			break;
		case MotionEvent.ACTION_CANCEL:
			mDraging = false;
			break;
		}
		return true;

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
		Log.d(TAG, String.format("onScroll %f %f", distanceX, distanceY));

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
		return false;
	}

	@Override
	public void onAnimationUpdate(ValueAnimator animation) {
		// TODO Auto-generated method stub
		Log.d(TAG,String.format("onAnimationUpdate %f", mAngle));

	}

}
