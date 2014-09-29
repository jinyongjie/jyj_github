package com.example.myimage.app;

import java.util.ArrayList;

import com.example.myimage.ctrl.ListByTime;
import com.example.myimage.ctrl.AdapterTime.EditModeListener;
import com.example.myimage.data.ImageMgr;
import com.example.myimage.data.ImageMgr.ImageInfo;
import com.example.myimage.data.ImageMgr.ImageMgrListener;
import com.example.myimage.R;






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
		if(ImageMgr.GetInstance().getInitState() == ImageMgr.init_complete)
		{
			refreshData();
		}
		else
		{
			mList.showProgress(true);
		}
	}
	public void uninit()
	{
		ImageMgr.GetInstance().removeListener(this);
	}
	private void refreshData() {
		Log.d("refresh","Viewall");
		ArrayList<ImageInfo> array = ImageMgr.GetInstance().getImageArray();
		mList.setData(array);

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
