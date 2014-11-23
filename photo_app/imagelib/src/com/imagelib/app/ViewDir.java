package com.imagelib.app;

import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Map.Entry;

import com.imagelib.R;





import com.imagelib.ctrl.Folder_GridMode;
import com.imagelib.ctrl.Folder_ListMode;
import com.imagelib.ctrl.Folder_Mode;
import com.imagelib.data.ImageMgr;
import com.imagelib.data.ImageMgr.DirInfo;
import com.imagelib.data.ImageMgr.FileComparator;
import com.imagelib.data.ImageMgr.ImageInfo;
import com.imagelib.data.ImageMgr.ImageMgrListener;

import android.content.Context;
import android.content.Intent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.FrameLayout;
import android.widget.ListView;
import android.widget.RelativeLayout;

public class ViewDir extends FrameLayout implements ImageMgrListener {
	public static final int mode_list = 0;
	public static final int mode_grid = 1;
	private int mShowMode = mode_list; 
	private	Folder_Mode mList;
	
	public void setShowMode(int mode)
	{
		mShowMode = mode;
		if(mList != null)
		{
			this.removeView(mList.getView());
		}
		if(mShowMode == mode_list)
		{
			mList = new Folder_ListMode(this.getContext(),null);
		}
		else
		{
			mList = new Folder_GridMode(this.getContext(),null);
		}
		addView(mList.getView());
		mList.init(false);
		mList.setEmptyView(findViewById(R.id.empty));
	}
	public ViewDir(Context context, android.util.AttributeSet attrs) {
		super(context, attrs);
		// TODO Auto-generated constructor stub
		ImageMgr.GetInstance().addListener(this);

	}

	public void init() {
		setShowMode(mode_list);
		refreshData();
	}

	private void refreshData()
	{
		DirInfo dirinfo = ImageMgr.GetInstance().new DirInfo();
		dirinfo.dir = "/sdcard/DCIM/Camera";
		dirinfo.name = "Camera";
		dirinfo.array = new ArrayList<ImageInfo>();
		
		LinkedHashMap<String, DirInfo> map = new LinkedHashMap<String, DirInfo>();
		map.put("Camera", dirinfo);
		
		ArrayList<ImageInfo> array0 = ImageMgr.GetInstance().getImageArray();
		//Collections.sort(array0, ImageMgr.GetInstance().new FileComparator());
		for (int i = 0; i < array0.size(); i++) {
			ImageInfo info = array0.get(i);
			if (info == null)
				continue;

			int end = info.path.lastIndexOf(File.separator);
			int begin = info.path.lastIndexOf(File.separator, end - 1);
			String dirName = info.path.substring(begin + 1, end);
			String dirPath = info.path.substring(0, end);
			
			DirInfo dir = map.get(dirName);
			if (dir == null) {
				dir = ImageMgr.GetInstance().new DirInfo();
				dir.array = new ArrayList<ImageInfo>();
				dir.name = dirName;
				dir.dir = dirPath;
				
				map.put(dir.name, dir);
			}
			
			dir.array.add(info);
		}
		
		ArrayList<DirInfo> array = new ArrayList<DirInfo>();
		Iterator<Entry<String, DirInfo>> it = map.entrySet().iterator();
		while (it.hasNext()) {
			Entry<String, DirInfo> entry = it.next();
			if(entry.getValue().array.size() > 0)
			array.add(entry.getValue());
		}
		
		mList.setData(array);

	}
	@Override
	public void onImageMgrNotify(int type, Object path) {
		// TODO Auto-generated method stub
		if ( type == ImageMgr.delete_end|| type==ImageMgr.refresh) {
			refreshData();
		}

	}
	public void setSel(boolean sel)
	{
		if(sel)
		{
			
		}
	}
}
