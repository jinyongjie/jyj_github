package com.imagelib.ctrl;

import java.util.Map;

import com.imagelib.R;
import com.imagelib.app.ActivityDir;
import com.imagelib.data.Helper;
import com.imagelib.data.ImageMgr;
import com.imagelib.data.ImageMgr.DirInfo;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.text.SpannableString;
import android.text.Spanned;
import android.text.style.ForegroundColorSpan;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;

public class DirItem extends LinearLayout {
	private ThumbImageView mImage0;
	private ThumbImageView mImage1;
	private ThumbImageView mImage2;
	private ThumbImageView mImage3;
	private DirInfo mDirInfo;
	private TextView mTextName;
	private OnClickListener mClickListener = null;
	private View mFrame = null;
	private View mSel ;

	public DirItem(Context context) {
		super(context);
		// TODO Auto-generated constructor stub

	}

	public DirItem(android.content.Context context,
			android.util.AttributeSet attrs) {
		super(context, attrs);

	}

	public DirItem(Context context, AttributeSet attrs, Map inflateParams) {
		super(context, attrs);
	}

	public DirInfo GetDirInfo() {
		return mDirInfo;
	}

	public void init(DirInfo dirinfo, int imagesize, OnClickListener listener) {
		mClickListener = listener;
		mTextName = (TextView) findViewById(R.id.name);
		mImage0 = (ThumbImageView) findViewById(R.id.image0);
		mImage1 = (ThumbImageView) findViewById(R.id.image1);
		mImage2 = (ThumbImageView) findViewById(R.id.image2);
		mImage3 = (ThumbImageView) findViewById(R.id.image3);
		mFrame = findViewById(R.id.frame);
		mSel = findViewById(R.id.sel);

		mDirInfo = dirinfo;
		if(mDirInfo.name == null)
			mDirInfo.name = "";
		if(mDirInfo.dir == null)
			mDirInfo.dir = "";
		String text = String.format("%s(%d)", mDirInfo.name,
				mDirInfo.array.size());
		SpannableString ss = new SpannableString(text);
		ss.setSpan(new ForegroundColorSpan(Color.GRAY), mDirInfo.name.length(),
				text.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
		mTextName.setText(ss);

		mFrame.getLayoutParams().height = imagesize;
		mFrame.getLayoutParams().width = imagesize;
		
		this.setOnClickListener(mClickListener);
		
		int spacing = Helper.dp2px(getContext(), 3);
		int size = (imagesize  - spacing)/2;
		mImage0.setImageSize(imagesize, imagesize);
		mImage0.setImagePath(dirinfo.array.get(0).path);
		mImage0.getLayoutParams().width = size;
		mImage0.getLayoutParams().height = size;
		
		
		mImage1.setImageSize(imagesize, imagesize);
		

		FrameLayout.LayoutParams param = (android.widget.FrameLayout.LayoutParams) mImage1.getLayoutParams();
		param.width = size;
		param.height = size;
		param.leftMargin = size + spacing;
		if(mDirInfo.array.size() > 1)
		{
	
			mImage1.setImagePath(dirinfo.array.get(1).path);
		}
		else
		{
			mImage1.setImagePath(null);
		}

		mImage2.setImageSize(imagesize, imagesize);
		
		mImage2.getLayoutParams().width = size;
		mImage2.getLayoutParams().height = size;
		
		param = (android.widget.FrameLayout.LayoutParams) mImage2.getLayoutParams();
		param.width = size;
		param.height = size;
		param.topMargin = size + spacing;
		if(mDirInfo.array.size() > 2)
		{
			mImage2.setImagePath(dirinfo.array.get(2).path);
		}
		else
		{
			mImage2.setImagePath(null);
		}
		mImage3.setImageSize(imagesize, imagesize);
		
		param = (android.widget.FrameLayout.LayoutParams) mImage3.getLayoutParams();
		param.width = size;
		param.height = size;
		param.leftMargin = size + spacing;
		param.topMargin = size + spacing;
		if(mDirInfo.array.size() > 3)
		{
			mImage3.setImagePath(dirinfo.array.get(3).path);
		}
		else
		{
			mImage3.setImagePath(null);
		}

	}

	public void setSel(boolean sel) {
		if (mFrame != null) {
			if (sel) {
				mSel.setVisibility(View.VISIBLE);
			} else {
				mSel.setVisibility(View.GONE);
			}
		}
	}
}