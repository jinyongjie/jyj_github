package com.nanshan.myimage.ctrl;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;

import com.nanshan.myimage.R;
import com.nanshan.myimage.app.SingleImageActivity;
import com.nanshan.myimage.ctrl.ListByTime.MyListAdapter.ItemTag;
import com.nanshan.myimage.data.ImageLoader;
import com.nanshan.myimage.data.ImageMgr;
import com.nanshan.myimage.data.ImageLoader.ImageCallback;
import com.nanshan.myimage.data.ImageMgr.TimeInfo;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AbsListView;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.view.View.OnClickListener;

public class ListByTime extends LinearLayout implements OnClickListener{

	int mImageWidth;
	final int mColum = 4;
	ArrayList<TimeInfo> mGroups;
	ListView mListView;
	TextView mTextSelNum;
	Button mButtonReturn;
	Button mButtonDel;
	View mBarOp;

	HashSet<Integer> mSelectItems;

	class ItemInfo {
		public boolean isTitle;// 0=tile,1=image
		public int groupid;
		public int imageid[] = { -1, -1, -1, -1 };
	}

	ArrayList<ItemInfo> mItemInfos = new ArrayList<ItemInfo>();

	public ListByTime(Context context) {
		super(context);
		// TODO Auto-generated constructor stub


	}

	public ListByTime(android.content.Context context,
			android.util.AttributeSet attrs) {
		super(context, attrs);


	}

	public void Init() {

		mSelectItems = new HashSet<Integer>();
				 
		DisplayMetrics dm = new DisplayMetrics();
		((Activity) this.getContext()).getWindowManager().getDefaultDisplay()
				.getMetrics(dm);
		mImageWidth = (dm.widthPixels) * 39 / 160;
		
		mButtonReturn = (Button) findViewById(R.id.button_return);
		mTextSelNum = (TextView) findViewById(R.id.text_sel_num);
		mButtonDel = (Button) findViewById(R.id.button_del);
		mBarOp = findViewById(R.id.bar_op);
		
		mButtonReturn.setOnClickListener(this);
		mButtonDel.setOnClickListener(this);

	}

	public void SetGroups(ArrayList<TimeInfo> groups) {
		

		
		
		mGroups = groups;
		if (mSelectItems != null) {
			mSelectItems.clear();
		}
		mListView = (ListView) findViewById(R.id.timelist);
		MyListAdapter adapter = new MyListAdapter(this.getContext(), 0);
		mListView.setAdapter(adapter);
		

	}

	class MyListAdapter extends BaseAdapter implements OnClickListener {

		LayoutInflater mInflator;

		/**
		 * 
		 * @param context
		 *            mainActivity
		 * @param persons
		 *            显示的数据
		 * @param resource
		 *            一个Item的布局
		 */
		public MyListAdapter(Context context, int resource) {

			mInflator = LayoutInflater.from(context);

			ArrayList<TimeInfo> groups = mGroups;
			int groupcount = groups.size();
			mItemInfos.clear();
			for (int i = 0; i < groupcount; i++) {
				int image_count = groups.get(i).array.size();
				ItemInfo info = new ItemInfo();
				info.isTitle = true;
				info.groupid = i;

				mItemInfos.add(info);

				ItemInfo info2 = null;
				for (int j = 0; j < image_count; j++) {
					if (j % 4 == 0) {
						info2 = new ItemInfo();
						info2.isTitle = false;
						info2.groupid = i;

						mItemInfos.add(info2);
					}

					info2.imageid[j % 4] = groups.get(i).array.get(j).id;
				}
			}
		}

		/*
		 * 获得数据总数
		 */
		@Override
		public int getCount() {
			return mItemInfos.size();
		}

		/*
		 * 根据索引为position的数据
		 */
		@Override
		public Object getItem(int position) {
			return mItemInfos.get(position);
		}

		/*
		 * 根据索引值获得Item的Id
		 */
		@Override
		public long getItemId(int position) {
			return position;
		}

		public int getItemViewType(int position) {
			return mItemInfos.get(position).isTitle ? 0 : 1;
		}

		public int getViewTypeCount() {
			return 2;
		}

		/*
		 * 通过索引值position将数据映射到视图convertView具有缓存功能，在第一页时为null，在第二第三....页时不为null
		 */
		public class ItemTag {
			public boolean isTitle;
			int group;
			TextView textDate;
			TextView textWeek;
			TextView textCount;
			Button buttonSelAll;
			
		}

		@Override
		public View getView(int position, View convertView, ViewGroup parent) {
			View v = null;

			ItemInfo info = mItemInfos.get(position);
			if (convertView != null) {
				ItemTag tag = (ItemTag) convertView.getTag();
				if (tag.isTitle == info.isTitle) {
					v = convertView;
				}
			}
			ItemTag tag = null;
			if (v == null) {
				tag = new ItemTag();
				tag.isTitle = info.isTitle;
				if (info.isTitle == true) {
					v = mInflator.inflate(R.layout.item_title, null);
					tag.textDate = (TextView) v.findViewById(R.id.date);
					tag.textWeek = (TextView) v.findViewById(R.id.week);
					tag.textCount = (TextView) v.findViewById(R.id.count);
					tag.buttonSelAll=(Button)v.findViewById(R.id.sel_all);
					tag.buttonSelAll.setOnClickListener(this);
				} else {
					v = mInflator.inflate(R.layout.item_4image, null);
				}
				v.setTag(tag);

			} else {
				tag = (ItemTag) v.getTag();
			}
			tag.group = info.groupid;
			if (info.isTitle == true) {// title
				ImageMgr.TimeInfo group = mGroups.get(info.groupid);

				SimpleDateFormat sd = new SimpleDateFormat("yyyy-MM-dd");
				SimpleDateFormat sdw = new SimpleDateFormat("(E)");

				tag.textDate.setText(sd.format(group.date));

				tag.textWeek.setText(sdw.format(group.date));
				tag.textCount.setText(String.format("%d张", group.array.size()));
				
				tag.buttonSelAll.setVisibility(mEditMode == true?View.VISIBLE:View.GONE);
				tag.textCount.setVisibility(mEditMode == true?View.GONE:View.VISIBLE);
				
			} else {// 4image
				ViewGroup viewgroup = (ViewGroup) v;
				for (int i = 0; i < mColum; i++) {
					MyImageView imageview = (MyImageView) viewgroup
							.getChildAt(i);

					imageview.SetImageSize(mImageWidth, mImageWidth, true);
					imageview.SetImageId(info.imageid[i]);
					if (info.imageid[i] == -1) {
						imageview.setVisibility(View.INVISIBLE);
					} else {
						imageview.setVisibility(View.VISIBLE);
					}

					imageview.setOnClickListener(this);
					if (mEditMode == true) {
						imageview.SetSelect(mSelectItems.contains(imageview
								.GetImageId()));
					}
				}
			}

			return v;
		}
		void SelAll(int group)
		{
			boolean sel_already = true;
			//判断是否已全选
			for(int i = 0;i<mItemInfos.size();i++)
			{
				if(sel_already == true)
				{
					ItemInfo info = mItemInfos.get(i);
					if(info.groupid == group && info.isTitle == false)
					{
						for(int j = 0;j<mColum;j++)
						{
							if(info.imageid[j] != -1)
							{
								if(mSelectItems.contains(info.imageid[j]) == false)
								{
									sel_already = false;
									break;
								}
							}
						}
					}
				}
			}
//			if(info.imageid[j] != -1)
	
		//	mSelectItems.add(info.imageid[j]);
			
			
			for(int i = 0;i<mListView.getChildCount();i++)
			{
				ViewGroup item = (ViewGroup)mListView.getChildAt(i);
				ItemTag tag = (ItemTag)item.getTag();
				if(tag != null && tag.group == group && tag.isTitle == false)
				{
						for (int j = 0; j < mColum; j++) {
						MyImageView imageview = (MyImageView) item.getChildAt(j);
						imageview.SetSelect(!sel_already);	
						
					}
				}
			
			}
			for(int i = 0;i<mItemInfos.size();i++)
			{
					ItemInfo info = mItemInfos.get(i);
					if(info.groupid == group && info.isTitle == false)
					{
						for(int j = 0;j<mColum;j++)
						{
							if(info.imageid[j] != -1)
							{
								if(sel_already == true)
								{
									mSelectItems.remove(info.imageid[j]);
								}
								else
								{
									mSelectItems.add(info.imageid[j]);
								}
							}
						}
					}
				
			}
				
			UpdateSelectNum();
		}
		void SetImageSelect(MyImageView imageview,boolean sel)
		{
			
		}
		@Override
		public void onClick(View v) {
			// TODO Auto-generated method stub
			// TODO Auto-generated method stub
			if(v instanceof MyImageView)
			{
				MyImageView clickview = (MyImageView) v;
				if (mEditMode == false) {
					Intent intent = new Intent();
					// Bundle data = new Bundle();

					ArrayList<Integer> array = new ArrayList<Integer>();
					intent.setClass(v.getContext(), SingleImageActivity.class);

					for (int i = 0; i < mGroups.size(); i++) {
						for (int j = 0; j < mGroups.get(i).array.size(); j++) {
							array.add(mGroups.get(i).array.get(j).id);
						}
					}

					int[] idarray = new int[array.size()];
					int cur = 0;

					for (int i = 0; i < array.size(); i++) {
						idarray[i] = array.get(i);
						if (clickview.GetImageId() == idarray[i])
							cur = i;
					}

					intent.putExtra("array", idarray);
					intent.putExtra("index", cur);
					

					v.getContext().startActivity(intent);
				} else {
					boolean old = clickview.GetSelect();
					clickview.SetSelect(!old);
					if (old == true) {
						mSelectItems.remove(clickview.GetImageId());
					} else {
						mSelectItems.add(clickview.GetImageId());
					}
					UpdateSelectNum();
					
				}
			}
			else if(v instanceof Button)
			{
				ViewGroup parent =	(ViewGroup)v.getParent();
				ItemTag tag = (ItemTag)parent.getTag();
				this.SelAll(tag.group);
				
			}
			
		}

	}

	EditModeListener mEditModeListener = null;
	boolean mEditMode = false;

	public void SetEditModeListener(EditModeListener listener)
	{
		mEditModeListener = listener;
	}
	public void SetEditMode(boolean mode) {
		if (mEditMode != mode) {
			mEditMode = mode;
			if (mEditMode == true) {
			
				UpdateSelectNum();
				mBarOp.setVisibility(View.VISIBLE);
			} else {
				CleanSelect();
				mBarOp.setVisibility(View.GONE);
			}
			for(int i = 0;i<mListView.getChildCount();i++)
			{
				ItemTag tag = (ItemTag)mListView.getChildAt(i).getTag();
				if(tag != null && tag.isTitle == true)
				{
					tag.buttonSelAll.setVisibility(mEditMode == true?View.VISIBLE:View.GONE);
					tag.textCount.setVisibility(mEditMode == true?View.GONE:View.VISIBLE);
				}
			
			}
		}
	}
	private void CleanSelect()
	{
		mSelectItems.clear();

		int count = mListView.getChildCount();
		for (int i = 0; i <count ; i++) {
			ViewGroup v = (ViewGroup) mListView.getChildAt(i);
			MyListAdapter.ItemTag tag = (MyListAdapter.ItemTag) v.getTag();
			if (tag.isTitle == false) {
				for (int j = 0; j < mColum; j++) {
					MyImageView image = (MyImageView) v.getChildAt(j);
					image.SetSelect(false);
				}
			}
		}
	}
	private void UpdateSelectNum()
	{
		if (mTextSelNum != null) {
			mTextSelNum.setText(String.format("多选,已选择%d张", GetSelCount()));
		}
	}

	public interface EditModeListener {
		public void OnFinishEditMode();
	}

	public int GetSelCount() {
		if (mSelectItems != null) {
			return mSelectItems.size();
		}
		return 0;
	}

	public void DelSelImages() {
		ImageMgr.GetInstance().RemoveImage(mSelectItems);
	}

	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		if (v == mButtonReturn) {
			
			if(mEditModeListener != null)
			{
				mEditModeListener.OnFinishEditMode();
			}
			
		} else if (v == mButtonDel) {
			int selnum = GetSelCount();
			if (selnum > 0) {
				dialog(selnum);
			
			}
		}
	}
	protected void dialog(int num) {
		AlertDialog.Builder builder = new Builder(this.getContext());
		builder.setMessage(String.format("已选中%d张照片，确定要删除吗?", num));
		// builder.setTitle("提示");
		builder.setPositiveButton("确认", new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				dialog.dismiss();
				DelSelImages();
				mSelectItems.clear();
				UpdateSelectNum();
			}

		});
		builder.setNegativeButton("取消", new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				dialog.dismiss();
			}
		});
		builder.create().show();
	}
	public void OnImageRotate(int id)
	{
		for(int i = 0;i<mListView.getChildCount();i++)
		{
			ViewGroup group = (ViewGroup) mListView.getChildAt(i);
			ItemTag tag = (ItemTag)group.getTag();
			if(tag.isTitle == false)
			{
				for(int j=0;j<mColum;j++)
				{
					MyImageView view = (MyImageView) group.getChildAt(j);
					if(view.GetImageId() == id)
					{
						view.UpdateImage();
					}
				}
			}
		}
	}
}
