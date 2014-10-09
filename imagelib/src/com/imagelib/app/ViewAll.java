package com.imagelib.app;

import java.util.ArrayList;

import com.imagelib.R;







import com.imagelib.ctrl.ListByTime;
import com.imagelib.ctrl.AdapterTime.EditModeListener;
import com.imagelib.data.ImageMgr;
import com.imagelib.data.ImageMgr.ImageInfo;
import com.imagelib.data.ImageMgr.ImageMgrListener;
import com.imagelib.data.ImageMgr.LineInfo;

import android.content.Context;
import android.util.Log;
import android.widget.ListView;
import android.widget.RelativeLayout;

public class ViewAll extends RelativeLayout implements ImageMgrListener {

	private ListByTime mList;

	public ViewAll(Context context, android.util.AttributeSet attrs) {
		super(context, attrs);

		// TODO Auto-generated constructor stub

	}

	public void init() {
		mList = (ListByTime) findViewById(R.id.listbytime);
		mList.Init();
		mList.setEnablePullRefresh(false);
		mList.setEmptyText(R.string.no_image);
		

		ImageMgr.GetInstance().addListener(this);
		//if(ImageMgr.GetInstance().getInitState() == ImageMgr.init_complete)
		{
			refreshData();
		}

	}
	public void uninit()
	{
		ImageMgr.GetInstance().removeListener(this);
	}
	private void refreshData() {
		
		ArrayList<LineInfo> array = ImageMgr.GetInstance().getLineInfoArray();
		mList.setLineInfo(array);

	}

	@Override
	public void onImageMgrNotify(int type, Object path) {
		// TODO Auto-generated method stub
		if ( type == ImageMgr.delete_end
				|| type == ImageMgr.refresh_end) {
			refreshData();
		}


	}

	public void setEditModeListener(EditModeListener listener) {
		// TODO Auto-generated method stub
		mList.setEditModeListener(listener);
	}

	public boolean haveData() {
		// TODO Auto-generated method stub
		return mList.haveData();
	}

	public void setEditMode(boolean b) {
		// TODO Auto-generated method stub
		mList.setEditMode(b);
	}

	public boolean getEditMode() {
		// TODO Auto-generated method stub
		return mList.GetEditMode();
	}

}
