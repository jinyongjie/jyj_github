package com.nanshan.myimage.app;

import java.io.File;
import java.util.Calendar;
import java.util.Locale;



import android.net.Uri;
import android.opengl.GLES10;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.provider.MediaStore;
import android.provider.MediaStore.Images.Media;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.content.ActivityNotFoundException;
import android.content.ContentValues;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Rect;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;

import android.text.format.DateFormat;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;
import android.widget.Button;
import android.widget.PopupWindow;
import android.widget.PopupWindow.OnDismissListener;
import android.widget.TextView;

import com.imagelib.app.ViewAll;
import com.imagelib.app.ViewDir;
import com.imagelib.app.ViewLike;
import com.imagelib.ctrl.AdapterTime.EditModeListener;
import com.imagelib.data.Helper;
import com.imagelib.data.ImageLoader2;
import com.imagelib.data.ImageMgr;
import com.imagelib.data.ImageMgr.ImageMgrListener;
import com.nanshan.myimage.R;
import com.umeng.analytics.MobclickAgent;
import com.umeng.update.UmengUpdateAgent;


public class ActivityMain extends Activity implements OnClickListener,
		EditModeListener,ImageMgrListener{
    
    private long mLastUpdateRequest = 0;
    
	private final static String TAG="MainActivity";
	private View mBarBegin;

	private PopupWindow mMenuMain;
	private Button mButtonMainMenu;
	private Button mButtonOp;
	private Button mButtonCamera;
	private Button mButtonLogout;
	private Button mButtonSelAll;

	private ViewGroup mViewContainer;

	private Button mMenuItemAll;
	private Button mMenuItemDir;
	private Button mMenuItemLike;

	private ViewAll mViewAll;
	private ViewDir mViewDir;
	private ViewLike mViewLike;
	
	private View mCurView;


	enum ViewType {
		none, all, dir, like, tag, yun,
	}

	public static final int req_capture = 0;
	public static final int req_login = 1;
	public static final int req_reg = 2;
	private ViewType mViewType = ViewType.none;
	private int normal_left[] = { R.drawable.selector_button_all,
			R.drawable.selector_button_dir, R.drawable.selector_button_like,
			};
	private int hilight_left[] = { R.drawable.main_menu_icon_all_folder_title,
			R.drawable.main_menu_icon_folder_title,
			R.drawable.main_menu_icon_favorite_title,
			R.drawable.main_menu_icon_tags_title };
	private int mButtonIndex = 0;
	private int button_text[] = { R.string.menu_all, R.string.menu_dir,
			R.string.menu_like, R.string.menu_tag, R.string.menu_yun };
	private Button buttons[];

	private boolean mEditMode = false;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		UmengUpdateAgent.update(this);

		setContentView(R.layout.activity_main);

		initCtrl();
		initData();
		

	}

	private void initCtrl() {

		mBarBegin = findViewById(R.id.bar_begin);

		mButtonMainMenu = (Button) findViewById(R.id.button_main_menu);
		mViewContainer = (ViewGroup) findViewById(R.id.view_container);
		
		mButtonCamera = (Button) findViewById(R.id.button_camera);
		mButtonOp = (Button) findViewById(R.id.button_op);
		mButtonLogout = (Button)findViewById(R.id.button_logout);

		mButtonMainMenu.setOnClickListener(this);
		mButtonCamera.setOnClickListener(this);
		mButtonOp.setOnClickListener(this);
		mButtonLogout.setOnClickListener(this);

		mButtonSelAll = (Button)findViewById(R.id.button_selall);

		// mGridView = (GridView) findViewById(R.id.gridView1);	
	}

	private void initData() {
		ImageMgr.GetInstance().addListener(this);
		setCurView(ViewType.all);
	


			
	}



	private void showMainMenu() {
		if (mMenuMain == null) {
			LayoutInflater inflater = LayoutInflater.from(this);
		
			View view = inflater.inflate(R.layout.menu_main, null);

			view.measure(0, 0);

			mMenuMain = new PopupWindow(view, LayoutParams.WRAP_CONTENT,
					LayoutParams.WRAP_CONTENT, false);

			mMenuMain.setBackgroundDrawable(new BitmapDrawable());
	
			mMenuMain.setFocusable(true);
			mMenuMain.setOutsideTouchable(true);
			mMenuMain.setOnDismissListener(new OnDismissListener() {

				@Override
				public void onDismiss() {
					// TODO Auto-generated method stub
					Drawable left = getResources().getDrawable(
							normal_left[mButtonIndex]);
					left.setBounds(new Rect(0, 0, left.getMinimumWidth(), left
							.getMinimumHeight()));

					Drawable right = getResources().getDrawable(
							R.drawable.main_menu_arrow);
					right.setBounds(new Rect(0, 0, right.getMinimumWidth(),
							right.getMinimumHeight()));
					mButtonMainMenu.setCompoundDrawables(left, null, right,
							null);

					mButtonMainMenu.setText(button_text[mButtonIndex]);

				}

			});
			mMenuItemAll = (Button) view.findViewById(R.id.menu_all);
			mMenuItemDir = (Button) view.findViewById(R.id.menu_dir);
			mMenuItemLike = (Button) view.findViewById(R.id.menu_like);
	

			mMenuItemAll.setOnClickListener(new OnClickListener() {
				@Override
				public void onClick(View v) {
					// TODO Auto-generated method stub
				    
			        //int x = 0;
			        //int y = 100 / x;

					mMenuMain.dismiss();

					setCurView(ViewType.all);
					setButtonIndex(0);
				}
			});
			mMenuItemDir.setOnClickListener(new OnClickListener() {
				@Override
				public void onClick(View v) {
					// TODO Auto-generated method stub
					mMenuMain.dismiss();
					setCurView(ViewType.dir);

				}
			});
			mMenuItemLike.setOnClickListener(new OnClickListener() {
				@Override
				public void onClick(View v) {
					// TODO Auto-generated method stub
					mMenuMain.dismiss();
					setCurView(ViewType.like);
				}
			});


		}
		mMenuMain.showAsDropDown(mButtonMainMenu, 0, 0);
	}

	private void setCurView(ViewType type) {
		if (mViewType != type) {
			if (mCurView != null) {
				mCurView.setVisibility(View.INVISIBLE);
			}
			mViewType = type;
			View view = null;
			int buttonIndex = 0;
			
			if (type == ViewType.all) {
				if (mViewAll == null) {
					// mViewAll = new ViewAll(this);

					mViewAll = (ViewAll) LayoutInflater.from(this).inflate(
							R.layout.view_all, null);

					mViewAll.init();

					mViewAll.setEditModeListener(this);
					mViewContainer.addView(mViewAll, LayoutParams.MATCH_PARENT,
							LayoutParams.MATCH_PARENT);
				}
				view = mViewAll;
				buttonIndex = 0;
				mButtonOp.setVisibility(View.VISIBLE);
				
				mButtonCamera.setVisibility(View.VISIBLE);

			} else if (type == ViewType.dir) {
				if (mViewDir == null) {

					mViewDir = (ViewDir) LayoutInflater.from(this).inflate(
							R.layout.view_dir, null);
					mViewContainer.addView(mViewDir, LayoutParams.MATCH_PARENT,
							LayoutParams.MATCH_PARENT);
					mViewDir.init();
					
				}
				view = mViewDir;
				buttonIndex = 1;
				mButtonOp.setVisibility(View.GONE);
			
				mButtonCamera.setVisibility(View.VISIBLE);
			} else if (type == ViewType.like) {
				if (mViewLike == null) {

					mViewLike = (ViewLike) LayoutInflater.from(this).inflate(
							R.layout.view_like, null);

					mViewLike.init();

					mViewLike.setEditModeListener(this);

					mViewContainer.addView(mViewLike,
							LayoutParams.MATCH_PARENT,
							LayoutParams.MATCH_PARENT);
				}

				view = mViewLike;
				buttonIndex = 2;
				mButtonOp.setVisibility(View.VISIBLE);
				
				mButtonCamera.setVisibility(View.VISIBLE);
			}

			mCurView = view;
			if (mCurView != null)
				mCurView.setVisibility(View.VISIBLE);
			setButtonIndex(buttonIndex);


			{
				mButtonSelAll.setVisibility(View.GONE);
				mButtonLogout.setVisibility(View.GONE);
			}
		}
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		// getMenuInflater().inflate(R.menu.main, menu);
		return true;

	}

	private void setButtonIndex(int index) {
		if (index != mButtonIndex) {
			Drawable left = getResources().getDrawable(normal_left[index]);
			left.setBounds(new Rect(0, 0, left.getMinimumWidth(), left
					.getMinimumHeight()));

			Drawable right = getResources().getDrawable(
					R.drawable.main_menu_arrow);
			right.setBounds(new Rect(0, 0, right.getMinimumWidth(), right
					.getMinimumHeight()));
			mButtonMainMenu.setCompoundDrawables(left, null, right, null);

			mButtonMainMenu.setText(button_text[index]);
			mButtonIndex = index;
		}
	}

	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		if (v == mButtonMainMenu) {
			// TODO Auto-generated method stub
			Drawable left = getResources().getDrawable(
					hilight_left[mButtonIndex]);
			left.setBounds(new Rect(0, 0, left.getMinimumWidth(), left
					.getMinimumHeight()));

			Drawable right = getResources().getDrawable(
					R.drawable.main_menu_arrow);
			right.setBounds(new Rect(0, 0, right.getMinimumWidth(), right
					.getMinimumHeight()));
			mButtonMainMenu.setCompoundDrawables(left, null, right, null);
			showMainMenu();
		} else if (v == mButtonCamera) {

			String sdStatus = Environment.getExternalStorageState();
			if (!sdStatus.equals(Environment.MEDIA_MOUNTED)) { // ���sd�Ƿ����
				Log.d("MainActivity",
						"SD card is not avaiable/writeable right now.");
				return;
			}
			try {
				File dir = new File(Environment.getExternalStorageDirectory()
						+ "/DCIM/Camera");
				if (!dir.exists())
					dir.mkdirs();

				String name = DateFormat.format("yyyyMMdd_hhmmss",
						Calendar.getInstance(Locale.CHINA))
						+ ".jpg";

				Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
				
				//Intent intent = Intent.createChooser(intent0,null);
				
				File f = new File(dir, name);
				mCameraPicPath = f.getAbsolutePath();
				intent.putExtra(MediaStore.EXTRA_OUTPUT, Uri.fromFile(f));
				intent.putExtra(MediaStore.EXTRA_VIDEO_QUALITY, 1);
				startActivityForResult(intent, req_capture);
				
				/*
				   Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);

			        Intent chooser = Intent.createChooser(intent,null);
			        startActivityForResult(chooser, req_capture);
				*/
			} catch (ActivityNotFoundException e) {
				e.printStackTrace();
			}

		} else if (v == mButtonOp) {
		
			if(mViewType == ViewType.all)
			{
				if(mViewAll.haveData())
				mViewAll.setEditMode(true);
			}
			else if(mViewType == ViewType.like)
			{
				if(mViewLike.haveData())
				mViewLike.setEditMode(true);
			}
			
			
		}

	}


	String mCameraPicPath;

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		// TODO Auto-generated method stub
		super.onActivityResult(requestCode, resultCode, data);
		if (requestCode == req_capture) {
			if (resultCode == Activity.RESULT_OK) {
				try{
					File f = new File(mCameraPicPath);
					if(f!=null && f.exists() && f.isFile())
					{
						ImageMgr.GetInstance().addImage(mCameraPicPath);
					//	this.getContentResolver().notifyChange(uri, null);
					}
				}
				catch(Exception e)
				{
					e.printStackTrace();
				}
				
				
			}
		}

	}



	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if (keyCode == KeyEvent.KEYCODE_BACK/* && event.getRepeatCount() == 0 */) {
			if(mViewType == ViewType.all)
			{
				if(mViewAll.getEditMode() == true)
				{
					mViewAll.setEditMode(false);
					return true;
				}
			}
			else if(mViewType == ViewType.like)
			{
				if(mViewLike.getEditMode() == true)
				{
					mViewLike.setEditMode(false);
					return true;
				}
			}
		}
		return super.onKeyDown(keyCode, event);
	}

	public void onResume() {
		super.onResume();
		
		MobclickAgent.onResume(this);

	}

	public void onPause() {
		super.onPause();
		MobclickAgent.onPause(this);

	}	

	@Override
	protected void onStop() {
		// TODO Auto-generated method stub
		super.onStop();
		
		//GlobalManager.getInstance().uploadManager().stopNow();
		//GlobalManager.getInstance().destroyNow();
	}

	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		ImageLoader2.GetInstance().clearCache();
		

	        
		super.onDestroy();
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
		if(mViewType == ViewType.all)
			mViewAll.setEditMode(false);
		else if(mViewType == ViewType.like)
			mViewLike.setEditMode(false);
	}

	@Override
	public void onImageMgrNotify(int type, Object path) {
		// TODO Auto-generated method stub
		if(type == ImageMgr.refresh)
		{
			
		}
		else if(type == ImageMgr.delete_begin)
		{
		
		}
		else if(type == ImageMgr.delete_end)
		{
			
		}
	}


}
