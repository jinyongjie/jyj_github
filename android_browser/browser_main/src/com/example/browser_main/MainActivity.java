package com.example.browser_main;

import com.browser.home.HomePage;

import android.app.Activity;
import android.content.Context;
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
import android.view.ViewGroup;

public class MainActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		ViewGroup root = (ViewGroup) LayoutInflater.from(this).inflate(R.layout.activity_main, null);
		setContentView(root);
		initTestRotate();

	//	View v = new ImageEffect(this);
		
	//	setContentView(v);
	}

	private void testHomePage()
	{
		HomePage page = new HomePage(this,null);
		setContentView(page);
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
			paint.setAntiAlias(true);// ȥ����ݡ�
			paint.setShadowLayer(5f, 5.0f, 5.0f, Color.BLACK); // ������Ӱ�㣬���ǹؼ���
			paint.setXfermode(new PorterDuffXfermode(Mode.SRC_IN));

		}

		public void onDraw(Canvas canvas) {
			super.onDraw(canvas);
			int posX = 20;
			int posY = 50;
			int PicWidth, PicHeight;

			Drawable drawable = getResources().getDrawable(R.drawable.ic_launcher);
			Drawable dbe = getResources().getDrawable(R.drawable.ic_launcher)
					.mutate();// ���������mutate��������ԭͼҲ�ᱻ�ı䣬��Ϊ���õ���Դ��ͬһ�������ж����ǹ���״̬�ġ�
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
					+ PicHeight / 2);// ͼ��ƽ��Ϊ�˸պ���ԭͼ���γ�Ӱ��Ч����
			canvas.skew(-0.9F, 0.0F);// ͼ����бЧ����
			canvas.scale(1.0f, 0.5f);// ͼ����ʵ�ǻ��������ţ�Y������СΪ1/2��
			dbe.draw(canvas);// �˴�Ϊ��ԭͼ��Ӱ��Ч��ͼ����ԭͼ�Ȼ���������²㡣
			drawable.clearColorFilter();
			canvas.restore();

			canvas.save(Canvas.MATRIX_SAVE_FLAG);
			drawable.draw(canvas);// �˴�Ϊ��ԭͼ������canvas�в��Ч������˻����Ӱ��֮�ϡ�
			canvas.restore();

			// Ĭ����Ч��ԭͼ
			canvas.save(Canvas.MATRIX_SAVE_FLAG);
			drawTest.draw(canvas);
			canvas.restore();

			// ͼƬ��ӰЧ��
			Rect rect = new Rect(2 * posX + PicWidth + 3, 2 * posY + PicHeight
					+ 3, 2 * posX + 2 * PicWidth - 2, 2 * posY + 2 * PicHeight
					- 2);
			// ����ͼƬ��ʵ�ʳߴ����ʾ������ͼ��Ҫ��һЩ�������Ҫ�ʵ������´�С���Դﵽ�Ϻõ�Ч��

			RectF rectF = new RectF(rect);
			canvas.drawRoundRect(rectF, 10f, 10f, paint);// ��ԭ�о��λ����ϣ�����Բ�Ǿ��Σ�ͬʱ������Ӱ�㡣
			canvas.drawBitmap(bmp, 2 * posX + PicWidth, 2 * posY + PicHeight,
					null);// ����ԭͼ��
			canvas.restore();
		}
	}
}
