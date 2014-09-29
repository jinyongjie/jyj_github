package com.example.myimage.app;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;

import com.example.myimage.ctrl.ListByTime;
import com.example.myimage.ctrl.AdapterTime.EditModeListener;
import com.example.myimage.data.ImageMgr;
import com.example.myimage.data.ImageMgr.DirInfo;
import com.example.myimage.data.ImageMgr.FileComparator;
import com.example.myimage.data.ImageMgr.ImageInfo;
import com.example.myimage.data.ImageMgr.ImageMgrListener;
import com.example.myimage.data.ImageMgr.TimeInfo;
import com.example.myimage.R;
import com.example.myimage.R.layout;






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
	private String[] mArray;
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
		mArray = intent.getStringArrayExtra("array");
		

		ImageMgr.GetInstance().addListener(this);
		init(mArray);
	}

	private void init(String[] array) {
		mList.Init();
		mList.setEditModeListener(this);
		mList.setEnablePullRefresh(false);
		

		refreshData();
	}
	private void refreshData()
	{
		ArrayList<ImageInfo> array2 = new ArrayList<ImageInfo>();

		for (int i = 0; i < mArray.length; i++) {

			ImageInfo info = ImageMgr.GetInstance().getImage(mArray[i]);
			if(info != null)
			{
				array2.add(info);
			}
		}
		
		mTitle.setText(String.format("%s(%d)", mDirName, array2.size()));
		mList.setData(array2);
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
