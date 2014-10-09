package com.imagelib.app;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;

import com.imagelib.R;
import com.imagelib.R.layout;






import com.imagelib.ctrl.ListByTime;
import com.imagelib.ctrl.AdapterTime.EditModeListener;
import com.imagelib.data.ImageMgr;
import com.imagelib.data.ImageMgr.DirInfo;
import com.imagelib.data.ImageMgr.FileComparator;
import com.imagelib.data.ImageMgr.ImageInfo;
import com.imagelib.data.ImageMgr.ImageMgrListener;

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

public class ActivityDir extends Activity implements OnClickListener,
		ImageMgrListener, EditModeListener {
	private ListByTime mList;
	private Button mButtonReturn;
	private Button mButtonOp;
	private TextView mTitle;
	private String mDirName;
	private View mBarBegin;
	private static ArrayList<ImageInfo> mArray;
	public static void setArray(ArrayList<ImageInfo> array)
	{
		mArray = array;
	}
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(R.layout.activity_dir);
		mList = (ListByTime) findViewById(R.id.listbytime);
		mButtonReturn = (Button) findViewById(R.id.button_return);
		mTitle = (TextView) findViewById(R.id.title);
		mButtonOp = (Button) findViewById(R.id.button_op);
		mBarBegin = findViewById(R.id.bar_begin);

		mButtonReturn.setOnClickListener(this);
		mButtonOp.setOnClickListener(this);

		Intent intent = getIntent();
		mDirName = intent.getStringExtra("dir");
	
		

		ImageMgr.GetInstance().addListener(this);
		init();
	}

	private void init() {
		mList.Init();
		mList.setEditModeListener(this);
		mList.setEnablePullRefresh(false);
		

		refreshData();
	}
	private void refreshData()
	{
		mTitle.setText(String.format("%s(%d)", mDirName, mArray.size()));
		mList.setArray(mArray);
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
		} else if (v == mButtonOp) {
			
			mList.setEditMode(true);
		}
	}

	@Override
	public void onImageMgrNotify(int type, Object path) {
		// TODO Auto-generated method stub
		if ( type == ImageMgr.delete_end|| type==ImageMgr.refresh_end)

			refreshData();
	}



	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if (keyCode == KeyEvent.KEYCODE_BACK/* && event.getRepeatCount() == 0 */) {
			if (mList.GetEditMode() == true) {
				mList.setEditMode(false);
				return true;
			}
		}
		return super.onKeyDown(keyCode, event);
	}

	public void onResume() {
		super.onResume();
	
	}

	public void onPause() {
		super.onPause();
	
	}

	@Override
	public void onSelCountChange(int count) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onEditModeBegin() {
		// TODO Auto-generated method stub
		mBarBegin.setVisibility(View.GONE);
	}
	@Override
	public void onEditModeFinish() {
		// TODO Auto-generated method stub
		mBarBegin.setVisibility(View.VISIBLE);
		
	}
}
