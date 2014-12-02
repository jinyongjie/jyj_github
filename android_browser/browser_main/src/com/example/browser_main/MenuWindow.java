package com.example.browser_main;

import android.animation.ObjectAnimator;
import android.content.Context;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.util.AttributeSet;
import android.view.View;
import android.view.animation.AccelerateInterpolator;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.view.animation.AnimationSet;
import android.view.animation.AnimationUtils;
import android.view.animation.Transformation;
import android.view.animation.TranslateAnimation;
import android.widget.LinearLayout;

public class MenuWindow extends LinearLayout {

	public MenuWindow(Context context, AttributeSet attrs) {
		super(context, attrs);
		// TODO Auto-generated constructor stub
	}
	
	View mMenu;
	private int mUpDistance = 20 ;
	private int mShowDurationUp = 200;
	private int mShowDurationDown = 30;
	private int mHideDuration = 200;
	@Override
	protected void onFinishInflate() {
		// TODO Auto-generated method stub
		super.onFinishInflate();
		
		mMenu = findViewById(R.id.menu);

	
		mUpDistance = Helper.dp2px(getContext(), 20);
	}

	class MyShowAnimation extends TranslateAnimation
	{

		public MyShowAnimation(float fromXDelta, float toXDelta, float fromYDelta,
				float toYDelta) {
			super(fromXDelta, toXDelta, fromYDelta, toYDelta);
			// TODO Auto-generated constructor stub
		}

		@Override
		protected void applyTransformation(float interpolatedTime,
				Transformation t) {
			// TODO Auto-generated method stub
			super.applyTransformation(interpolatedTime, t);
			ColorDrawable d  = new ColorDrawable(Color.argb((int) (0x30*interpolatedTime), 0, 0, 0));
			setBackgroundDrawable(d);
			invalidate();
			if(interpolatedTime == 1.0)
			{
				
				TranslateAnimation animation = new TranslateAnimation(0,0,0,mUpDistance);
				animation.setDuration(mShowDurationDown);
				animation.setFillAfter(true);
				mMenu.startAnimation(animation);
			}
		}
		
	}
	class MyHideAnimation extends TranslateAnimation
	{

		public MyHideAnimation(float fromXDelta, float toXDelta, float fromYDelta,
				float toYDelta) {
			super(fromXDelta, toXDelta, fromYDelta, toYDelta);
			// TODO Auto-generated constructor stub
		}

		@Override
		protected void applyTransformation(float interpolatedTime,
				Transformation t) {
			// TODO Auto-generated method stub
			super.applyTransformation(interpolatedTime, t);
			ColorDrawable d  = new ColorDrawable(Color.argb((int) (0x30*(1.0-interpolatedTime)), 0, 0, 0));
			setBackgroundDrawable(d);
			
			
			if(interpolatedTime == 1.0)
				setVisibility(View.GONE);
			invalidate();
		}
		
	}
	public void animShow()
	{
		setVisibility(View.VISIBLE);
		mMenu.measure(this.getWidth(), this.getHeight());
		int height = mMenu.getMeasuredHeight() ;
		MyShowAnimation animation = new MyShowAnimation(0,0,height,0);
		animation.setDuration(mShowDurationUp);
		animation.setInterpolator(new AccelerateInterpolator());
		
		
		mMenu.startAnimation(animation);

		
	}
	public void animHide()
	{
		
		mMenu.measure(this.getWidth(), this.getHeight());
		int height = mMenu.getMeasuredHeight();
		MyHideAnimation animation = new MyHideAnimation(0,0,mUpDistance,height);
		animation.setDuration(mHideDuration);
		animation.setFillAfter(true);
		
		mMenu.startAnimation(animation);
		
	}

}
