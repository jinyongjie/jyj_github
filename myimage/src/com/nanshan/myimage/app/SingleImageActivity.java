package com.nanshan.myimage.app;

import java.io.File;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map.Entry;

import com.nanshan.myimage.R;
import com.nanshan.myimage.ctrl.HackyViewPager;
import com.nanshan.myimage.ctrl.MySingleView;
import com.nanshan.myimage.ctrl.PhotoViewAttacher;
import com.nanshan.myimage.ctrl.SetTagDialog;
import com.nanshan.myimage.ctrl.PhotoViewAttacher.OnViewTapListener;
import com.nanshan.myimage.data.ImageLoader;
import com.nanshan.myimage.data.ImageLoader.ReleaseBitmapListener;
import com.nanshan.myimage.data.ImageMgr;
import com.nanshan.myimage.data.ImageLoader.ImageCallback;
import com.nanshan.myimage.data.ImageMgr.ImageInfo;
import com.nanshan.myimage.data.ImageMgr.ImageMgrListener;
import com.nanshan.myimage.data.ImageMgr.change_type;
import com.umeng.analytics.MobclickAgent;

import android.net.Uri;
import android.os.Bundle;
import android.os.Parcelable;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.AlertDialog.Builder;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Bitmap;
import android.support.v4.view.PagerAdapter;
import android.support.v4.view.ViewPager;
import android.support.v4.view.ViewPager.OnPageChangeListener;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.ProgressBar;

public class SingleImageActivity extends Activity implements OnClickListener ,ImageMgrListener,ReleaseBitmapListener{

	// private ImageView mImageView;
	private int mIndex;
	Button mButtonReturn;
	Button mButtonRotate;
	Button mButtonShow;
	Button mButtonDel;
	Button mButtonShare;
	Button mButtonTag;
	Button mButtonLike;
	View mBarTop;
	View mBarBottom;
	private HackyViewPager mViewPager;

	ArrayList<Integer> mArray;
	// private List<ImageView> mViews;
	private final int mViewCount = 10;
	LinkedHashMap<Integer, MySingleView> mViewMap;

	// private int mCurIndex;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		mViewMap = new LinkedHashMap<Integer, MySingleView>();
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_LAYOUT_NO_LIMITS);

		setContentView(R.layout.activity_single_image);

		mViewPager = (HackyViewPager) findViewById(R.id.vp);
		mButtonReturn = (Button) findViewById(R.id.button_return);
		mButtonShow = (Button) findViewById(R.id.button_show);
		mButtonRotate = (Button) findViewById(R.id.button_rotate);
		mButtonDel = (Button) findViewById(R.id.button_del);
		mButtonShare = (Button) findViewById(R.id.button_share);
		mButtonTag = (Button) findViewById(R.id.button_tag);
		mButtonLike = (Button) findViewById(R.id.button_like);
		mBarTop = findViewById(R.id.bar_top);
		mBarBottom = findViewById(R.id.bar_bottom);
		// mViewPager.setOnClickListener(this);
		mButtonReturn.setOnClickListener(this);
		mButtonShow.setOnClickListener(this);
		mButtonRotate.setOnClickListener(this);
		mButtonDel.setOnClickListener(this);
		mButtonShare.setOnClickListener(this);
		mButtonTag.setOnClickListener(this);
		mButtonLike.setOnClickListener(this);

		
		

		mArray = new ArrayList<Integer>();
		
		String path = null;
		Uri tmpUri = (Uri) this.getIntent().getData();
		if (tmpUri != null) {
			path = tmpUri.toString();
			mIndex = 0;
			
			
			int id = ImageMgr.GetInstance().AddImage(path);
		
			if(id == -1)
			{
				this.finish();
			}
			else mArray.add(id);
		} else {
			Intent intent = getIntent();
			mIndex = intent.getIntExtra("index", 0);
			
			int[] array = intent.getIntArrayExtra("array");
			for(int i = 0;i<array.length;i++)
			{
				mArray.add(array[i]);
			}
		}	

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
				UpdateCurImage();
				UpdateLikeState();
			}

		});
		
		mViewPager.setAdapter(new MyAdapter(this));
		mViewPager.setCurrentItem(mIndex);
	
		UpdateLikeState();
	}

	void UpdateCurImage()
	{
		MySingleView view = mViewMap.get(mIndex);
		if (view != null) {
			if (view.HaveImage() == false) {
				ClearAllBitmap();
				view.UpdateImage();//防内存不足
			}
		}
	}
	void UpdateLikeState()
	{
		int id = mArray.get(mIndex);
		ImageInfo info = ImageMgr.GetInstance().GetImage(id);
		if(info != null)
		{
			if(info.like == true)
				mButtonLike.setBackgroundResource(R.drawable.imagebrower_like_btn_press);
			else
				mButtonLike.setBackgroundResource(R.drawable.imagebrower_like_btn_normal);
		}
	}
	public void ClearAllBitmap() {
		Log.i("single","ClearAllBitmap");
		Iterator it = mViewMap.entrySet().iterator();
		while (it.hasNext()) {
			Entry<Integer, MySingleView> entry = (Entry<Integer, MySingleView>) it
					.next();
			entry.getValue().ReleaseBitmap();
		}
	}

	public int GetCurIndex() {
		return mIndex;
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		// getMenuInflater().inflate(R.menu.single_image, menu);
		return true;
	}

	private class MyAdapter extends PagerAdapter {

		private LayoutInflater mInflater;

		public MyAdapter(Context context) {
			mInflater = LayoutInflater.from(context);
		}

		@Override
		public int getCount() {
			return mArray.size();
		}

		@Override
		public Object instantiateItem(View arg0, int arg1) {
			// ImageView view = mViews.get(mCurIndex);

			MySingleView view = (MySingleView) mInflater.inflate(
					R.layout.view_single, null);
			((ViewPager) arg0).addView(view);

			// mCurId = arg1;
			view.Init(arg1,mViewTapListener);

			ImageInfo imageInfo = ImageMgr.GetInstance().GetImage(mArray.get(arg1));

			if(imageInfo != null)
			{
				DisplayMetrics dm = new DisplayMetrics();
				getWindowManager().getDefaultDisplay().getMetrics(dm);

				view.SetPath(imageInfo.id, imageInfo.path, dm.widthPixels,
						dm.heightPixels);

				mViewMap.put(arg1, view);
			}

			return view;
		}

		@Override
		public void destroyItem(View arg0, int arg1, Object arg2) {
			((MySingleView) arg2).Uninit();
			
			((ViewPager) arg0).removeView((View) arg2);
			mViewMap.remove(arg1);
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

	private void hideStatusBar() {
		WindowManager.LayoutParams attrs = getWindow().getAttributes();
		attrs.flags |= WindowManager.LayoutParams.FLAG_FULLSCREEN;
		getWindow().setAttributes(attrs);
	}

	private void showStatusBar() {
		WindowManager.LayoutParams attrs = getWindow().getAttributes();
		attrs.flags &= ~WindowManager.LayoutParams.FLAG_FULLSCREEN;
		getWindow().setAttributes(attrs);
	}

	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		// this.showStatusBar();
		if (v == mButtonReturn) {
			finish();
		} else if (v == mButtonRotate) {
			RotateImage();

		} else if (v == mButtonShow) {
			BeginShow();
		} else if (v == mButtonDel) {
			ShowDelDialog();
		} else if (v == mButtonShare) {
			ImageInfo info = ImageMgr.GetInstance().GetImage(mArray.get(mIndex));
			if(info != null && info.removed == false)
			{
				Intent intent = new Intent(Intent.ACTION_SEND);
				intent.setType("image/*");
				intent.putExtra(Intent.EXTRA_SUBJECT, "分享");
				//intent.putExtra(Intent.EXTRA_TEXT, "终于可以了!!!");
				File file = new File(info.path); 
				intent.putExtra(Intent.EXTRA_STREAM, Uri.fromFile(file));
				intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
				startActivity(Intent.createChooser(intent, getTitle()));	
			}

		} else if (v == mButtonLike) {
			AddOrDelLike();
		} else if (v == mButtonTag) {
			// AddTagDialog();

		}

	}
	void BeginShow()
	{
		int[] idarray = new int[mArray.size()-mIndex];
		int cur = 0;

		for (int i = mIndex; i < mArray.size(); i++) {
			idarray[i-mIndex] = mArray.get(i);
		}

		Intent intent = new Intent();
		intent.setClass(this, ShowActivity.class);
		intent.putExtra("array", idarray);
		
		

		startActivity(intent);
	}
	void RotateImage()
	{
	
		MySingleView view = mViewMap.get(mIndex);
		if (view != null) {
			view.RotateImage(); 
		}
	}
	protected void ShowDelDialog() {
		AlertDialog.Builder builder = new Builder(this);
		builder.setMessage(String.format("确定要删除吗?"));
		// builder.setTitle("提示");
		builder.setPositiveButton("确认", new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				dialog.dismiss();
				int id = mArray.get(mIndex);
				
				ImageMgr.GetInstance().RemoveImage(id);
			}

		});
		builder.setNegativeButton("取消", new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				dialog.dismiss();
			}
		});
		builder.create().show();
	}

	void AddOrDelLike()
	{
		int id = mArray.get(mIndex);
		ImageInfo info = ImageMgr.GetInstance().GetImage(id);
		if(info != null)
		{
			if(info.like == true)
			{
				ImageMgr.GetInstance().RemoveLikeImage(id);
			}
			else
			{
				ImageMgr.GetInstance().AddLikeImage(id);
			}
		}
	}
	void AddTagDialog()
	{
		final SetTagDialog dialog = new SetTagDialog(this,R.style.SetTagDlg);
		dialog.setContentView(R.layout.view_set_tag);
		dialog.setCancelable(false);
		

		dialog.show();
	}
	@Override
	public void OnDataChange(change_type type, int id) {
		// TODO Auto-generated method stub
		if(type == change_type.add_like)
		{
			int curid = mArray.get(mIndex);
			if(curid == id)
			{
				mButtonLike.setBackgroundResource(R.drawable.imagebrower_like_btn_press);
			}
		}
		else if(type == change_type.delete_like)
		{
			int curid = mArray.get(mIndex);
			if(curid == id)
			{
				mButtonLike.setBackgroundResource(R.drawable.imagebrower_like_btn_normal);
			}
		}
		else if(type == change_type.delete)
		{
			int curid = mArray.get(mIndex);
			ImageInfo info = ImageMgr.GetInstance().GetImage(curid);
			if(info == null || info.removed == true)
			{
				mArray.remove(mIndex);
				if(mArray.size() == 0)
				{
					this.finish();
				}
				else
				{
					if(mIndex >= mArray.size())
					{
						mIndex = mArray.size() -1;
						
					}
					mViewMap.clear();
					int oldindex = mIndex;
					mViewPager.setAdapter(new MyAdapter(this));
					mViewPager.setCurrentItem(oldindex);
									
					UpdateLikeState();
					
				}
			}

		}
	}

	@Override
	protected void onResume() {
		// TODO Auto-generated method stub
		super.onResume();
		ImageMgr.GetInstance().AddListener(this);
		MobclickAgent.onResume(this);
		ImageLoader.GetInstance().AddReleaseListener(this);
		this.UpdateCurImage();
	}

	@Override
	protected void onPause() {
		// TODO Auto-generated method stub
		super.onPause();
		ImageMgr.GetInstance().RemoveListener(this);
		MobclickAgent.onPause(this);
		ImageLoader.GetInstance().RemoveReleaseListener(this);
		ClearAllBitmap();
	}

	@Override
	public void ReleaseBitmap() {
		// TODO Auto-generated method stub
		Log.i("single","ReleaseBitmap");
		MySingleView view = mViewMap.get(mIndex);
		if (view != null) {
			if (view.HaveImage() == false) {
				ClearAllBitmap();
				
			}
		}
	}

	OnViewTapListener mViewTapListener = new OnViewTapListener() {
		public void onViewTap(View view, float x, float y) {
			// TODO Auto-generated method stub
			
				if (mButtonLike.getVisibility() == View.VISIBLE) {
					mButtonLike.setVisibility(View.INVISIBLE);
					mBarTop.setVisibility(View.INVISIBLE);
					mBarBottom.setVisibility(View.INVISIBLE);
				} else {
					mButtonLike.setVisibility(View.VISIBLE);
					mBarTop.setVisibility(View.VISIBLE);
					mBarBottom.setVisibility(View.VISIBLE);
				}
			
		}
	};

}
