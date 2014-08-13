package com.nanshan.myimage.app;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;

import com.nanshan.myimage.R;
import com.nanshan.myimage.R.layout;
import com.nanshan.myimage.R.menu;
import com.nanshan.myimage.ctrl.ListByTime;
import com.nanshan.myimage.ctrl.ListByTime.EditModeListener;
import com.nanshan.myimage.data.ImageMgr;
import com.nanshan.myimage.data.ImageMgr.DirInfo;
import com.nanshan.myimage.data.ImageMgr.FileComparator;
import com.nanshan.myimage.data.ImageMgr.ImageInfo;
import com.nanshan.myimage.data.ImageMgr.ImageMgrListener;
import com.nanshan.myimage.data.ImageMgr.TimeInfo;
import com.nanshan.myimage.data.ImageMgr.change_type;
import com.umeng.analytics.MobclickAgent;

import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TextView;

public class DirActivity extends Activity implements OnClickListener,
		ImageMgrListener,EditModeListener {
	DirInfo mDirInfo;
	ListByTime mList;
	Button mButtonReturn;
	Button mButtonOp;
	TextView mTitle;
	String mDirName;
	View mBarBegin;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		
		setContentView(R.layout.activity_dir);
		mList = (ListByTime) findViewById(R.id.list);
		mButtonReturn = (Button) findViewById(R.id.button_return);
		mTitle = (TextView) findViewById(R.id.title);
		mButtonOp = (Button)findViewById(R.id.button_op);
		mBarBegin=findViewById(R.id.bar_begin);

		mButtonReturn.setOnClickListener(this);
		mButtonOp.setOnClickListener(this);

		Intent intent = getIntent();
		mDirName = intent.getStringExtra("dir");
		mDirInfo = ImageMgr.GetInstance().GetDirArray().get(mDirName);

		ImageMgr.GetInstance().AddListener(this);
		Init();
	}

	private void Init() {
		mList.Init();
		mList.SetEditModeListener(this);
		mTitle.setText(String.format("%s(%d)", mDirName, mDirInfo.array.size()));

		ArrayList<TimeInfo> GroupArray = new ArrayList<TimeInfo>();

		TimeInfo group = null;

		for (int i = 0; i < mDirInfo.array.size(); i++) {

			if (group == null) {
				group = ImageMgr.GetInstance().new TimeInfo();
				group.date = mDirInfo.array.get(i).date;
				GroupArray.add(group);
			} else {
				Date last = group.date;
				Date cur = mDirInfo.array.get(i).date;
				if (last.getYear() != cur.getYear()
						|| last.getMonth() != cur.getMonth()
						|| last.getDay() != cur.getDay()) {
					group = ImageMgr.GetInstance().new TimeInfo();
					group.date = cur;
					GroupArray.add(group);
				}

			}
			group.array.add(mDirInfo.array.get(i));

		}
		mList.SetGroups(GroupArray);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		// getMenuInflater().inflate(R.menu.dir, menu);
		return true;
	}

	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		if (v == mButtonReturn) {
			this.finish();
		}
		else if(v == mButtonOp)
		{
			mBarBegin.setVisibility(View.GONE);
			mList.SetEditMode(true);
		}
	}

	@Override
	public void OnDataChange(ImageMgr.change_type type,int id) {
		// TODO Auto-generated method stub
		if(type == change_type.add || type == change_type.delete)
			
		Init();
	}

	@Override
	public void OnFinishEditMode() {
		// TODO Auto-generated method stub
		mBarBegin.setVisibility(View.VISIBLE);
		mList.SetEditMode(false);
	}

	@Override
    public boolean onKeyDown(int keyCode, KeyEvent event) { 
        if (keyCode == KeyEvent.KEYCODE_BACK/* && event.getRepeatCount() == 0*/) { 
        	if(mBarBegin.getVisibility() == View.VISIBLE)
        	{
        		return super.onKeyDown(keyCode, event);
        	}
        	else
        	{
        		OnFinishEditMode();
        	}
        } 
        return false; 
    }
	public void onResume() {
		super.onResume();
		MobclickAgent.onResume(this);
		}
	public void onPause() {
		super.onPause();
		MobclickAgent.onPause(this);
		}
}
