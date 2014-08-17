package com.nanshan.myimage.ctrl;

import java.util.ArrayList;
import java.util.Map;

import com.nanshan.myimage.R;
import com.nanshan.myimage.data.ImageMgr;
import com.nanshan.myimage.data.ImageMgr.DirInfo;
import com.nanshan.myimage.data.ImageMgr.ImageMgrListener;
import com.nanshan.myimage.data.ImageMgr.change_type;

import android.app.Activity;
import android.content.Context;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.GridView;
import android.widget.RelativeLayout;

public class ListByGroup extends GridView  {
	ArrayList<DirInfo> mData;
	public ListByGroup(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
		this.setNumColumns(2);

		setSelector(R.drawable.selector_grid);

		DisplayMetrics dm = new DisplayMetrics();
		((Activity) context).getWindowManager().getDefaultDisplay()
				.getMetrics(dm);

		this.setGravity(Gravity.CENTER);
		this.setStretchMode(STRETCH_COLUMN_WIDTH);
		this.setVerticalSpacing(5 * dm.widthPixels / 160);
		this.setHorizontalSpacing(0 * dm.widthPixels / 160);



		
	}

	public void SetData(ArrayList<DirInfo> data)
	{
		mData = data;
		GridAdapter adapter = new GridAdapter(this.getContext());
		this.setAdapter(adapter);
	}
	class GridAdapter extends BaseAdapter {
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
			dirItem.SetDirInfo(mData.get(position));

			return view;
		}

	}



	
}
