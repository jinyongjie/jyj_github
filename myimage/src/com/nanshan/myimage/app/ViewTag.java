package com.nanshan.myimage.app;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Map.Entry;

import com.nanshan.myimage.R;
import com.nanshan.myimage.ctrl.ListByGroup;
import com.nanshan.myimage.data.ImageMgr;
import com.nanshan.myimage.data.ImageMgr.DirInfo;
import com.nanshan.myimage.data.ImageMgr.ImageInfo;
import com.nanshan.myimage.data.ImageMgr.ImageMgrListener;
import com.nanshan.myimage.data.ImageMgr.change_type;

import android.content.Context;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.ListView;
import android.widget.TextView;

public class ViewTag extends FrameLayout implements ImageMgrListener {
	ListByGroup mList;
	TextView mTip;
	public ViewTag(Context context,android.util.AttributeSet attrs) {
		super(context,attrs);
		// TODO Auto-generated constructor stub
		ImageMgr.GetInstance().AddListener(this);
	}
	public void Init()
	{
		mList = (ListByGroup)findViewById(R.id.list);
		mTip=(TextView)findViewById(R.id.tip_no_tag);
		RefreshData();
		
	}
	@Override
	public void OnDataChange(change_type type, int id) {
		// TODO Auto-generated method stub
		if(type == change_type.add ||
				type==change_type.delete||
				type==change_type.tag)
			
		{
			RefreshData();
			
		}
	
	}
	void RefreshData()
	{
		ArrayList<DirInfo> array = new ArrayList<DirInfo>();
		int count = ImageMgr.GetInstance().GetImageCount();
		LinkedHashMap<String,DirInfo> map = new LinkedHashMap<String,DirInfo>();
		for(int i= 0;i<count;i++)
		{
			ImageInfo info = ImageMgr.GetInstance().GetImage(i);
			if(info == null)
				continue;
			if(info.tag == null || info.tag.length() == 0)
				continue;
			DirInfo dir = null;
			if(map.containsKey(info.tag) == false)
			{
				dir =ImageMgr.GetInstance().new DirInfo();
				dir.array = new ArrayList<ImageInfo>();
				dir.name = info.tag;
				map.put(info.tag, dir);
			
			}
			else
			{
				dir = map.get(info.tag);
			}
			dir.array.add(info);
		}
		Iterator<Entry<String,DirInfo>> it =	map.entrySet().iterator();
		while(it.hasNext())
		{
			Entry<String,DirInfo> entry = it.next();
			array.add(entry.getValue());
		}
		mList.SetData(array);
		if(array.size() == 0)
		{
			mTip.setVisibility(View.VISIBLE);
			mList.setVisibility(View.GONE);
		}
		else
		{
			mTip.setVisibility(View.GONE);
			mList.setVisibility(View.VISIBLE);
		}
	}
}
