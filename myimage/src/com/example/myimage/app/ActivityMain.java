package com.example.myimage.app;

import java.io.File;
import java.util.Calendar;
import java.util.Locale;








import com.example.myimage.ctrl.AdapterTime.EditModeListener;
import com.example.myimage.data.Helper;
import com.example.myimage.data.ImageLoader2;
import com.example.myimage.data.ImageMgr;
import com.example.myimage.data.ImageMgr.ImageMgrListener;
import com.example.myimage.R;










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
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.provider.MediaStore;
import android.text.format.DateFormat;
import android.util.Log;
import android.view.Gravity;
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

import com.example.myimage.R;




public class ActivityMain extends Activity implements OnClickListener,
		EditModeListener,ImageMgrListener{
    
    private long mLastUpdateRequest = 0;
    
	private final static String TAG="MainActivity";
	private View mBarBegin;

	private PopupWindow mMenuMain;
	private Button mButtonMainMenu;
	private Button mButtonOp;
	private Button mButtonCamera;
	private Button mButtonMenu;

	private ViewGroup mViewContainer;

	private ViewAll mViewAll;
	private ViewDir mViewDir;
	private ViewLike mViewLike;
	private View mCurView;

	enum ViewType {
		none, all, dir, like
	}

	public static final int req_capture = 0;
	public static final int req_login = 1;
	public static final int req_reg = 2;
	private ViewType mViewType = ViewType.none;
	private int normal_left[] = { R.drawable.selector_button_all,
			R.drawable.selector_button_dir, R.drawable.selector_button_like,
			  };
	private int hilight_left[] = { R.drawable.all_hover,
			R.drawable.album_hover,
			R.drawable.like_hover,
			 };
	private int mButtonIndex = 0;
	private int button_text[] = { R.string.menu_all, R.string.menu_dir,
			R.string.menu_like, R.string.menu_cloud };

	private boolean mEditMode = false;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
	

		setContentView(R.layout.activity_main);

		initCtrl();
		initData();
		


        
        Helper.statistic("http://dd.browser.360.cn/static/a/219.7293.gif?_" + String.valueOf(Math.random()));
	}

	private void initCtrl() {

		mBarBegin = findViewById(R.id.bar_begin);

		mButtonMainMenu = (Button) findViewById(R.id.button_main_menu);
		mViewContainer = (ViewGroup) findViewById(R.id.view_container);
		
		mButtonCamera = (Button) findViewById(R.id.button_camera);
		mButtonOp = (Button) findViewById(R.id.button_op);
		mButtonMenu = (Button)findViewById(R.id.button_menu);

		mButtonMainMenu.setOnClickListener(this);
		mButtonCamera.setOnClickListener(this);
		mButtonOp.setOnClickListener(this);
		mButtonMenu.setOnClickListener(this);


		// mGridView = (GridView) findViewById(R.id.gridView1);	
	}

	private void initData() {
		ImageMgr.GetInstance().addListener(this);
		setCurView(ViewType.all);
	
		int state = ImageMgr.GetInstance().getInitState();


		if(state == ImageMgr.init_no)
		{
			ImageMgr.GetInstance().init(this);
		
		}

			
	}



	private void showLeftMenu() {
		if (mMenuMain == null) {
			LayoutInflater inflater = LayoutInflater.from(this);
		
			View view = inflater.inflate(R.layout.menu_left, null);

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

					setMainButtonHover(false);
				}

			});


			view.findViewById(R.id.menu_all).setOnClickListener(new OnClickListener() {
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
			view.findViewById(R.id.menu_dir).setOnClickListener(new OnClickListener() {
				@Override
				public void onClick(View v) {
					// TODO Auto-generated method stub
					mMenuMain.dismiss();
					setCurView(ViewType.dir);

				}
			});
			view.findViewById(R.id.menu_like).setOnClickListener(new OnClickListener() {
				@Override
				public void onClick(View v) {
					// TODO Auto-generated method stub
					mMenuMain.dismiss();
					setCurView(ViewType.like);
				}
			});



		}
		mMenuMain.showAsDropDown(mButtonMainMenu, 0, -Helper.dp2px(this,2) - 6);
	//	mMenuMain.showAtLocation(((ViewGroup)findViewById(android.R.id.content)).getChildAt(0), Gravity.NO_GRAVITY, -30, mBarBegin.getMeasuredHeight());
		
	}

	private void showSettingMenu() {
		   
			LayoutInflater inflater = LayoutInflater.from(this);
		
			View view = inflater.inflate(R.layout.menu_setting, null);

			view.measure(0, 0);

			final PopupWindow		menu = new PopupWindow(view, LayoutParams.WRAP_CONTENT,
					LayoutParams.WRAP_CONTENT, false);

			menu.setBackgroundDrawable(new BitmapDrawable());
	
			menu.setFocusable(true);
			menu.setOutsideTouchable(true);
			menu.setOnDismissListener(new OnDismissListener() {

				@Override
				public void onDismiss() {
					// TODO Auto-generated method stub

				
				}

			});



			view.findViewById(R.id.button_refresh).setOnClickListener(new OnClickListener() {
				@Override
				public void onClick(View v) {
					// TODO Auto-generated method stub
				    
					ImageMgr.GetInstance().refresh();

					menu.dismiss();

					
				}
			});
			

		
		menu.showAsDropDown(mButtonMenu, 0, -Helper.dp2px(this,2) - 6);
	
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
	private void setMainButtonHover(boolean hover)
	{
		if(hover)
		{
			Drawable left = getResources().getDrawable(
					hilight_left[mButtonIndex]);
			left.setBounds(new Rect(0, 0, left.getMinimumWidth(), left
					.getMinimumHeight()));

			Drawable right = getResources().getDrawable(
					R.drawable.main_menu_arrow);
			right.setBounds(new Rect(0, 0, right.getMinimumWidth(), right
					.getMinimumHeight()));
			mButtonMainMenu.setCompoundDrawables(left, null, right, null);
			mButtonMainMenu.setTextColor(getResources().getColor(R.color.textBlue));
		}
		else
		{
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
			mButtonMainMenu.setTextColor(getResources().getColor(R.color.textWhite));
//			mButtonMainMenu.setText(button_text[mButtonIndex]);
		}
	}
	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		int id = v.getId();
		if (id == R.id.button_main_menu) {
			//Util.showToast(this, Helper.nativeMain());
			// TODO Auto-generated method stub
			setMainButtonHover(true);
			showLeftMenu();
		} else if (id == R.id.button_camera) {

			String sdStatus = Environment.getExternalStorageState();
			if (!sdStatus.equals(Environment.MEDIA_MOUNTED)) { //
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

				File f = new File(dir, name);
				Intent intent0 = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
				intent0.putExtra(MediaStore.EXTRA_OUTPUT, Uri.fromFile(f));
				intent0.putExtra(MediaStore.EXTRA_VIDEO_QUALITY, 1);
				
				Intent intent = Intent.createChooser(intent0,null);
				
			
				mCameraPicPath = f.getAbsolutePath();
				
				startActivityForResult(intent, req_capture);
				

			} catch (ActivityNotFoundException e) {
				e.printStackTrace();
			}

		} else if (id == R.id.button_op) {
		
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
		else if(id == R.id.button_menu)
		{

			{
				showSettingMenu();
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
						Helper.insertImage2MediaStore(this, mCameraPicPath);
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
		
		
	}

	public void onPause() {
		super.onPause();
	
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
		if(type == ImageMgr.refresh_begin)
		{
			
			
		}
		else if(type == ImageMgr.refresh_end)
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
