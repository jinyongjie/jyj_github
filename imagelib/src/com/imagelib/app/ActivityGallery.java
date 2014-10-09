package com.imagelib.app;

import java.io.File;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map.Entry;

import com.imagelib.R;






import com.imagelib.ctrl.ViewSingle;
import com.imagelib.data.Helper;
import com.imagelib.data.ImageLoader2;
import com.imagelib.data.ImageMgr;
import com.imagelib.data.ImageLoader2.ImageCallback;
import com.imagelib.data.ImageLoader2.ReleaseBitmapListener;
import com.imagelib.data.ImageMgr.ImageInfo;
import com.imagelib.data.ImageMgr.ImageMgrListener;
import com.imagelib.photoview.HackyViewPager;
import com.imagelib.photoview.PhotoViewAttacher;
import com.imagelib.photoview.PhotoViewAttacher.OnViewTapListener;

import android.media.ExifInterface;
import android.net.Uri;
import android.os.Bundle;
import android.os.Parcelable;
import android.provider.MediaStore.Images.Media;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.AlertDialog.Builder;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.support.v4.view.PagerAdapter;
import android.support.v4.view.ViewPager;
import android.support.v4.view.ViewPager.OnPageChangeListener;
import android.util.DisplayMetrics;
import android.util.Log;
import android.util.Pair;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.webkit.MimeTypeMap;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.PopupWindow;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.LinearLayout.LayoutParams;

public class ActivityGallery extends Activity implements OnClickListener,
		ImageMgrListener, ReleaseBitmapListener {

	// private ImageView mImageView;
	private static int mIndex;
	private static ArrayList<String> mArray;
	private Button mButtonReturn;
	private Button mButtonRotate;
	private Button mButtonShow;
	private Button mButtonDel;
	private Button mButtonShare;
	private Button mButtonLike;
	private TextView mTitle;
	private View mTip;

	private View mBarTop;
	private View mBarBottom;
	private HackyViewPager mViewPager;

	
	// private List<ImageView> mViews;
	private final int mViewCount = 10;
	private LinkedHashMap<Integer, ViewSingle> mViewMap;

	private int mDefaultWidth = 0;
	private int mDefaultHeight = 0;
	// private int mCurIndex;
	private final static String TAG = "SingleActivity";

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		ImageLoader2.GetInstance().init(this);// 有可能外部调用打开图片,故需要init
		ImageMgr.GetInstance().init(this);
		

		super.onCreate(savedInstanceState);

		DisplayMetrics dm = new DisplayMetrics();
		getWindowManager().getDefaultDisplay().getMetrics(dm);
		mDefaultWidth = dm.widthPixels;
		mDefaultHeight = dm.heightPixels;

		mViewMap = new LinkedHashMap<Integer, ViewSingle>();
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_LAYOUT_NO_LIMITS);

		setContentView(R.layout.activity_gallery);

		mViewPager = (HackyViewPager) findViewById(R.id.vp);
		mButtonReturn = (Button) findViewById(R.id.button_return);
		mButtonShow = (Button) findViewById(R.id.button_show);
		mButtonRotate = (Button) findViewById(R.id.button_rotate);

		mButtonDel = (Button) findViewById(R.id.button_del);
		mButtonShare = (Button) findViewById(R.id.button_share);
		mButtonLike = (Button) findViewById(R.id.button_like);

		mTip = findViewById(R.id.tip);

		mBarTop = findViewById(R.id.bar_top);
		mBarBottom = findViewById(R.id.bar_bottom);
		mTitle = (TextView) findViewById(R.id.title);

		// mViewPager.setOnClickListener(this);
		mButtonReturn.setOnClickListener(this);
		mButtonShow.setOnClickListener(this);
		mButtonRotate.setOnClickListener(this);
		mButtonDel.setOnClickListener(this);
		mButtonShare.setOnClickListener(this);
		mButtonLike.setOnClickListener(this);
		
		findViewById(R.id.button_more).setOnClickListener(this);

		String path = null;
		
		Intent intent = getIntent();
		if(!intent.hasExtra("self"))
		{
			Uri tmpUri = (Uri) this.getIntent().getData();
			if (tmpUri != null) {
				String scheme = tmpUri.getScheme();
				if (scheme.equals("file")) {
					path = tmpUri.getPath();
				} else if (scheme.equals("content")) {
					Cursor cursor = this.getContentResolver().query(tmpUri, null,
							null, null, null);
					if (cursor != null) {
						int index = cursor.getColumnIndex(Media.DATA);

						cursor.moveToFirst();
						path = cursor.getString(index);

					}
				}

			}
			if (path == null) {
			

				Bundle bundle = this.getIntent().getExtras();
				if (bundle != null) {
					path = bundle.getString("PATH");
				}
				// file:///storage/emulated/0/DCIM/Camera/20140823_125052.jpg

			}
			mArray = new ArrayList<String>();
			mIndex = 0;
			if (path != null) {
				if (path.toLowerCase().startsWith("file://")) {
					path = path.substring(7);
				}
				mIndex = 0;
				mArray.add(path);
			}
		}
		
		
		

		if (path == null && mArray.size() == 0) {
			Helper.showToast(this, "参数错误,按返回键退出");
			mBarTop.setVisibility(View.VISIBLE);
			mButtonShow.setVisibility(View.GONE);
		} else {
			mBarTop.setVisibility(View.GONE);
			mButtonShow.setVisibility(View.VISIBLE);
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
				Log.d(TAG, String.format("OnPageSelected %d", arg0));
				mIndex = arg0;
				UpdateCurImage();
				UpdateButtonState();
			}

		});

		Log.d(TAG, String.format("OnCreate 3 %d %d", mIndex, mArray.size()));
		mViewPager.setAdapter(new MyAdapter(this));
		mViewPager.setCurrentItem(mIndex);

		UpdateButtonState();
	}
	public static void start(Context context,int index,ArrayList<String> array)
	{
		mArray = array;
		mIndex = index;
		Intent intent = new Intent(context,ActivityGallery.class);
		intent.putExtra("self", 1);
		context.startActivity(intent);
	}
	private void UpdateCurImage() {
		ViewSingle view = mViewMap.get(mIndex);
		if (view != null) {
			if (view.HaveImage() == false) {
				view.UpdateImage();// ���ڴ治��
			}
		}
	}

	private void UpdateButtonState() {
		if (mArray == null || mArray.size() == 0)
			return;
		String path = mArray.get(mIndex);
		
	boolean like = ImageMgr.GetInstance().isLike(path);
			if (like) {
				Drawable d = getResources().getDrawable(R.drawable.like_ic_1);
				d.setBounds(0, 0, d.getMinimumWidth(), d.getMinimumHeight());
				mButtonLike.setCompoundDrawables(null, d, null, null);
				// mTextLike.setText("取消喜欢");
			}

			else {
				Drawable d = getResources().getDrawable(R.drawable.like_normal);
				d.setBounds(0, 0, d.getMinimumWidth(), d.getMinimumHeight());
				mButtonLike.setCompoundDrawables(null, d, null, null);

				// mTextLike.setText("喜欢");
			}

		
		mTitle.setText(String.format("%d/%d", mIndex + 1, mArray.size()));
	}

	public void ClearAllBitmap() {

		Iterator it = mViewMap.entrySet().iterator();
		while (it.hasNext()) {
			Entry<Integer, ViewSingle> entry = (Entry<Integer, ViewSingle>) it
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
			Log.d(TAG, String.format("inst %d", arg1));
			ViewSingle view = (ViewSingle) mInflater.inflate(
					R.layout.view_single, null);
			((ViewPager) arg0).addView(view);

			view.Init(mViewTapListener, mDefaultWidth, mDefaultHeight,
					mArray.get(arg1));
			mViewMap.put(arg1, view);

			return view;
		}

		@Override
		public void destroyItem(View arg0, int arg1, Object arg2) {
			((ViewSingle) arg2).Uninit();

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

	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		// this.showStatusBar();
		int id = v.getId();
		if (id == R.id.button_return) {
			finish();
		} else if (id == R.id.button_rotate) {
			RotateImage();

		} else if (id == R.id.button_show) {
			BeginShow();
		} else if (id == R.id.button_del) {
			ShowDelDialog();
		} else if (id == R.id.button_share) {
			ImageInfo info = ImageMgr.GetInstance()
					.getImage(mArray.get(mIndex));
			if (info != null) {
				Intent intent = new Intent(Intent.ACTION_SEND);
				intent.setType("image/*");
				intent.putExtra(Intent.EXTRA_SUBJECT, "分享");

				File file = new File(info.path);
				intent.putExtra(Intent.EXTRA_STREAM, Uri.fromFile(file));
				intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
				startActivity(Intent.createChooser(intent, getTitle()));
			}

		} else if (id == R.id.button_like) {
			AddOrDelLike();
		} else if (id == R.id.button_more) {
			showMoreMenu(mBarBottom);
		} else if (id == R.id.button_copy) {
			Intent intent = new Intent();
			intent.setClass(this, ActivitySelDir.class);

			intent.putExtra("type", ActivitySelDir.type_copy);
			ArrayList<String> list = new ArrayList<String>();
			list.add(mArray.get(mIndex));
			intent.putStringArrayListExtra("list", list);
			this.startActivity(intent);

			mOpMenu.dismiss();
		} else if (id == R.id.button_move) {
			Intent intent = new Intent();
			intent.setClass(this, ActivitySelDir.class);

			intent.putExtra("type", ActivitySelDir.type_move);
			ArrayList<String> list = new ArrayList<String>();
			list.add(mArray.get(mIndex));
			intent.putStringArrayListExtra("list", list);
			this.startActivity(intent);

			mOpMenu.dismiss();
		} else if (id == R.id.button_info) {
			ExifInterface exif = null;
			try {
				exif = new ExifInterface(mArray.get(mIndex));
			} catch (IOException e) {
				e.printStackTrace();
				exif = null;
			}

			BitmapFactory.Options options = new BitmapFactory.Options();
			options.inJustDecodeBounds = true;
			BitmapFactory.decodeFile(mArray.get(mIndex), options);

			File f = new File(mArray.get(mIndex));

			AlertDialog.Builder builder = new Builder(this);
			String info = String.format("路径:%s" + "\n大小:%.2fM" + "\n尺寸:%dx%d",
					f.getAbsoluteFile(), (float) f.length() / (1024 * 1024),
					options.outWidth, options.outHeight);

			String time = null;

			if (exif != null) {
				String make = exif.getAttribute(ExifInterface.TAG_MAKE);
				if (make != null) {
					info += ("\n相机:" + make);
				}

				time = exif.getAttribute(ExifInterface.TAG_DATETIME);
				if (time != null) {
					byte[] bytes = time.getBytes();
					if (bytes.length > 7 && bytes[4] == ':' && bytes[7] == ':') {
						bytes[4] = '-';
						bytes[7] = '-';
						String time2 = new String(bytes);
						info += ("\n拍摄时间:" + time2);
					} else
						time = null;

				}
			}

			if (time == null) {
				SimpleDateFormat sd = new SimpleDateFormat(
						"yyyy-MM-dd hh:mm:ss");

				Date date = new Date(f.lastModified());

				info += ("\n修改时间:" + sd.format(date));
			}
			builder.setMessage(info);
			builder.setIcon(R.drawable.framework_infobtn_normal);
			builder.setTitle("详细信息");
			builder.setPositiveButton("确定",
					new DialogInterface.OnClickListener() {
						@Override
						public void onClick(DialogInterface dialog, int which) {
							dialog.dismiss();
						}

					});

			builder.create().show();
			mOpMenu.dismiss();
		} else if (id == R.id.button_wallpaper) {

			File file = new File(mArray.get(mIndex));
			Intent intent = createSetAsIntent(Uri.fromFile(file), null);
			intent.setAction(Intent.ACTION_ATTACH_DATA);
			intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
//			startActivity(Intent.createChooser(intent, "设置壁纸"));
			
			this.startActivity(intent);

		}
	 else if (id == R.id.button_edit) {

		File file = new File(mArray.get(mIndex));
		Intent intent = createSetAsIntent(Uri.fromFile(file), null);
		intent.setAction(Intent.ACTION_EDIT);
		intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
		startActivity(intent);

	}
	}

	public static Intent createSetAsIntent(Uri uri, String mimeType) {
		// Infer MIME type if missing for file URLs.
		if (uri.getScheme().equals("file")) {
			String path = uri.getPath();
			int lastDotIndex = path.lastIndexOf('.');
			if (lastDotIndex != -1) {
				mimeType = MimeTypeMap.getSingleton()
						.getMimeTypeFromExtension(
								uri.getPath().substring(lastDotIndex + 1)
										.toLowerCase());
			}
		}

		Intent intent = new Intent();
		intent.setDataAndType(uri, mimeType);
		intent.putExtra("mimeType", mimeType);
		return intent;
	}


	private PopupWindow mOpMenu = null;

	public void showMoreMenu(View v) {

		if (mOpMenu == null) {
			ViewGroup view = null;
			LayoutInflater inflater = LayoutInflater.from(this);

			view = (ViewGroup) inflater.inflate(R.layout.menu_bottom, null);

			mOpMenu = new PopupWindow(view, LayoutParams.MATCH_PARENT,
					LayoutParams.WRAP_CONTENT, false);

			mOpMenu.setBackgroundDrawable(new BitmapDrawable());

			mOpMenu.setFocusable(true);
			mOpMenu.setOutsideTouchable(true);

			view.findViewById(R.id.button_info).setOnClickListener(this);
			view.findViewById(R.id.button_copy).setOnClickListener(this);

			view.findViewById(R.id.button_move).setOnClickListener(this);
			view.findViewById(R.id.button_wallpaper).setOnClickListener(this);
			view.findViewById(R.id.button_edit).setOnClickListener(this);

			view.measure(0, 0);
		}

		mOpMenu.showAtLocation(
				((ViewGroup) findViewById(android.R.id.content)).getChildAt(0),
				Gravity.NO_GRAVITY, 0, (int) (mBarBottom.getTop()
						- mOpMenu.getContentView().getMeasuredHeight() - 1));
	}

	private void BeginShow() {

		Intent intent = new Intent();
		intent.setClass(this, ActivityPlay.class);
		ActivityPlay.setArray(mArray);

		startActivityForResult(intent, 1);
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		// TODO Auto-generated method stub
		super.onActivityResult(requestCode, resultCode, data);
		if (requestCode == 1 && resultCode == 1 && data != null) {
			String path = data.getStringExtra("path");
			if (path != null) {
				for (int i = 0; i < mArray.size(); i++) {
					if (mArray.get(i).equals(path)) {
						mViewPager.setCurrentItem(i);
						break;
					}
				}
			}

		}
	}

	private void RotateImage() {

		ViewSingle view = mViewMap.get(mIndex);
		if (view != null) {
			view.RotateImage();
		}
	}

	protected void ShowDelDialog() {
		AlertDialog.Builder builder = new Builder(this);
		builder.setMessage(String.format("确定要删除吗?"));

		builder.setPositiveButton("确定", new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				dialog.dismiss();
				String path = mArray.get(mIndex);

				ImageMgr.GetInstance().deleteImage(path);
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

	private void AddOrDelLike() {
		String path = mArray.get(mIndex);
	
		boolean like = ImageMgr.GetInstance().isLike(path);
		
			ImageMgr.GetInstance().setLike(path, !like);
		
	}



	@Override
	public void onImageMgrNotify(int type, Object path) {
		// TODO Auto-generated method stub
		if (mArray.size() == 0 || mIndex >= mArray.size())
			return;
		String curPath = mArray.get(mIndex);
		if (type == ImageMgr.like) {
			this.UpdateButtonState();
		} else if (type == ImageMgr.delete_begin) {
			mTip.setVisibility(View.VISIBLE);
		} else if (type == ImageMgr.delete_end || type == ImageMgr.refresh_end) {
			mTip.setVisibility(View.GONE);
			handleDelete();
		}
	}

	private void handleDelete() {
		if (mArray == null || mArray.size() == 0)
			return;
		File f = new File(mArray.get(mIndex));
		if (!f.exists()) {
			mArray.remove(mIndex);
			mViewPager.getAdapter().notifyDataSetChanged();
			if (mArray.size() == 0) {
				this.finish();
			} else {

				if (mIndex >= mArray.size()) {
					mIndex = mArray.size() - 1;

				}
				mViewMap.clear();
				int oldindex = mIndex;
				mViewPager.setAdapter(new MyAdapter(this));

				mViewPager.setCurrentItem(oldindex);

				UpdateButtonState();

			}
		}
	}

	@Override
	protected void onResume() {
		// TODO Auto-generated method stub
		super.onResume();
		ImageMgr.GetInstance().addListener(this);

		ImageLoader2.GetInstance().addReleaseMemoryListener(this);
		this.UpdateCurImage();
	}

	@Override
	protected void onPause() {
		// TODO Auto-generated method stub
		super.onPause();
		ImageMgr.GetInstance().removeListener(this);

		ImageLoader2.GetInstance().removeReleaseListener(this);
		ClearAllBitmap();
	}

	@Override
	public void ReleaseBitmap() {
		// TODO Auto-generated method stub
		Log.d(TAG, "ReleaseBitmap");
		ViewSingle view = mViewMap.get(mIndex);
		if (view != null) {
			if (view.HaveImage() == false) {
				ClearAllBitmap();
			}
		}
	}

	private OnViewTapListener mViewTapListener = new OnViewTapListener() {
		public void onViewTap(View view, float x, float y) {
			// TODO Auto-generated method stub

			if (mBarTop.getVisibility() == View.VISIBLE) {

				mBarTop.setVisibility(View.INVISIBLE);
				mBarBottom.setVisibility(View.INVISIBLE);
			} else {

				mBarTop.setVisibility(View.VISIBLE);
				mBarBottom.setVisibility(View.VISIBLE);
			}

		}
	};

}
