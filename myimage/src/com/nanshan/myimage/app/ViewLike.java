package com.nanshan.myimage.app;

import java.util.ArrayList;
import java.util.Date;

import com.nanshan.myimage.ctrl.ListByTime;
import com.nanshan.myimage.data.ImageMgr;
import com.nanshan.myimage.data.ImageMgr.ImageInfo;
import com.nanshan.myimage.data.ImageMgr.ImageMgrListener;
import com.nanshan.myimage.data.ImageMgr.TimeInfo;
import com.nanshan.myimage.data.ImageMgr.change_type;

import android.content.Context;


public class ViewLike extends ListByTime implements ImageMgrListener {

	public ViewLike(Context context,android.util.AttributeSet attrs) {
		super(context,attrs);
		
		// TODO Auto-generated constructor stub
		
	}
	public void Init()
	{
		super.Init();
		ImageMgr.GetInstance().AddListener(this);
	
		RereshData();
	}
	@Override
	public void OnDataChange(ImageMgr.change_type type,int id) {
		// TODO Auto-generated method stub
		if(type == change_type.add || type == change_type.delete ||
				type == change_type.add_like || type==change_type.delete_like)
		RereshData();
		else if(type == change_type.rotate)
		{
			OnImageRotate(id);
		}

	}
	
	private void RereshData()
	{
		ArrayList<TimeInfo> arrayGroup = new ArrayList<TimeInfo>();
		TimeInfo group = null;
		ArrayList<ImageInfo> array = ImageMgr.GetInstance().GetLikeArray();
		for (int i = 0; i < array.size(); i++) {

			if (group == null) {
				group = ImageMgr.GetInstance().new TimeInfo();
				group.date = array.get(i).date;
				arrayGroup.add(group);
			} else {
				Date last = group.date;
				Date cur = array.get(i).date;
				if (last.getYear() != cur.getYear()
						|| last.getMonth() != cur.getMonth()
						|| last.getDay() != cur.getDay()) {
					group = ImageMgr.GetInstance().new TimeInfo();
					group.date = cur;
					arrayGroup.add(group);
				}

			}
			group.array.add(array.get(i));

		}
		this.SetGroups(arrayGroup);
	}

}