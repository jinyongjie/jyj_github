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

public class testSurfaceView extends SurfaceView implements
		SurfaceHolder.Callback {

	public SurfaceHolder holder;
	public Bitmap bp3;

	public testSurfaceView(Context context) {
		super(context);

		holder = this.getHolder();
		holder.addCallback(this);
		try {
			bp3 = getImageFromAssetsFile("jiu_dian.png");
		} catch (IOException e) {
			e.printStackTrace();
		}

	}

	Paint paint = new Paint();

	protected void onDraw() {

		Canvas canvas = holder.lockCanvas();
		canvas.drawColor(Color.WHITE);

		// 画原图
		canvas.drawBitmap(bp3, 350, 0, paint);

		// 设置Mask过滤器(模糊)
		BlurMaskFilter blurFilter = new BlurMaskFilter(20,
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
		paint.setColor(Color.BLUE);
		canvas.drawBitmap(shadowBitmap, 50, 0, paint);

		BlurMaskFilter blurFilter2 = new BlurMaskFilter(20,
				BlurMaskFilter.Blur.INNER);
		Paint shadowPaint2 = new Paint();
		shadowPaint.setMaskFilter(blurFilter2); // 为画笔设置过滤器
		Bitmap shadowBitmap2 = bp3.extractAlpha(shadowPaint2, null);
		paint.setColor(Color.YELLOW);
		canvas.drawBitmap(shadowBitmap2, 650, 0, paint);

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
		canvas.drawBitmap(shadowBitmap2, 50, 250, paint);
		paint.setShader(linearGradient);
		canvas.drawBitmap(shadowBitmap2, 650, 250, paint);
		paint.setShader(radialGradientShader);
		canvas.drawBitmap(shadowBitmap2, 350, 250, paint);

		if (canvas != null) {
			holder.unlockCanvasAndPost(canvas);
		}

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
		o.inSampleSize = 3;
		image = BitmapFactory.decodeResource(getContext().getResources(),
				R.drawable.image1,o);

		return image;
	}

}