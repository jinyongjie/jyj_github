package com.nanshan.myimage.app;

import java.io.File;
import java.util.Calendar;
import java.util.Locale;






import com.nanshan.myimage.R;
import com.nanshan.myimage.ctrl.ListByTime.EditModeListener;
import com.nanshan.myimage.data.ImageLoader;
import com.nanshan.myimage.data.ImageMgr;
import com.umeng.analytics.MobclickAgent;
import com.umeng.update.UmengUpdateAgent;

import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.app.Activity;
import android.content.ActivityNotFoundException;
import android.content.Context;
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

public class MainActivity extends Activity implements OnClickListener,
EditModeListener {

	View mBarBegin;
	
	PopupWindow mMenuMain;
	Button mButtonMainMenu;
	Button mButtonOp;
	Button mButtonCamera;

	View mMask;
	ViewGroup mViewContainer;


	Button mMenuItemAll;
	Button mMenuItemDir;
	Button mMenuItemLike;
	Button mMenuItemTag;
	ViewAll mViewAll;
	ViewDir mViewDir;
	ViewLike mViewLike;
	ViewTag mViewTag;
	View mCurView;
	enum ViewType
	{
		none,
		all,
		dir,
		like,
		tag,
	}
	ViewType mViewType = ViewType.none;
	int normal_left[] = { R.drawable.selector_button_all,
			R.drawable.selector_button_dir, R.drawable.selector_button_like,
			R.drawable.selector_button_tag };
	int hilight_left[] = { R.drawable.main_menu_icon_all_folder_title,
			R.drawable.main_menu_icon_folder_title,
			R.drawable.main_menu_icon_favorite_title,
			R.drawable.main_menu_icon_tags_title };
	int mButtonIndex = 0;
	int button_text[] = { R.string.menu_all, R.string.menu_dir,
			R.string.menu_like, R.string.menu_tag };
	Button buttons[];

	boolean mEditMode = false;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		UmengUpdateAgent.update(this);

		setContentView(R.layout.activity_main);


		/*
		 * try { getWindow().addFlags(
		 * WindowManager.LayoutParams.class.getField(
		 * "FLAG_NEEDS_MENU_KEY").getInt(null)); } catch (NoSuchFieldException
		 * e) { // Ignore since this field won't exist in most versions of
		 * Android Log.w(TAG,
		 * "cant find field FLAG_NEEDS_MENU_KEY at WindowManager.LayoutParams");
		 * } catch (IllegalAccessException e) { Log.w(TAG,
		 * "Could not access FLAG_NEEDS_MENU_KEY in addLegacyOverflowButton()",
		 * e); }
		 */
		InitCtrl();
		InitData();

	}

	private void InitCtrl() {
		mBarBegin = findViewById(R.id.bar_begin);
		
		mButtonMainMenu = (Button) findViewById(R.id.button_main_menu);
		mViewContainer = (ViewGroup) findViewById(R.id.view_container);
		mMask = findViewById(R.id.mask);
		mButtonCamera = (Button) findViewById(R.id.button_camera);
		mButtonOp = (Button) findViewById(R.id.button_op);


		mButtonMainMenu.setOnClickListener(this);
		mButtonCamera.setOnClickListener(this);
		mButtonOp.setOnClickListener(this);

		// mGridView = (GridView) findViewById(R.id.gridView1);
		
	}

	private void InitData() {
		ImageLoader.GetInstance().Init(this);
		new InitAsyncTask(this).execute(this);

	}

	public class InitAsyncTask extends AsyncTask<Context, Integer, String> {

		public InitAsyncTask(Context context) {

		}

		protected String doInBackground(Context... params) {
			// TODO Auto-generated method stub
			ImageMgr.GetInstance().Init(params[0]);
			return null;
		}

		@Override
		protected void onPostExecute(String result) {
			mMask.setVisibility(View.GONE);

			SetCurView(ViewType.all);
		}
	}

	private void ShowMainMenu() {
		if (mMenuMain == null) {
			LayoutInflater inflater = LayoutInflater.from(this);
			// 引入窗口配置文件
			View view = inflater.inflate(R.layout.view_main_menu, null);

			view.measure(0, 0);
			
			// 创建PopupWindow对象
			mMenuMain = new PopupWindow(view, LayoutParams.WRAP_CONTENT,
					LayoutParams.WRAP_CONTENT, false);

			mMenuMain.setBackgroundDrawable(new BitmapDrawable());
			// 设置此参数获得焦点，否则无法点击
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
			mMenuItemTag = (Button) view.findViewById(R.id.menu_tag);

			mMenuItemAll.setOnClickListener(new OnClickListener() {
				@Override
				public void onClick(View v) {
					// TODO Auto-generated method stub

					mMenuMain.dismiss();

					SetCurView(ViewType.all);
					setButtonIndex(0);
				}
			});
			mMenuItemDir.setOnClickListener(new OnClickListener() {
				@Override
				public void onClick(View v) {
					// TODO Auto-generated method stub
					mMenuMain.dismiss();
					SetCurView(ViewType.dir);

				}
			});
			mMenuItemLike.setOnClickListener(new OnClickListener() {
				@Override
				public void onClick(View v) {
					// TODO Auto-generated method stub
					mMenuMain.dismiss();
					SetCurView(ViewType.like);
				}
			});
			mMenuItemTag.setOnClickListener(new OnClickListener() {
				@Override
				public void onClick(View v) {
					// TODO Auto-generated method stub
					mMenuMain.dismiss();
					SetCurView(ViewType.tag);
				}
			});

		}
		mMenuMain.showAsDropDown(mButtonMainMenu, 0, 1);
	}

	private void SetCurView(ViewType type) {
		if (mViewType != type) {
			if (mCurView != null) {
				mCurView.setVisibility(View.INVISIBLE);
			}
			View view = null;
			int buttonIndex = 0;
			if (type == ViewType.all) {
				if (mViewAll == null) {
					// mViewAll = new ViewAll(this);

					mViewAll = (ViewAll) LayoutInflater.from(this).inflate(R.layout.view_all, null);
					
					mViewAll.Init();

					mViewAll.SetEditModeListener(this);
					mViewContainer.addView(mViewAll, LayoutParams.MATCH_PARENT,
							LayoutParams.MATCH_PARENT);
				}
				view = mViewAll;
				buttonIndex = 0;
				mButtonOp.setVisibility(View.VISIBLE);

			} else if (type == ViewType.dir) {
				if (mViewDir == null) {

					mViewDir = new ViewDir(this);
					mViewContainer.addView(mViewDir, LayoutParams.MATCH_PARENT,
							LayoutParams.MATCH_PARENT);
				}
				view = mViewDir;
				buttonIndex = 1;
				mButtonOp.setVisibility(View.GONE);
			} else if (type == ViewType.like) {
				if (mViewLike == null) {
					
					
					mViewLike = (ViewLike) LayoutInflater.from(this).inflate(R.layout.view_like, null);
					
					mViewLike.Init();

					mViewLike.SetEditModeListener(this);
					
					mViewContainer.addView(mViewLike,
							LayoutParams.MATCH_PARENT,
							LayoutParams.MATCH_PARENT);
				}

				view = mViewLike;
				buttonIndex = 2;
				mButtonOp.setVisibility(View.VISIBLE);
			} else if (type == ViewType.tag) {
				if (mViewTag == null) {
					mViewTag = (ViewTag) new View(this);
					mViewContainer.addView(mViewTag, LayoutParams.MATCH_PARENT,
							LayoutParams.MATCH_PARENT);
				}

				view = mViewTag;
				buttonIndex = 3;
				mButtonOp.setVisibility(View.GONE);
			}
			mCurView = view;
			if(mCurView != null)
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
			ShowMainMenu();
		} else if (v == mButtonCamera) {

			String sdStatus = Environment.getExternalStorageState();
			if (!sdStatus.equals(Environment.MEDIA_MOUNTED)) { // 检测sd是否可用
				Log.i("MainActivity",
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
				File f = new File(dir, name);
				mCameraPicPath = f.getAbsolutePath();
				intent.putExtra(MediaStore.EXTRA_OUTPUT, Uri.fromFile(f));
				intent.putExtra(MediaStore.EXTRA_VIDEO_QUALITY, 1);
				startActivityForResult(intent, 1);
			} catch (ActivityNotFoundException e) {
				e.printStackTrace();
			}

		} else if (v == mButtonOp) {
			mBarBegin.setVisibility(View.GONE);
			
			mViewAll.SetEditMode(true);
		} 
	}



	String mCameraPicPath;

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		// TODO Auto-generated method stub
		super.onActivityResult(requestCode, resultCode, data);
		if (resultCode == Activity.RESULT_OK) {
			// Bundle bundle = data.getExtras();

			// Bitmap bitmap = (Bitmap) bundle.get("data");//
			// 获取相机返回的数据，并转换为Bitmap图片格式
			ImageMgr.GetInstance().AddImage(mCameraPicPath);
		}
	}

	@Override
	public void OnFinishEditMode() {
		// TODO Auto-generated method stub
		mBarBegin.setVisibility(View.VISIBLE);
		
		mViewAll.SetEditMode(false);
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
