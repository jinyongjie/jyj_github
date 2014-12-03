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


import android.view.View.OnClickListener;
import android.app.Activity;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;

public class ViewTest extends Activity implements OnClickListener{
    /** Called when the activity is first created. */
	FrameLayout mFrame1;
	ImageView mImageView;
	Button mButton;
	@Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.test_surface);
        mFrame1 = (FrameLayout) findViewById(R.id.frame1);
        mImageView = (ImageView) findViewById(R.id.image);
        mButton = (Button) findViewById(R.id.button);
        mButton.setOnClickListener(this);
        
        MyView v =new MyView(this);
        mFrame1.addView(v);
    }
    //视图内部类
    class MyView extends SurfaceView implements SurfaceHolder.Callback
    {
        private SurfaceHolder holder;
        private MyThread myThread; 
        public MyView(Context context) {
            super(context);
            // TODO Auto-generated constructor stub
            holder = this.getHolder();
            holder.addCallback(this);
            myThread = new MyThread(holder);//创建一个绘图线程
        }

        @Override
        public void surfaceChanged(SurfaceHolder holder, int format, int width,
                int height) {
            // TODO Auto-generated method stub
            
        }

        @Override
        public void surfaceCreated(SurfaceHolder holder) {
            // TODO Auto-generated method stub
            myThread.isRun = true;
            myThread.start();
        }

        @Override
        public void surfaceDestroyed(SurfaceHolder holder) {
            // TODO Auto-generated method stub
            myThread.isRun = false;
        }
        //线程内部类
        class MyThread extends Thread
        {
            private SurfaceHolder holder;
            public boolean isRun ;
            public  MyThread(SurfaceHolder holder)
            {
                this.holder =holder; 
                isRun = true;
            }
            @Override
            public void run()
            {
                int count = 0;
                while(isRun)
                {
                    Canvas c = null;
                    try
                    {
                        synchronized (holder)
                        {
                            c = holder.lockCanvas();//锁定画布，一般在锁定后就可以通过其返回的画布对象Canvas，在其上面画图等操作了。
                            c.drawColor(Color.BLUE);//设置画布背景颜色
                            Paint p = new Paint(); //创建画笔
                            p.setColor(Color.WHITE);
                            Rect r = new Rect(100, 100, 300, 300);
                            c.drawRect(r, p);
                            p.setTextSize(50);
                            c.drawText("这是第"+(count++)+"秒", 100, 400, p);
                            
                            holder.unlockCanvasAndPost(c);//结束锁定画图，并提交改变。
                            Thread.sleep(1000);//睡眠时间为1秒
                        }
                    }
                    catch (Exception e) {
                        // TODO: handle exception
                        e.printStackTrace();
                    }
                    finally
                    {
                        if(c!= null)
                        {
                           

                        }
                    }
                }
            }
        }
    }
	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		int id = v.getId();
		if(id == R.id.button)
		{
			jietu();
		}
	}
	private void jietu()
	{
		
	}
}