package com.nanshan.myimage.ctrl;

import java.util.Map;

import com.nanshan.myimage.R;
import com.nanshan.myimage.data.ImageMgr;
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

public class ListByGroup extends GridView implements ImageMgrListener {
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

		GridAdapter adapter = new GridAdapter(context);
		this.setAdapter(adapter);

		ImageMgr.GetInstance().AddListener(this);
	}

	class GridAdapter extends BaseAdapter {
		LayoutInflater mInflater;

		public GridAdapter(Context context) {
			super();
			mInflater = LayoutInflater.from(context);

		}

		@Override
		public int getCount() {
			return ImageMgr.GetInstance().GetDirArray().size();
		}

		@Override
		public Object getItem(int position) {
			return ImageMgr.GetInstance().GetDirInfo(position);
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
			dirItem.SetDirInfo(ImageMgr.GetInstance().GetDirInfo(position));

			return view;
		}

	}

	@Override
	public void OnDataChange(ImageMgr.change_type type,int id) {
		// TODO Auto-generated method stub
		if(type == change_type.add || type == change_type.delete)
		{
			GridAdapter adapter = new GridAdapter(this.getContext());
			this.setAdapter(adapter);	
		}

	}

}
