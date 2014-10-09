package com.example.myimage.app;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Map.Entry;

import com.example.myimage.ctrl.DialogChooseDir;

import com.example.myimage.ctrl.ListByGroup;
import com.example.myimage.data.Helper;
import com.example.myimage.data.ImageMgr;
import com.example.myimage.data.ImageLoader2.ReleaseBitmapListener;
import com.example.myimage.data.ImageMgr.DirInfo;
import com.example.myimage.data.ImageMgr.FileComparator;
import com.example.myimage.data.ImageMgr.ImageInfo;
import com.example.myimage.R;
import com.example.myimage.R.layout;

import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

public class ActivitySelDir extends Activity implements OnClickListener {
	private ListByGroup mList;
	private Button mButtonReturn;
	private Button mButtonConfirm;
	private Button mButtonNewDir;
	private ArrayList<String> mData;
	public final static int type_copy = 0;
	public final static int type_move = 1;
	private int mType = type_copy;
	private View mMask;
	private TextView mTip;
	private static final String TAG = "ActivitySelDir";
	private MyHandler mHandler ;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_sel_dir);

		mList = (ListByGroup) findViewById(R.id.list);
		mButtonReturn = (Button) findViewById(R.id.button_return);
		mButtonConfirm = (Button) findViewById(R.id.button_confirm);
		mButtonNewDir = (Button) findViewById(R.id.button_newdir);
		
		mButtonReturn.setOnClickListener(this);
		mButtonConfirm.setOnClickListener(this);
		mButtonNewDir.setOnClickListener(this);
		mMask = findViewById(R.id.mask);
		mTip = (TextView) findViewById(R.id.tip);
		
		mList.init(true);
		
		Intent intent = this.getIntent();
		mData = intent.getStringArrayListExtra("list");
		mType = intent.getIntExtra("type", type_copy);
		initData();
		mHandler = new MyHandler();
	}
	private void initData()
	{
		DirInfo dirinfo = ImageMgr.GetInstance().new DirInfo();
		dirinfo.dir = "/sdcard/DCIM/Camera";
		dirinfo.name = "Camera";
		dirinfo.array = new ArrayList<ImageInfo>();
		
		LinkedHashMap<String, DirInfo> map = new LinkedHashMap<String, DirInfo>();
		map.put("Camera", dirinfo);
		
		ArrayList<ImageInfo> array0 = ImageMgr.GetInstance().getImageArray();
		
		for (int i = 0; i < array0.size(); i++) {
			ImageInfo info = array0.get(i);
			if (info == null)
				continue;

			DirInfo dir = map.get(info.dirName);
			if (dir == null) {
				dir = ImageMgr.GetInstance().new DirInfo();
				dir.array = new ArrayList<ImageInfo>();
				dir.name = info.dirName;
				dir.dir = info.dirPath;
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
	public void onClick(View v) {
		// TODO Auto-generated method stub
		int id = v.getId();
		switch (id) {
		case R.id.button_return: {
			finish();
		}
			break;
		case R.id.button_confirm: {
			String dir = mList.getSelDir();
			if(dir != null)
			{
				this.copyOrMoveFiles(dir);
				
			
			}
			else 
			{
				Helper.showToast(this, R.string.sel_dir);
			}
		}
			break;
		case R.id.button_newdir: {
			
			DialogChooseDir dlg = new DialogChooseDir(this);
			dlg.setTitle("选择文件夹");
			dlg.setListener(new DialogChooseDir.Listener()
			{

				@Override
				public void onOk(String dir) {
					// TODO Auto-generated method stub
					if(dir != null )
					{
						dir.trim();
						if(dir.length() != 0)
						{
						
							File file = new File(dir);
							if(!file.isDirectory())
							{
								file.mkdir();

							}
							copyOrMoveFiles(dir);
								
						}
					}
				}
				
			});
			dlg.show();
		}
			break;
		default:
			break;
		}
	}
	


	private class MyHandler extends Handler {
		public void handleMessage(Message msg) {
		
				if(mType == type_copy)
				{
					mTip.setText(String.format("已复制 %d/%d",msg.what,mData.size() ));
				}
				else
				{
					mTip.setText(String.format("已移动 %d/%d",msg.what,mData.size() ));
				}

				


		}
	}
	private void copyOrMoveFiles(String dest)
	{
		new AsyncCopyMove().execute(dest);
	}
	private void copyOrMoveFilesRaw(String dest)
	{
		if(mData != null && mData.size() > 0)
		{
			int i = 0;
			for(String path:mData)
			{
				i++;
				File file = new File(path);
			
			
				if(file != null && file.exists() && file.isFile())
				{
					String newPath = dest + "/" + file.getName();
					File newfile = new File(newPath);
					if (!file.getAbsoluteFile().equals(
							newfile.getAbsoluteFile())) {
						if (mType == type_copy) {
							if (Helper.copyFile(path, newPath)) {
								Helper.insertImage2MediaStore(this, newPath);
							}
						}

						else if (mType == type_move)
						{
							boolean ret = Helper.moveFile(path, newPath);
							if(ret)
							{
								Helper.insertImage2MediaStore(this, newPath);
								boolean like = ImageMgr.GetInstance().isLike(path);
								if(like)
								{
									ImageMgr.GetInstance().setLike(newPath, true);
								}	
							}
						}
						Message msg = new Message();
						msg.what = i;
						mHandler.sendMessage(msg);
					}

				}
			}
		}
	}
	private class AsyncCopyMove extends AsyncTask<String, Integer, String> {

	    protected void onPreExecute() {
	    	if(mType == type_move)
	    	{
	    		mTip.setText("正在移动..");
	    	}
	    	else
	    	{
	    		mTip.setText("正在复制..");
	    	}
	    	mMask.setVisibility(View.VISIBLE);
	    	ImageMgr.GetInstance().setEnableRefresh(false);
	    	
	    }
		protected String doInBackground(String... params) {
			// TODO Auto-generated method stub
			copyOrMoveFilesRaw(params[0]);
			
			return null;
		}

		@Override
		protected void onPostExecute(String result) {
			mMask.setVisibility(View.GONE);
			if(mType == type_move)
			{
				Helper.showToast(ActivitySelDir.this, R.string.move_complete);
			}
			else
			{
				Helper.showToast(ActivitySelDir.this, R.string.copy_complete);
			}
			
			ImageMgr.GetInstance().setEnableRefresh(true);
			ImageMgr.GetInstance().refresh();
		
			
			finish();
		}
	}

}
