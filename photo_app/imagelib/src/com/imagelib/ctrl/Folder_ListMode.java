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
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.widget.BaseAdapter;
import android.widget.GridView;
import android.widget.ListView;
import android.widget.RelativeLayout;

public class Folder_ListMode extends ListView implements Folder_Mode {
	private ArrayList<DirInfo> mData = new ArrayList<DirInfo>();
	private GridAdapter mAdapter;

	private OnClickListener	mClickListener;
	private String mSelDir;

	public Folder_ListMode(Context context, android.util.AttributeSet attrs) {
		super(context, attrs);
		// TODO Auto-generated constructor stub

		setSelector(R.drawable.selector_grid);

		DisplayMetrics dm = new DisplayMetrics();
		((Activity) context).getWindowManager().getDefaultDisplay()
				.getMetrics(dm);


		mAdapter = new GridAdapter(this.getContext());
		this.setAdapter(mAdapter);
		
		ColorDrawable d = new ColorDrawable(0xffeeeeee);
		setDivider(d);
		setDividerHeight(Helper.dp2px(context, 1));
		
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
					DirItemList dir = (DirItemList)v;
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
					for(int i = 0;i<Folder_ListMode.this.getChildCount();i++)
					{
						DirItemGrid item = (DirItemGrid) Folder_ListMode.this.getChildAt(i);
						item.setSel(false);
					}
					DirItemGrid item = (DirItemGrid) v;
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
				view = mInflater.inflate(R.layout.item_dir_list, null);

			} else {
				view = convertView;

			}
			DirItemList dirItem = (DirItemList) view;
			DirInfo info = mData.get(position);
			dirItem.init(info,0,mClickListener);
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
	@Override
	public View getView() {
		// TODO Auto-generated method stub
		return this;
	}
}
