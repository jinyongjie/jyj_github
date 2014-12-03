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
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;

public class MainActivity extends Activity implements OnClickListener {
	private MenuWindow mMenuWindow;
	private ViewGroup mContainer;
	private ViewGroup mRoot;
	private View mTabSwitch;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
	
		LayoutInflater inflater = LayoutInflater.from(this);
		mRoot = (ViewGroup) inflater.inflate(R.layout.activity_main, null);
		setContentView(mRoot);
		

		findViewById(R.id.button_menu).setOnClickListener(this);
		findViewById(R.id.button_tab).setOnClickListener(this);
		findViewById(R.id.button_back).setOnClickListener(this);
		mMenuWindow = (MenuWindow) findViewById(R.id.menu_widow);
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
			paint = new Paint(); // ��ʼ�����ʣ�Ϊ������ӰЧ��ʹ�á�
			paint.setAntiAlias(true);// ȥ���ݡ�
			paint.setShadowLayer(5f, 5.0f, 5.0f, Color.BLACK); // ������Ӱ�㣬���ǹؼ�
			paint.setXfermode(new PorterDuffXfermode(Mode.SRC_IN));

		}

		public void onDraw(Canvas canvas) {
			super.onDraw(canvas);
			int posX = 20;
			int posY = 50;
			int PicWidth, PicHeight;

			Drawable drawable = getResources().getDrawable(R.drawable.ic_launcher);
			Drawable dbe = getResources().getDrawable(R.drawable.ic_launcher)
					.mutate();// ������mutate��������ԭͼҲ�ᱻ�ı䣬��Ϊ���õ���Դ��ͬһ�������ж����ǹ���״̬�ġ�
			Drawable drawTest = getResources().getDrawable(R.drawable.ic_launcher);
			Bitmap bmp = BitmapFactory.decodeResource(getResources(),
					R.drawable.ic_launcher);
			PicWidth = drawable.getIntrinsicWidth();
			PicHeight = drawable.getIntrinsicHeight();

			drawTest.setBounds(posX, (2 * posY) + PicHeight, posX + PicWidth,
					(2 * posY) + 2 * PicHeight);
			drawable.setBounds(posX, posY, posX + PicWidth, posY + PicHeight);
			dbe.setBounds(0, 0, PicWidth, PicHeight);

			canvas.drawColor(Color.WHITE);// ���û�����ɫ
			canvas.save(Canvas.MATRIX_SAVE_FLAG);
			dbe.setColorFilter(0x7f000000, PorterDuff.Mode.SRC_IN);
			canvas.translate(posX + (int) (0.9 * PicWidth / 2), posY
					+ PicHeight / 2);// ͼ��ƽ��Ϊ�˸պ���ԭͼ���γ�Ӱ��Ч��
			canvas.skew(-0.9F, 0.0F);// ͼ����бЧ��
			canvas.scale(1.0f, 0.5f);// ͼ����ʵ�ǻ��������ţ�Y������СΪ1/2��
			dbe.draw(canvas);// �˴�Ϊ��ԭͼ��Ӱ��Ч��ͼ����ԭͼ�Ȼ���������²㡣
			drawable.clearColorFilter();
			canvas.restore();

			canvas.save(Canvas.MATRIX_SAVE_FLAG);
			drawable.draw(canvas);// �˴�Ϊ��ԭͼ������canvas�в��Ч����˻����Ӱ��֮�ϡ�
			canvas.restore();

			// Ĭ����Ч��ԭͼ
			canvas.save(Canvas.MATRIX_SAVE_FLAG);
			drawTest.draw(canvas);
			canvas.restore();

			// ͼƬ��ӰЧ��
			Rect rect = new Rect(2 * posX + PicWidth + 3, 2 * posY + PicHeight
					+ 3, 2 * posX + 2 * PicWidth - 2, 2 * posY + 2 * PicHeight
					- 2);
			// ����ͼƬ��ʵ�ʳߴ����ʾ������ͼ��Ҫ��һЩ�������Ҫ�ʵ�����´�С���Դﵽ�Ϻõ�Ч��

			RectF rectF = new RectF(rect);
			canvas.drawRoundRect(rectF, 10f, 10f, paint);// ��ԭ�о��λ��ϣ�����Բ�Ǿ��Σ�ͬʱ������Ӱ�㡣
			canvas.drawBitmap(bmp, 2 * posX + PicWidth, 2 * posY + PicHeight,
					null);// ����ԭͼ��
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
		/*	Intent intent = new Intent();
			intent.setClass(this, TabSwitchActivity.class);
			startActivity(intent);
			*/showTabSwitch();
			break;
		case R.id.button_back:
			
			Intent intent = new Intent();
			intent.setClass(this, ViewTest.class);
			this.startActivity(intent);
		
			break;
		}
	}
	private void popup_menuwindow()
	{
		
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
		
	}
	private void showTabSwitch()
	{
		if(mTabSwitch == null)
		{
			LayoutInflater inflater = LayoutInflater.from(this);
			mTabSwitch =  inflater.inflate(R.layout.tab_switch, null);
			mRoot.addView(mTabSwitch);
			TabSwitch t = (TabSwitch) mTabSwitch.findViewById(R.id.tab_switch);
			t.init();

		}
		mTabSwitch.setVisibility(View.VISIBLE);
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		// TODO Auto-generated method stub
		if(keyCode == event.KEYCODE_BACK)
		{
			if(mTabSwitch != null && mTabSwitch.getVisibility() == View.VISIBLE)
			{
				mTabSwitch.setVisibility(View.GONE);
				return true;
			}
		}
		return super.onKeyDown(keyCode, event);
	}
}
