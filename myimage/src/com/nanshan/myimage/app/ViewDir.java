package com.nanshan.myimage.app;

import java.util.ArrayList;
import java.util.Map.Entry;

import com.nanshan.myimage.ctrl.ListByGroup;
import com.nanshan.myimage.data.ImageMgr;
import com.nanshan.myimage.data.ImageMgr.DirInfo;
import com.nanshan.myimage.data.ImageMgr.ImageMgrListener;
import com.nanshan.myimage.data.ImageMgr.change_type;

import android.content.Context;
import android.widget.ListView;

public class ViewDir extends ListByGroup implements ImageMgrListener{

	public ViewDir(Context context,android.util.AttributeSet attrs) {
		super(context,attrs);
		// TODO Auto-generated constructor stub
		ImageMgr.GetInstance().AddListener(this);

	}
	public void Init()
	{
		Object[] list =  ImageMgr.GetInstance().GetDirArray().entrySet().toArray(); 
		ArrayList<DirInfo> array = new ArrayList<DirInfo>();
		for(int i= 0;i<list.length;i++)
		{
			Entry<String,DirInfo> entry = (Entry<String, DirInfo>) list[i];
			array.add(entry.getValue());
		}
		this.SetData(array);
	
	}
	@Override
	public void OnDataChange(ImageMgr.change_type type,int id) {
		// TODO Auto-generated method stub
		if(type == change_type.add || type == change_type.delete)
		{
			Init();
		}

	}
}
