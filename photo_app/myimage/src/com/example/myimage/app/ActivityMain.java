package com.example.myimage.app;

import java.io.File;
import java.util.Calendar;
import java.util.HashMap;
import java.util.Locale;










import com.example.myimage.R;

import android.net.Uri;
import android.opengl.GLES10;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Parcelable;
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
import android.support.v4.view.PagerAdapter;
import android.support.v4.view.ViewPager;
import android.support.v4.view.ViewPager.OnPageChangeListener;
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
import com.imagelib.app.ViewAll;
import com.imagelib.app.ViewDir;
import com.imagelib.app.ViewLike;
import com.imagelib.ctrl.AdapterTime.EditModeListener;
import com.imagelib.data.Helper;
import com.imagelib.data.ImageLoader2;
import com.imagelib.data.ImageMgr;
import com.imagelib.data.ImageMgr.ImageMgrListener;

public class ActivityMain extends Activity implements OnClickListener,
		EditModeListener, ImageMgrListener {

	private final static String TAG = "MainActivity";
	private View mBarBegin;


	private Button mButtonOp;
	private Button mButtonCamera;
	private Button mButtonMenu;
	private Button mButtonAll;
	private Button mButtonDir;
	private Button mButtonLike;
	private ViewPager mViewPager;

	private ViewAll mViewAll;
	private ViewDir mViewDir;
	private ViewLike mViewLike;
	private View mCurView;
	private HashMap<Integer, View> mViewMap = new HashMap<Integer, View>();

	private static final int view_all = 0;
	private static final int view_dir = 1;
	private static final int view_like = 2;

	public static final int req_capture = 0;

	private int mIndex = 0;

	private boolean mEditMode = false;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(R.layout.activity_main);

		initCtrl();
		initData();

	}

	private void initCtrl() {

		mBarBegin = findViewById(R.id.bar_top);

		mViewPager = (ViewPager) findViewById(R.id.viewpager);

		mButtonCamera = (Button) findViewById(R.id.button_camera);
		mButtonOp = (Button) findViewById(R.id.button_op);
		mButtonMenu = (Button) findViewById(R.id.button_menu);

		mButtonCamera.setOnClickListener(this);
		mButtonOp.setOnClickListener(this);
		mButtonMenu.setOnClickListener(this);
		mButtonAll = (Button) findViewById(R.id.button_all);
		mButtonAll.setOnClickListener(this);
		mButtonDir = (Button) findViewById(R.id.button_dir);
		mButtonDir.setOnClickListener(this);
		mButtonLike = (Button) findViewById(R.id.button_like);
		mButtonLike.setOnClickListener(this);

		// mGridView = (GridView) findViewById(R.id.gridView1);
		
		mViewPager.setOnPageChangeListener(new OnPageChangeListener() {

			@Override
			public void onPageScrollStateChanged(int arg0) {
				// TODO Auto-generated method stub

			}

			@Override
			public void onPageScrolled(int arg0, float arg1, int arg2) {
				// TODO Auto-generated method stub

			}

			@Override
			public void onPageSelected(int arg0) {
				// TODO Auto-generated method stub
				
				mIndex = arg0;
				updateButton();
			}

		});

	
		mViewPager.setAdapter(new MyAdapter(this));
		updateButton();
	}

	private void initData() {
		ImageMgr.GetInstance().addListener(this);

	}
	private void updateButton()
	{
		mButtonAll.setBackgroundResource(R.drawable.selector_button);
		mButtonDir.setBackgroundResource(R.drawable.selector_button);
		mButtonLike.setBackgroundResource(R.drawable.selector_button);
		mButtonAll.setTextColor(this.getResources().getColor(R.color.textWhite));
		mButtonDir.setTextColor(this.getResources().getColor(R.color.textWhite));
		mButtonLike.setTextColor(this.getResources().getColor(R.color.textWhite));
		
		if(mIndex == view_all)
		{
			mButtonAll.setBackgroundResource(R.color.button_down);
			mButtonAll.setTextColor(this.getResources().getColor(R.color.textBlue));
		}
			
		else if(mIndex == view_dir)
		{
				mButtonDir.setBackgroundResource(R.color.button_down);
			mButtonDir.setTextColor(this.getResources().getColor(R.color.textBlue));
		}
		
		else if(mIndex == view_like)
		{
			mButtonLike.setBackgroundResource(R.color.button_down);
			mButtonLike.setTextColor(this.getResources().getColor(R.color.textBlue));
		}
			
	}
	private class MyAdapter extends PagerAdapter {

		private LayoutInflater mInflater;

		public MyAdapter(Context context) {
			mInflater = LayoutInflater.from(context);
		}

		@Override
		public int getCount() {
			return 3;
		}

		@Override
		public Object instantiateItem(View arg0, int arg1) {

			View view = mViewMap.get(arg1);

			if (view == null) {
				if (arg1 == view_all) {

					// mViewAll = new ViewAll(this);

					mViewAll = (ViewAll) mInflater.inflate(R.layout.view_all,
							null);

					mViewAll.init();

					mViewAll.setEditModeListener(ActivityMain.this);

					view = mViewAll;

				} else if (arg1 == view_dir) {

					mViewDir = (ViewDir) mInflater.inflate(R.layout.view_dir,
							null);

					mViewDir.init();

					view = mViewDir;

				} else if (arg1 == view_like) {

					mViewLike = (ViewLike) mInflater.inflate(
							R.layout.view_like, null);

					mViewLike.init();

					mViewLike.setEditModeListener(ActivityMain.this);

					view = mViewLike;

				}
				mViewMap.put(arg1, view);
				((ViewPager) arg0).addView(view);
			}

			
			return view;
		}

		@Override
		public void destroyItem(View arg0, int arg1, Object arg2) {
			

		//	((ViewPager) arg0).removeView((View) arg2);

		}

		@Override
		public boolean isViewFromObject(View arg0, Object arg1) {
			return arg0 == arg1;
		}

		@Override
		public void restoreState(Parcelable arg0, ClassLoader arg1) {

		}

		@Override
		public Parcelable saveState() {
			return null;
		}

		@Override
		public void startUpdate(View arg0) {

		}

		@Override
		public void finishUpdate(View arg0) {

		}
	}

	private void showSettingMenu() {

		LayoutInflater inflater = LayoutInflater.from(this);

		View view = inflater.inflate(R.layout.menu_setting, null);

		view.measure(0, 0);

		final PopupWindow menu = new PopupWindow(view,
				LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT, false);

		menu.setBackgroundDrawable(new BitmapDrawable());

		menu.setFocusable(true);
		menu.setOutsideTouchable(true);
		menu.setOnDismissListener(new OnDismissListener() {

			@Override
			public void onDismiss() {
				// TODO Auto-generated method stub

			}

		});

		view.findViewById(R.id.button_refresh).setOnClickListener(
				new OnClickListener() {
					@Override
					public void onClick(View v) {
						// TODO Auto-generated method stub

						ImageMgr.GetInstance().refresh();

						menu.dismiss();

					}
				});

		menu.showAsDropDown(mButtonMenu, 0, -Helper.dp2px(this, 2) - 6);

	}

	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		int id = v.getId();
		if (id == R.id.button_camera) {

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

				Intent intent = Intent.createChooser(intent0, null);

				mCameraPicPath = f.getAbsolutePath();

				startActivityForResult(intent, req_capture);

			} catch (ActivityNotFoundException e) {
				e.printStackTrace();
			}

		} else if (id == R.id.button_op) {

			if (mIndex == view_all) {
				if (mViewAll.haveData())
					mViewAll.setEditMode(true);
			} else if (mIndex == view_like) {
				if (mViewLike.haveData())
					mViewLike.setEditMode(true);
			}

		} else if (id == R.id.button_menu) {

			{
				showSettingMenu();
			}
		}
		else if(id == R.id.button_all)
		{
			mViewPager.setCurrentItem(view_all);
		}
		else if(id == R.id.button_dir)
		{
			mViewPager.setCurrentItem(view_dir);
		}
		else if(id == R.id.button_like)
		{
			mViewPager.setCurrentItem(view_like);
		}
	}

	String mCameraPicPath;

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		// TODO Auto-generated method stub
		super.onActivityResult(requestCode, resultCode, data);
		if (requestCode == req_capture) {
			if (resultCode == Activity.RESULT_OK) {
				try {
					File f = new File(mCameraPicPath);
					if (f != null && f.exists() && f.isFile()) {
						ImageMgr.GetInstance().addImage(mCameraPicPath);
						
					}
				} catch (Exception e) {
					e.printStackTrace();
				}

			}
		}
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if (keyCode == KeyEvent.KEYCODE_BACK/* && event.getRepeatCount() == 0 */) {
			if (mIndex == view_all) {
				if (mViewAll.getEditMode() == true) {
					mViewAll.setEditMode(false);
					return true;
				}
			} else if (mIndex == view_like) {
				if (mViewLike.getEditMode() == true) {
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

		// GlobalManager.getInstance().uploadManager().stopNow();
		// GlobalManager.getInstance().destroyNow();
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
		if (mIndex == view_all)
			mViewAll.setEditMode(false);
		else if (mIndex == view_like)
			mViewLike.setEditMode(false);
	}

	@Override
	public void onImageMgrNotify(int type, Object path) {
		// TODO Auto-generated method stub
		if (type == ImageMgr.refresh) {

		} else if (type == ImageMgr.delete_begin) {

		} else if (type == ImageMgr.delete_end) {

		}
	}

}
