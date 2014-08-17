package com.nanshan.myimage.ctrl;

import com.nanshan.myimage.R;
import com.nanshan.myimage.data.ImageMgr;
import com.nanshan.myimage.data.ImageMgr.ImageInfo;

import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Parcelable;
import android.support.v4.view.PagerAdapter;
import android.support.v4.view.ViewPager;
import android.util.DisplayMetrics;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

public class SetTagDialog extends Dialog {
	String[] mTagList = { "自拍", "朋友合照", "家庭合照", "美女们", "帅哥们", "宝宝", "美食", "聚餐",
			"建筑", "室内", "风景", "夜景", "街景", "天空", "商品", "植物", "玩偶", "表演", "随拍",
			"旅游" };
	int mImageId = -1;
	String mCurTag;

	Context mContext;
	GridView mGrid;

	public SetTagDialog(Context context, int theme, int id) {
		super(context, theme);
		// TODO Auto-generated constructor stub
		mContext = context;
		mImageId = id;
		ImageInfo info = ImageMgr.GetInstance().GetImage(id);
		if (info != null) {
			mCurTag = info.tag;
		}
	}

	Button mButtonOk;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);

		Button buttonClose = (Button) findViewById(R.id.button_close);
		buttonClose.setOnClickListener(new android.view.View.OnClickListener() {

			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				dismiss();
			}

		});
		mButtonOk = (Button) findViewById(R.id.button_ok);
		mButtonOk.setOnClickListener(new android.view.View.OnClickListener() {

			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub

				ImageInfo info = ImageMgr.GetInstance().GetImage(mImageId);
				if (info != null) {
					ImageMgr.GetInstance().SetTag(mImageId, mCurTag);
					dismiss();
				}

			}

		});
		mGrid = (GridView) findViewById(R.id.grid);
		mGrid.setAdapter(new MyAdapter(mContext));
	}

	class MyAdapter extends BaseAdapter {

		MyAdapter(Context context) {

		}

		public int getCount() {
			return mTagList.length;
		}

		public Object getItem(int item) {

			return item;
		}

		public long getItemId(int id) {
			return id;
		}

		// 创建View方法
		public View getView(int position, View convertView, ViewGroup parent) {
			TextView view;
			if (convertView != null) {
				view = (TextView) convertView;
			} else {
				view = new TextView(mContext);
				view.setTextColor(Color.WHITE);
				view.setTextSize(12);
				view.setGravity(Gravity.CENTER);

				view.setPadding(10, 40, 10, 40);
				view.setOnClickListener(clicklistner);

			}
			view.setText(mTagList[position]);
			String tag = (String) view.getText();
			if (tag == mCurTag) {
				view.setBackgroundResource(R.drawable.shape_tagbutton);
			} else {
				view.setBackgroundColor(Color.GRAY);
			}

			return view;
		}
	}

	public void SetImageId(int id) {
		mImageId = id;
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		// TODO Auto-generated method stub
		if (keyCode == KeyEvent.KEYCODE_BACK) {
			this.dismiss();
			return true;
		}
		return super.onKeyDown(keyCode, event);
	}

	android.view.View.OnClickListener clicklistner = new android.view.View.OnClickListener() {
		public void onClick(View v) {
			// TODO Auto-generated method stub

			TextView text = (TextView) v;
			String tag = (String) text.getText();
			if (mCurTag == null || mCurTag.length() == 0) {
				mCurTag = tag;
				text.setBackgroundResource(R.drawable.shape_tagbutton);
			} else {
				if (mCurTag == tag) {
					mCurTag = "";
					text.setBackgroundColor(Color.GRAY);
				} else {
					for (int i = 0; i < mGrid.getChildCount(); i++) {
						TextView t = (TextView) mGrid.getChildAt(i);
						if (t.getText() == mCurTag) {
							t.setBackgroundColor(Color.GRAY);
						}
					}
					text.setBackgroundResource(R.drawable.shape_tagbutton);
					mCurTag = tag;
				}

			}

		}
	};

}
