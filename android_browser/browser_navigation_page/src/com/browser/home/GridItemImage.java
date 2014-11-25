package com.browser.home;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.PorterDuff.Mode;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.GradientDrawable;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

public class GridItemImage extends View {
	private Drawable mBackground;
	private Bitmap mBitmap;
	public GridItemImage(Context context, AttributeSet attrs) {
		super(context, attrs);
		// TODO Auto-generated constructor stub
	}
	public void setDrawable(Drawable background,Bitmap bmp)
	{
		mBackground = background;
		mBitmap = bmp;
		this.invalidate();

	}
	@Override
	protected void onDraw(Canvas canvas) {
		// TODO Auto-generated method stub
		//super.onDraw(canvas);
		if(mPressed)
		{
			canvas.save();
			canvas.scale(0.7f, 0.7f);
			canvas.translate(getWidth()*0.3f, (getHeight()*0.3f));
		}
		if(mBackground != null)
		{
			mBackground.setBounds(0, 0, getWidth(), getHeight());
			mBackground.draw(canvas);
		}
		
		if(mBitmap != null)
		{
			
			canvas.drawBitmap(mBitmap, (getWidth() - mBitmap.getWidth())/2, (getHeight() - mBitmap.getHeight())/2, null);
		}
		if(mPressed)
		{
			canvas.restore();
		}
	
	}
	private boolean mPressed = false;



	public boolean onFilterTouchEventForSecurity(MotionEvent paramMotionEvent) {
		if (paramMotionEvent.getAction() == 1)
			setPressed(false);
		return super.onFilterTouchEventForSecurity(paramMotionEvent);
	}

	public void setPressed(boolean paramBoolean) {
		
		if ( this.mPressed != paramBoolean) {
			this.mPressed = paramBoolean;
	
			invalidate();
		}
		super.setPressed(paramBoolean);
	}
}
