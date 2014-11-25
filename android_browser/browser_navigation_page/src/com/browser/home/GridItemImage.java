package com.browser.home;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.GradientDrawable;
import android.util.AttributeSet;
import android.view.View;

public class GridItemImage extends View {
	private Drawable mBackground;
	private Drawable mForeground;
	public GridItemImage(Context context, AttributeSet attrs) {
		super(context, attrs);
		// TODO Auto-generated constructor stub
	}
	public void setDrawable(Drawable background,Drawable foreground)
	{
		mBackground = background;
		mForeground = foreground;
		this.invalidate();
	}
	@Override
	protected void onDraw(Canvas canvas) {
		// TODO Auto-generated method stub
		//super.onDraw(canvas);
		if(mBackground != null)
		{
			mBackground.setBounds(0, 0, getWidth(), getHeight());
			mBackground.draw(canvas);
		}
		
		if(mForeground != null)
		{
			mForeground.draw(canvas);
		
		}
	
	}

}
