package com.nanshan.myimage.app;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Map.Entry;

import com.nanshan.myimage.ctrl.ListByGroup;
import com.nanshan.myimage.data.ImageMgr;
import com.nanshan.myimage.data.ImageMgr.DirInfo;
import com.nanshan.myimage.data.ImageMgr.ImageInfo;
import com.nanshan.myimage.data.ImageMgr.ImageMgrListener;
import com.nanshan.myimage.data.ImageMgr.change_type;

import android.content.Context;
import android.widget.ListView;

public class ViewTag extends ListByGroup implements ImageMgrListener {

	public ViewTag(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
		ImageMgr.GetInstance().AddListener(this);
	}
	public void Init()
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
		this.SetData(array);
	}
	@Override
	public void OnDataChange(change_type type, int id) {
		// TODO Auto-generated method stub
		if(type == change_type.add ||
				type==change_type.delete||
				type==change_type.tag)
			
		{
			Init();
			
		}
	
	}

}
