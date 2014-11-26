package com.example.browser_main;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.drawable.BitmapDrawable;
import android.view.View;

public class TestRotate extends View {
	private Bitmap[] mBitmap = new Bitmap[4];
	private Matrix[] mMatrix = new Matrix[4];
	private int[] mIds = {R.drawable.image1,R.drawable.image2,R.drawable.image3,R.drawable.image4};
	public TestRotate(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
	}

	@Override
	protected void onDraw(Canvas canvas) {
		// TODO Auto-generated method stub
		//super.onDraw(canvas);
		for(int i= 0;i<4;i++)
		canvas.drawBitmap(mBitmap[i], mMatrix[i], null);
	}

	public void init() {
		
		BitmapFactory.Options options = new BitmapFactory.Options();
		options.inSampleSize = 2;
		
		for(int i= 0;i<4;i++)
		{
			mBitmap[i]  = BitmapFactory.decodeResource(getResources(),
					mIds[i],options);
			mMatrix[i] = new Matrix();
			mMatrix[i].setTranslate(200, 500);
			mMatrix[i].postRotate(-45 + 30*i, (float) mBitmap[i].getWidth()/2 + 200 , (float) mBitmap[i].getHeight() + 500);
			
		}

	}
}
