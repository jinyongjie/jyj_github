package com.example.browser_main;

import java.util.ArrayList;

import com.browser.home.HomePage;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PorterDuff;
import android.graphics.PorterDuffXfermode;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.PorterDuff.Mode;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

import android.view.View.OnClickListener;

import android.view.ViewGroup;

public class MainActivity extends Activity implements OnClickListener {
	private MenuWindow2 mMenuWindow;
	private ViewGroup mContainer;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
	

	//	View v = new ImageEffect(this);
		
	//	setContentView(v);

		setContentView(R.layout.activity_main);
		
		//initTestRotate();
		//testPopup();
		findViewById(R.id.button_menu).setOnClickListener(this);
		findViewById(R.id.button_tab).setOnClickListener(this);
		mMenuWindow = (MenuWindow2) findViewById(R.id.menu_widow);
		mContainer = (ViewGroup)findViewById(R.id.container);
		
		initHomePage();

	}

	private void initHomePage()
	{
		HomePage page = new HomePage(this,null);
		mContainer.addView(page);
		page.init();
	}
	private void initTestRotate()
	{
		TabSwitch v = (TabSwitch) findViewById(R.id.tab_switch);
		
		v.init();
	}
	
	class ImageEffect extends View {
		Paint paint;

		public ImageEffect(Context context) {
			super(context);
			paint = new Paint(); // 初始化画笔，为后面阴影效果使用。
			paint.setAntiAlias(true);// 去除锯齿。
			paint.setShadowLayer(5f, 5.0f, 5.0f, Color.BLACK); // 设置阴影层，这是关键。
			paint.setXfermode(new PorterDuffXfermode(Mode.SRC_IN));

		}

		public void onDraw(Canvas canvas) {
			super.onDraw(canvas);
			int posX = 20;
			int posY = 50;
			int PicWidth, PicHeight;

			Drawable drawable = getResources().getDrawable(R.drawable.ic_launcher);
			Drawable dbe = getResources().getDrawable(R.drawable.ic_launcher)
					.mutate();// 如果不调用mutate方法，则原图也会被改变，因为调用的资源是同一个，所有对象是共享状态的。
			Drawable drawTest = getResources().getDrawable(R.drawable.ic_launcher);
			Bitmap bmp = BitmapFactory.decodeResource(getResources(),
					R.drawable.ic_launcher);
			PicWidth = drawable.getIntrinsicWidth();
			PicHeight = drawable.getIntrinsicHeight();

			drawTest.setBounds(posX, (2 * posY) + PicHeight, posX + PicWidth,
					(2 * posY) + 2 * PicHeight);
			drawable.setBounds(posX, posY, posX + PicWidth, posY + PicHeight);
			dbe.setBounds(0, 0, PicWidth, PicHeight);

			canvas.drawColor(Color.WHITE);// 设置画布颜色
			canvas.save(Canvas.MATRIX_SAVE_FLAG);
			dbe.setColorFilter(0x7f000000, PorterDuff.Mode.SRC_IN);
			canvas.translate(posX + (int) (0.9 * PicWidth / 2), posY
					+ PicHeight / 2);// 图像平移为了刚好在原图后形成影子效果。
			canvas.skew(-0.9F, 0.0F);// 图像倾斜效果。
			canvas.scale(1.0f, 0.5f);// 图像（其实是画布）缩放，Y方向缩小为1/2。
			dbe.draw(canvas);// 此处为画原图像影子效果图，比原图先画，则会在下层。
			drawable.clearColorFilter();
			canvas.restore();

			canvas.save(Canvas.MATRIX_SAVE_FLAG);
			drawable.draw(canvas);// 此处为画原图像，由于canvas有层次效果，因此会盖在影子之上。
			canvas.restore();

			// 默认无效果原图
			canvas.save(Canvas.MATRIX_SAVE_FLAG);
			drawTest.draw(canvas);
			canvas.restore();

			// 图片阴影效果
			Rect rect = new Rect(2 * posX + PicWidth + 3, 2 * posY + PicHeight
					+ 3, 2 * posX + 2 * PicWidth - 2, 2 * posY + 2 * PicHeight
					- 2);
			// 由于图片的实际尺寸比显示出来的图像要大一些，因此需要适当更改下大小，以达到较好的效果

			RectF rectF = new RectF(rect);
			canvas.drawRoundRect(rectF, 10f, 10f, paint);// 在原有矩形基础上，画成圆角矩形，同时带有阴影层。
			canvas.drawBitmap(bmp, 2 * posX + PicWidth, 2 * posY + PicHeight,
					null);// 画上原图。
			canvas.restore();
		}
	}


	@Override
	public void onClick(View v) {	
		// TODO Auto-generated method stub
		int id= v.getId();
		switch(id)
		{
		case R.id.button_menu:
			popup_menuwindow();
			break;
		case R.id.button_tab:
			Intent intent = new Intent();
			intent.setClass(this, TabSwitchActivity.class);
			startActivity(intent);
			break;
		}
	}
	private void popup_menuwindow()
	{
		/*
		if(mMenuWindow.getVisibility() == View.VISIBLE)
		{
			//mMenuWindow.setVisibility(View.GONE);
			mMenuWindow.animHide();
		}
		else
		{
			mMenuWindow.animShow();
			//mMenuWindow.setVisibility(View.VISIBLE);
		}
		*/
	}
}
