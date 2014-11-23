package com.imagelib.ctrl;

import java.util.Map;

import com.imagelib.R;
import com.imagelib.app.ActivityDir;
import com.imagelib.app.AppSetting;
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

public class DirItemList extends LinearLayout {
	private ThumbImageView mImage0;
	private DirInfo mDirInfo;
	private TextView mTextName;
	private TextView mTextCount;
	private OnClickListener mClickListener = null;


	public DirItemList(Context context) {
		super(context);
		// TODO Auto-generated constructor stub

	}

	public DirItemList(android.content.Context context,
			android.util.AttributeSet attrs) {
		super(context, attrs);

	}

	public DirItemList(Context context, AttributeSet attrs, Map inflateParams) {
		super(context, attrs);
	}

	public DirInfo GetDirInfo() {
		return mDirInfo;
	}

	public void init(DirInfo dirinfo, int imagesize, OnClickListener listener) {
		mClickListener = listener;
		mTextName = (TextView) findViewById(R.id.title);
		mTextCount = (TextView)findViewById(R.id.count);
		mImage0 = (ThumbImageView) findViewById(R.id.image);

		mTextName.setTextColor(AppSetting.instance().getInt("album_name_color"));
		mTextCount.setTextColor(AppSetting.instance().getInt("album_count_color"));
		

		mDirInfo = dirinfo;
		if(mDirInfo.name == null)
			mDirInfo.name = "";
		if(mDirInfo.dir == null)
			mDirInfo.dir = "";

		mTextName.setText( mDirInfo.name);
		if(mDirInfo.name.compareToIgnoreCase("camera") == 0)
			mTextName.setText("系统相册");
		mTextCount.setText(String.format("共%d张", mDirInfo.array.size()));


		setOnClickListener(mClickListener);
		
		int size = Helper.dp2px(getContext(), 100);
		mImage0.setImageSize(size, size);
		mImage0.setImagePath(dirinfo.array.get(0).path);

	}

	public void setSel(boolean sel) {

	}
}