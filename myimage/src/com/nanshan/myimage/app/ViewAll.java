package com.nanshan.myimage.app;

import com.nanshan.myimage.ctrl.ListByTime;
import com.nanshan.myimage.data.ImageMgr;
import com.nanshan.myimage.data.ImageMgr.ImageMgrListener;
import com.nanshan.myimage.data.ImageMgr.change_type;

import android.content.Context;
import android.widget.ListView;

public class ViewAll extends ListByTime implements ImageMgrListener {

	public ViewAll(Context context,android.util.AttributeSet attrs) {
		super(context,attrs);
		
		// TODO Auto-generated constructor stub
		
	}
	public void Init()
	{
		super.Init();
		ImageMgr.GetInstance().AddListener(this);
		this.SetGroups(ImageMgr.GetInstance().GetGroups());
	}
	@Override
	public void OnDataChange(ImageMgr.change_type type,int id) {
		// TODO Auto-generated method stub
		if(type == change_type.add || type == change_type.delete)
		this.SetGroups(ImageMgr.GetInstance().GetGroups());

	}

}
