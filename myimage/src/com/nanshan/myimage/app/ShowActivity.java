package com.nanshan.myimage.app;


import java.util.ArrayList;

import com.nanshan.myimage.R;
import com.nanshan.myimage.R.layout;
import com.nanshan.myimage.R.menu;

import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.view.Menu;
import android.view.View;
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
				imageView.setLayoutParams(new ImageSwitcher.LayoutParams(LayoutParams.WRAP_CONTENT,
						LayoutParams.WRAP_CONTENT));
				return imageView;
			}
		});
		// 设置图片切换的动画效果
		mSwitcher.setInAnimation(AnimationUtils.loadAnimation(this,
				android.R.anim.fade_in));
		mSwitcher.setOutAnimation(AnimationUtils.loadAnimation(this,
				android.R.anim.fade_out));
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.show, menu);
		return true;
	}
}
