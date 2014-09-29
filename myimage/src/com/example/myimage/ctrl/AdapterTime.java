package com.example.myimage.ctrl;

import java.lang.ref.WeakReference;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;
import java.util.HashSet;

import com.example.myimage.app.ActivityGallery;
import com.example.myimage.data.Helper;
import com.example.myimage.data.ImageMgr;
import com.example.myimage.data.Size;
import com.example.myimage.data.ImageMgr.FileComparator;
import com.example.myimage.data.ImageMgr.ImageInfo;
import com.example.myimage.data.ImageMgr.TimeInfo;
import com.example.myimage.R;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.graphics.drawable.Drawable;
import android.util.DisplayMetrics;
import android.util.Pair;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.view.View.OnLongClickListener;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;

public abstract class AdapterTime extends BaseAdapter implements OnClickListener,
		OnLongClickListener {
	public interface IImageItem
	{
		public void init(int size, OnClickListener l1, OnLongClickListener l2);
		public void setSel(boolean sel);
		public boolean getSel();
		public void setImagePath(String path);
		public String GetImagePath();
		
		public void setSelMode(boolean mode);
		public void updateImage();
		public void setData(ImageData data);
		public ImageData getData();
	}
	public class ImageData
	{
		public String path;
		public boolean sel;
	}
	public class LineInfo {
		public boolean isTitle;// 0=tile,1=image
		public ArrayList<ImageData> imagearray;
		public Date date;
		public int childcount;// children num of group
		
		int pos;
	}
	private int mImageWidth;
	private int mSpacing = 10;
	private final int mColum = 3;
	protected ArrayList<LineInfo> mData = new ArrayList<LineInfo>();
	protected LayoutInflater mInflator;
	private boolean mEditMode = false;
	WeakReference<ListView> mListView;

	public AdapterTime(ListView list) {

		mListView = new WeakReference<ListView>(list);
		mInflator = LayoutInflater.from(list.getContext());

		Size sz = Helper.getScreenSize(list.getContext());
		mSpacing = Helper.dp2px(this.getContext(), 3);
		mImageWidth = (sz.width - mSpacing * (mColum -1)) / mColum;
		
		ColorDrawable d = new ColorDrawable(Color.TRANSPARENT);
		list.setDivider(d);
		list.setDividerHeight(mSpacing);
	}

	public  ArrayList<LineInfo> parse(ArrayList<ImageInfo> array) {

		ArrayList<LineInfo> data = new ArrayList<LineInfo>();

		Collections.sort(array, ImageMgr.GetInstance().new FileComparator());
		ArrayList<TimeInfo> groups = new ArrayList<TimeInfo>();

		TimeInfo group = null;

		for (int i = 0; i < array.size(); i++) {

			ImageInfo info = array.get(i);
			if (info != null) {
				if (group == null) {
					group = ImageMgr.GetInstance().new TimeInfo();
					group.date = info.date;
					groups.add(group);
				} else {
					Date last = group.date;
					Date cur = info.date;
					if (last.getYear() != cur.getYear()
							|| last.getMonth() != cur.getMonth()
							|| last.getDay() != cur.getDay()) {
						group = ImageMgr.GetInstance().new TimeInfo();
						group.date = cur;
						groups.add(group);
					}

				}
				group.array.add(info);
			}
		}

		int groupcount = groups.size();

		for (int i = 0; i < groupcount; i++) {
			TimeInfo g = groups.get(i);
			int image_count = g.array.size();

			LineInfo info = new LineInfo();
			info.isTitle = true;
			info.date = g.date;
			info.childcount = image_count;

			data.add(info);

			LineInfo info2 = null;
			for (int j = 0; j < image_count; j++) {
				if (j % mColum == 0) {
					info2 = new LineInfo();
					info2.isTitle = false;
					info2.date = info.date;
					info2.childcount = image_count;
					info2.imagearray = new ArrayList<ImageData>();
					

					data.add(info2);
				}
				ImageData item = new ImageData();
				item.path = g.array.get(j).path;
				item.sel = false;
				info2.imagearray.add(item);
			
			}
		}

		return data;
	}
	public void setData(ArrayList<LineInfo> data)
	{
		mData = data;
		notifyDataSetChanged();
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

	public int getItemViewType(int position) {
		return mData.get(position).isTitle ? 0 : 1;
	}

	public int getViewTypeCount() {
		return 2;
	}


	private Context getContext()
	{
		if(mListView != null && mListView.get() != null)
		{
			return mListView.get().getContext();
		}
		return null;
	}
	protected abstract IImageItem createImageItem();
	protected abstract void initItemData(IImageItem item);
	
	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		View v = null;

		LineInfo info = mData.get(position);
		if (convertView != null) {
			LineInfo tag = (LineInfo) convertView.getTag();
			if (tag.isTitle == info.isTitle) {
				v = convertView;
			}
		}
		LineInfo tag = info;
		if (v == null) {
			if (info.isTitle == true) {
				v = mInflator.inflate(R.layout.item_title, null);
				
			} else {
				LinearLayout layout = new LinearLayout(getContext());
				v = layout;
				layout.setOrientation(LinearLayout.HORIZONTAL);
			
				for (int i = 0; i < mColum; i++) {
					IImageItem item = createImageItem();
					item.init(mImageWidth,this,this);
					View image = (View) item;
					layout.addView(image);

					LinearLayout.LayoutParams param = new LinearLayout.LayoutParams(
							mImageWidth, mImageWidth);

					if (i < mColum - 1) {
						param.rightMargin = mSpacing;
					}
					image.setLayoutParams(param);
				}
			}
			
		}
		tag.pos = position;
		v.setTag(tag);
		if (info.isTitle == true) {// title

			TextView textDate = (TextView) v.findViewById(R.id.date);
			TextView textWeek = (TextView) v.findViewById(R.id.week);
			TextView textCount = (TextView) v.findViewById(R.id.count);
			Button buttonSelAll = (Button) v.findViewById(R.id.sel_all);
			buttonSelAll.setOnClickListener(this);
			
			SimpleDateFormat sd = new SimpleDateFormat("yyyy-MM-dd");
			SimpleDateFormat sdw = new SimpleDateFormat("E");

			textDate.setText(sd.format(info.date));

			textWeek.setText(sdw.format(info.date));
			textCount.setText(String.format("%d张", info.childcount));

			buttonSelAll.setVisibility(mEditMode == true ? View.VISIBLE
					: View.GONE);
			textCount.setVisibility(mEditMode == true ? View.GONE
					: View.VISIBLE);

			boolean all = checkGroupAllSel(tag.pos);//todo 效率
		
			updateSelButton(all,buttonSelAll);

		} else {// 4image
			ViewGroup viewgroup = (ViewGroup) v;
			for (int i = 0; i < mColum; i++) {
				View imageview =  viewgroup
						.getChildAt(i);
				IImageItem item = (IImageItem)imageview;
				if (i < info.imagearray.size()) {
					imageview.setVisibility(View.VISIBLE);
					
					ImageData data = info.imagearray.get(i);
					item.setImagePath(data.path);
					item.setSelMode(mEditMode);
					if (mEditMode == true) {
						item.setSel(data.sel);
					}
					initItemData(item);
					item.setData(data);
				} else {
					imageview.setVisibility(View.INVISIBLE);
				}

			}
		}

		return v;
	}

	public void updateSelButton(boolean allseled,Button button)
	{
	
		
		if (allseled) {
			button.setText("全不选");
			
			Drawable d = mListView.get().getResources().getDrawable(R.drawable.selall_hover);
			d.setBounds(0, 0, d.getMinimumWidth(), d.getMinimumHeight());
			
			button.setCompoundDrawables(null, null, d, null);
		} else {
			
			Drawable d = mListView.get().getResources().getDrawable(R.drawable.selall_normal);
			d.setBounds(0, 0, d.getMinimumWidth(), d.getMinimumHeight());
			button.setCompoundDrawables(null, null, d, null);
			button.setText("全选");
		}
	}
	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		// TODO Auto-generated method stub
		if (v instanceof IImageItem) {
			IImageItem clickview = (IImageItem) v;
			if (mEditMode == false) {
			

				ArrayList<String> array = new ArrayList<String>();
		

				for (int i = 0; i < mData.size(); i++) {
					LineInfo info = mData.get(i);
					if (info.isTitle == false)
						for (int j = 0; j < info.imagearray.size(); j++) {
							array.add(info.imagearray.get(j).path);
						}
				}

				
				int cur = 0;

				for (int i = 0; i < array.size() ; i++) {
					
					if (clickview.GetImagePath().equals(array.get(i)))
						cur = i;
				}
				

				ActivityGallery.start(v.getContext(), cur, array);

				
			} else {
				boolean old = clickview.getSel();
				
				clickview.setSel(!old);
				clickview.getData().sel = !old;
				UpdateSelectNum();

			}
		} else if (v instanceof Button) {
			ViewGroup parent = (ViewGroup) v.getParent();
			LineInfo tag = (LineInfo) parent.getTag();
			selGroup(tag.pos);

		}

	}

	@Override
	public boolean onLongClick(View v) {
		// TODO Auto-generated method stub
		SetEditMode(true);
		return false;
	}

	boolean checkGroupAllSel(int pos) {
		boolean sel_already = true;
		LineInfo info = mData.get(pos);
		for (int i = pos + 1; i < mData.size(); i++) {
			if (sel_already == true) {
				LineInfo info2 = mData.get(i);
				if (info2.date == info.date && info2.isTitle == false) {
					for (int j = 0; j < info2.imagearray.size(); j++) {

						if (info2.imagearray.get(j).sel == false) {
							sel_already = false;
							break;
						}
					}

				}
			} else
				break;
		}
		return sel_already;
	}
	public interface EditModeListener {
		public void onEditModeBegin();

		public void onEditModeFinish();

		public void onSelCountChange(int count);
	}
	private EditModeListener mEditModeListener = null;
	
	public void SetEditModeListener(EditModeListener listener) {
		mEditModeListener = listener;
	}
	public boolean GetEditMode() {
		return mEditMode;
	}

	public void SetEditMode(boolean mode) {
		if (mEditMode != mode) {
			if (mode == true) {
				mEditMode = true;
				UpdateSelectNum();
		

			} else {
				mEditMode = false;

				selAll(false);
			}
			for (int i = 0; i < mListView.get().getChildCount(); i++) {
				ViewGroup line = (ViewGroup) mListView.get()
						.getChildAt(i);
				LineInfo tag = (LineInfo) line.getTag();
				if (tag != null) {
					if (tag.isTitle) {
						
						line.findViewById(R.id.sel_all)
								.setVisibility(mEditMode == true ? View.VISIBLE
										: View.GONE);
						
						line.findViewById(R.id.count)
								.setVisibility(mEditMode == true ? View.GONE
										: View.VISIBLE);
					} else {
						for (int j = 0; j < mColum; j++) {
							IImageItem image = (IImageItem) line
									.getChildAt(j);
							image.setSelMode(mEditMode);
							if(mEditMode)
								image.setSel(false);
						}
					}
				}

			}
			if (mEditModeListener != null) {
				if (mEditMode) {
					mEditModeListener.onEditModeBegin();
				} else {
					mEditModeListener.onEditModeFinish();
				}
			}
		}
	}
	public int getSelCount() {
		int n= 0;
		for(LineInfo line:mData)
		{
			if(line.isTitle)
				continue;
			for(ImageData data:line.imagearray)
			{
				if(data.sel)
					n++;
			}
		}
		return n;
	}

	public ArrayList<String> GetSelectItems() {
		ArrayList<String> set = new ArrayList<String>();
		for(LineInfo line:mData)
		{
			if(line.isTitle)
				continue;
			for(ImageData data:line.imagearray)
			{
				if(data.sel)
					set.add(data.path);
			}
		}
		return set;
	}
	private void UpdateSelectNum() {
		int count = getSelCount();

		if (mEditModeListener != null) {
			mEditModeListener.onSelCountChange(count);
		}

		for (int i = 0; i < mListView.get().getChildCount(); i++) {
			ViewGroup v = (ViewGroup) mListView.get()
					.getChildAt(i);
			LineInfo tag = (LineInfo) v.getTag();
			if (tag.isTitle) {
				boolean all = checkGroupAllSel(tag.pos);
				
				updateSelButton(all,(Button)v.findViewById(R.id.sel_all));
			}
		}
	}
	void updateImage(String path) {
		for (int i = 0; i < mListView.get().getChildCount(); i++) {
			ViewGroup group = (ViewGroup) mListView.get()
					.getChildAt(i);
			LineInfo tag = (LineInfo) group.getTag();
			if (tag != null && tag.isTitle == false) {
				for (int j = 0; j < mColum; j++) {
					IImageItem view = (IImageItem) group.getChildAt(j);
					if (view.GetImagePath().equals(path)) {
						view.updateImage();
						return;
					}
				}
			}
		}
	}

	public void selAll(boolean sel) {
		for (int i = 0; i < mListView.get().getChildCount(); i++) {
			ViewGroup item = (ViewGroup) mListView.get()
					.getChildAt(i);
			LineInfo tag = (LineInfo) item.getTag();
			if (tag != null && tag.isTitle == false) {
				for (int j = 0; j < mColum; j++) {
					IImageItem imageview = (IImageItem) item
							.getChildAt(j);
					imageview.setSel(sel);

				}
			}

		}
		for (int i = 0; i < mData.size(); i++) {
			LineInfo info = mData.get(i);
			if (info.isTitle == false) {
				for (int j = 0; j < info.imagearray.size(); j++) {
					{
						info.imagearray.get(j).sel = sel;
					
					}
				}
			}

		}

		UpdateSelectNum();
	}

	void selGroup(int pos) {
		boolean sel_already = this.checkGroupAllSel(pos);
		LineInfo info = mData.get(pos);

		for (int i = 0; i < mListView.get().getChildCount(); i++) {
			ViewGroup item = (ViewGroup) mListView.get()
					.getChildAt(i);
			LineInfo tag = (LineInfo) item.getTag();
			if (tag != null && tag.isTitle == false
					&& tag.date == info.date) {
				for (int j = 0; j < mColum; j++) {
					IImageItem imageview = (IImageItem) item
							.getChildAt(j);
					imageview.setSel(!sel_already);
				}
			}

		}
		for (int i = pos + 1; i < mData.size(); i++) {
			LineInfo info2 = mData.get(i);
			if (info2.date == info.date && info2.isTitle == false) {
				for (int j = 0; j < info2.imagearray.size(); j++) {
					{
						info2.imagearray.get(j).sel = !sel_already;
					
					}
				}
			}

		}

		UpdateSelectNum();
	}
	public boolean haveData()
	{
		return !mData.isEmpty();
	}
	public int getCanselCount() {
		int n = 0;
		for (int i = 0; i < mData.size(); i++) {
			LineInfo info = mData.get(i);
			if (info.isTitle == false) {
				n+=info.imagearray.size();

			}

		}
		return n;
	}

}