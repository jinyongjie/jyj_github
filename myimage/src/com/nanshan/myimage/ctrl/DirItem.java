package com.nanshan.myimage.ctrl;

import java.util.Map;

import com.nanshan.myimage.R;
import com.nanshan.myimage.app.DirActivity;
import com.nanshan.myimage.data.ImageMgr;
import com.nanshan.myimage.data.ImageMgr.DirInfo;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;

public class DirItem extends LinearLayout {
	MyImageView mImage;
	DirInfo mDirInfo;
	TextView mTextName;
	TextView mTextCount;
	int mImageWidth;

	public DirItem(Context context) {
		super(context);
		// TODO Auto-generated constructor stub

	}

	public DirItem(android.content.Context context,
			android.util.AttributeSet attrs) {
		super(context, attrs);

		DisplayMetrics dm = new DisplayMetrics();
		((Activity) context).getWindowManager().getDefaultDisplay()
				.getMetrics(dm);
		mImageWidth = (dm.widthPixels) * 75 / 160;

	}

	public DirItem(Context context, AttributeSet attrs, Map inflateParams) {
		super(context, attrs);
	}

	public void SetDirInfo(DirInfo dirinfo) {
		mTextName = (TextView) findViewById(R.id.name);
		mTextCount = (TextView) findViewById(R.id.count);
		mImage = (MyImageView) findViewById(R.id.image);
		mImage.SetImageSize(mImageWidth, mImageWidth, true);

		mDirInfo = dirinfo;
		mTextName.setText(mDirInfo.name);
		mTextCount.setText(String.format("(%d)", mDirInfo.array.size()));
		if (dirinfo.array.size() > 0)
			mImage.SetImageId(dirinfo.array.get(0).id);

		mImage.setOnClickListener(new OnClickListener() {
			// todo
			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub

				Intent intent = new Intent();
				// Bundle data = new Bundle();

				intent.setClass(v.getContext(), DirActivity.class);
				intent.putExtra("dir", mDirInfo.name);
				// intent.putExtra("count",
				// ImageMgr.GetInstance().GetImageCount());
				v.getContext().startActivity(intent);
			}

		});
	}
}