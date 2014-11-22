package com.imagelib.ctrl;

import java.util.ArrayList;
import java.util.Map;

import com.imagelib.R;





import com.imagelib.app.ActivityDir;
import com.imagelib.data.Helper;
import com.imagelib.data.ImageMgr;
import com.imagelib.data.ImageMgr.DirInfo;
import com.imagelib.data.ImageMgr.ImageMgrListener;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.widget.BaseAdapter;
import android.widget.GridView;
import android.widget.RelativeLayout;

public class ListByGroup extends GridView {
	private ArrayList<DirInfo> mData = new ArrayList<DirInfo>();
	private GridAdapter mAdapter;
	int mPadding = 10;
	int mImageWidth = 60;
	private OnClickListener	mClickListener;
	private String mSelDir;
	
	public ListByGroup(Context context, android.util.AttributeSet attrs) {
		super(context, attrs);
		// TODO Auto-generated constructor stub
		this.setNumColumns(2);

		setSelector(R.drawable.selector_grid);

		DisplayMetrics dm = new DisplayMetrics();
		((Activity) context).getWindowManager().getDefaultDisplay()
				.getMetrics(dm);

		//this.setGravity(Gravity.CENTER);
		this.setStretchMode(STRETCH_COLUMN_WIDTH);
		mPadding = Helper.dp2px(context, 20);
		this.setVerticalSpacing(mPadding);
		this.setHorizontalSpacing(mPadding);
		mImageWidth = (dm.widthPixels - mPadding)/2 - 2*Helper.dp2px(context, 5);

		mAdapter = new GridAdapter(this.getContext());
		this.setAdapter(mAdapter);
	}
	public void init(boolean selmode)
	{
		if(selmode == false )
		{
			setClickListener(new OnClickListener() {
				// todo
				@Override
				public void onClick(View v) {
					// TODO Auto-generated method stub

					Intent intent = new Intent();
					// Bundle data = new Bundle();
					DirItem dir = (DirItem)v;
					if(dir != null)
					{
						DirInfo info = dir.GetDirInfo();
						intent.setClass(v.getContext(), ActivityDir.class);
						intent.putExtra("dir", info.name);
						ActivityDir.setArray(info.array);
						
						v.getContext().startActivity(intent);		
					}
				}
			});
		}
		else
		{
			setClickListener(new OnClickListener() {
				// todo
				@Override
				public void onClick(View v) {
					// TODO Auto-generated method stub
					for(int i = 0;i<ListByGroup.this.getChildCount();i++)
					{
						DirItem item = (DirItem) ListByGroup.this.getChildAt(i);
						item.setSel(false);
					}
					DirItem item = (DirItem) v;
					item.setSel(true);
					mSelDir = item.GetDirInfo().dir;
				}
			});
		}
	}
	
	private void setClickListener(OnClickListener listener)
	{
		mClickListener = listener;
	}
	public void setData(ArrayList<DirInfo> data) {
		mData = data;
		if(mAdapter != null)
			mAdapter.notifyDataSetChanged();
	}

	private class GridAdapter extends BaseAdapter {
		LayoutInflater mInflater;

		public GridAdapter(Context context) {
			super();
			mInflater = LayoutInflater.from(context);

		}

		@Override
		public int getCount() {
			return mData.size();
		}

		@Override
		public Object getItem(int position) {
			return mData.get(position);
		}

		@Override
		public long getItemId(int position) {
			return position;
		}

		@Override
		public View getView(int position, View convertView, ViewGroup parent) {
			View view = null;

			if (convertView == null) {
				view = mInflater.inflate(R.layout.item_dir, null);

			} else {
				view = convertView;

			}
			DirItem dirItem = (DirItem) view;
			DirInfo info = mData.get(position);
			dirItem.init(info,mImageWidth,mClickListener);
			if(mSelDir != null && info.dir.equals(mSelDir))
			{
				dirItem.setSel(true);
			}
			else
			{
				dirItem.setSel(false);
			}
			return view;
		}

	}
	public String getSelDir()
	{
		return mSelDir;
	}
}
