package com.example.browser_main;

import java.io.IOException;
import java.io.InputStream;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.BlurMaskFilter;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.ColorMatrix;
import android.graphics.ColorMatrixColorFilter;
import android.graphics.LinearGradient;
import android.graphics.Paint;
import android.graphics.RadialGradient;
import android.graphics.Region;
import android.graphics.Shader;
import android.graphics.SweepGradient;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

public class testSurfaceView extends View implements
		SurfaceHolder.Callback {

	
	public Bitmap bp3;

	public testSurfaceView(Context context) {
		super(context);

		

		try {
			bp3 = getImageFromAssetsFile("jiu_dian.png");
		} catch (IOException e) {
			e.printStackTrace();
		}

	}

	Paint paint = new Paint();

	@Override
	protected void onDraw(Canvas canvas) {


		canvas.drawColor(Color.WHITE);

		// 画原图
		canvas.drawBitmap(bp3, 350, 10, paint);

		// 设置Mask过滤器(模糊)
		int shadow_width = 10;
		BlurMaskFilter blurFilter = new BlurMaskFilter(shadow_width,
				BlurMaskFilter.Blur.OUTER);
		// BlurMaskFilter blurFilter = new BlurMaskFilter(20,
		// BlurMaskFilter.Blur.NORMAL);
		// BlurMaskFilter blurFilter = new BlurMaskFilter(20,
		// BlurMaskFilter.Blur.INNER);
		// BlurMaskFilter blurFilter = new BlurMaskFilter(20,
		// BlurMaskFilter.Blur.SOLID);
		Paint shadowPaint = new Paint();
		shadowPaint.setMaskFilter(blurFilter); // 为画笔设置过滤器
		Bitmap shadowBitmap = bp3.extractAlpha(shadowPaint, null);
		paint.setColor(Color.BLACK);
		canvas.drawBitmap(shadowBitmap, 50, 10, paint);
		
		canvas.drawBitmap(bp3, 50+shadow_width, 10+shadow_width, paint);

		BlurMaskFilter blurFilter2 = new BlurMaskFilter(20,
				BlurMaskFilter.Blur.INNER);
		Paint shadowPaint2 = new Paint();
		shadowPaint.setMaskFilter(blurFilter2); // 为画笔设置过滤器
		Bitmap shadowBitmap2 = bp3.extractAlpha(shadowPaint2, null);
		paint.setColor(Color.YELLOW);
		canvas.drawBitmap(shadowBitmap2, 650, 10, paint);

		// 三种shader方式
		RadialGradient radialGradientShader = new RadialGradient(10, 10, 2,
				new int[] { Color.RED, Color.YELLOW, Color.BLUE }, null,
				Shader.TileMode.MIRROR);
		SweepGradient sweepGradient = new SweepGradient(10, 10, new int[] {
				Color.RED, Color.YELLOW, Color.BLUE }, null);
		LinearGradient linearGradient = new LinearGradient(0, 0, 150, 70,
				new int[] { Color.RED, Color.GREEN, Color.BLUE }, null,
				Shader.TileMode.MIRROR);

		paint.setShader(sweepGradient);
		canvas.drawBitmap(shadowBitmap2, 50, 500, paint);
		paint.setShader(linearGradient);
		canvas.drawBitmap(shadowBitmap2, 650, 500, paint);
		paint.setShader(radialGradientShader);
		canvas.drawBitmap(shadowBitmap2, 350, 500, paint);



	}

	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		// TODO Auto-generated method stub
		// onDraw();

	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width,
			int height) {
		// TODO Auto-generated method stub

	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		// TODO Auto-generated method stub

	}

	public Bitmap getImageFromAssetsFile(String fileName) throws IOException {
		Bitmap image = null;
		BitmapFactory.Options o = new BitmapFactory.Options();
		o.inSampleSize = 8;
		image = BitmapFactory.decodeResource(getContext().getResources(),
				R.drawable.image1,o);

		return image;
	}

}