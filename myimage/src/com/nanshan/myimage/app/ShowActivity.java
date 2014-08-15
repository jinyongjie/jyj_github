package com.nanshan.myimage.app;


import java.util.ArrayList;

import com.nanshan.myimage.R;
import com.nanshan.myimage.R.layout;
import com.nanshan.myimage.R.menu;
import com.nanshan.myimage.data.ImageLoader;
import com.nanshan.myimage.data.ImageMgr;
import com.nanshan.myimage.data.ImageLoader.ImageCallback;
import com.nanshan.myimage.data.ImageMgr.ImageInfo;

import android.os.Bundle;
import android.os.Handler;
import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.util.DisplayMetrics;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup.LayoutParams;
import android.view.animation.AnimationUtils;
import android.widget.ImageSwitcher;
import android.widget.ImageView;
import android.widget.ViewSwitcher.ViewFactory;

public class ShowActivity extends Activity {
	ArrayList<Integer> mArray;
	ImageSwitcher mSwitcher;
	int mIndex = 0;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_show);
	
		mSwitcher = (ImageSwitcher)findViewById(R.id.switcher);
		
		mArray = new ArrayList<Integer>();
		Intent intent = getIntent();
		mIndex = 0;
		
		int[] array = intent.getIntArrayExtra("array");
		for(int i = 0;i<array.length;i++)
		{
			mArray.add(array[i]);
		}
		
		mSwitcher.setFactory(new ViewFactory() {
			@Override
			public View makeView() {
				ImageView imageView = new ImageView(ShowActivity.this);
				imageView.setBackgroundColor(0xff0000);
				imageView.setScaleType(ImageView.ScaleType.FIT_CENTER);
				imageView.setLayoutParams(new ImageSwitcher.LayoutParams(LayoutParams.MATCH_PARENT,
						LayoutParams.MATCH_PARENT));
				return imageView;
			}
		});
		// 设置图片切换的动画效果
		mSwitcher.setInAnimation(AnimationUtils.loadAnimation(this,
				android.R.anim.fade_in));
		mSwitcher.setOutAnimation(AnimationUtils.loadAnimation(this,
				android.R.anim.fade_out));
		mSwitcher.setOnClickListener(new OnClickListener()
		{

			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				ShowActivity.this.finish();
			}
			
		});
		ShowNext(0);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.show, menu);
		return true;
	}

	void ShowNext(int time) {
		Handler handler = new Handler();
		Runnable runnable = new Runnable() {
			@Override
			public void run() {
				// TODO Auto-generated method stub
				// 要做的事情
				if(mIndex == mArray.size())
				{
					ShowActivity.this.finish();
				}
				else
				{
					DisplayMetrics dm = new DisplayMetrics();
					getWindowManager().getDefaultDisplay().getMetrics(dm);
					
					ImageInfo info = ImageMgr.GetInstance().GetImage(mArray.get(mIndex));
					ImageLoader.GetInstance().GetImage(info.id, info.path, dm.widthPixels, dm.heightPixels, false, callback);

				}
			}
		};
		handler.postDelayed(runnable, time);//
	}
	ImageCallback callback = new ImageCallback()
	{

		@Override
		public void OnImageLoad(Bitmap bitmap, String path) {
			// TODO Auto-generated method stub
			mSwitcher.setImageDrawable(new BitmapDrawable(bitmap));
			mIndex++;
			ShowNext(2000);
		}

		@Override
		public void OnImageRotate(int id) {
			// TODO Auto-generated method stub
			
		}
		
	};
}
