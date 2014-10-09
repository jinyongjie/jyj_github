package com.imagelib.data;

import java.io.File;
import java.io.FileFilter;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.ref.WeakReference;
import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Collections;
import java.util.Comparator;
import java.util.Date;
import java.util.GregorianCalendar;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map.Entry;
import java.util.WeakHashMap;



import com.imagelib.R;
import com.imagelib.ctrl.AdapterTime;

import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Context;
import android.database.ContentObserver;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.media.ExifInterface;
import android.media.ThumbnailUtils;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Handler;
import android.os.Message;
import android.provider.MediaStore;
import android.provider.MediaStore.Images.Media;
import android.util.DisplayMetrics;
import android.util.Log;
import android.util.Pair;
import android.view.View;

public class ImageMgr {
	private static final String TAG = "ImageMgr";
	// private HashMap<String, ImageInfo> mImages = new HashMap<String,
	// ImageInfo>();

	private ArrayList<ImageInfo> mArray = new ArrayList<ImageInfo>();
	private HashSet<String> mArrayLike = new HashSet<String>();
	private ArrayList<LineInfo> mArrayLineInfo = new ArrayList<LineInfo>();
	private static ImageMgr instance = null;
	private Context mContext;
	private SQLiteDatabase mDb;
	private final static boolean mEnableTag = false;
	private Object mLock = new Object();

	public static final int refresh_begin = 0;
	public static final int refresh_end = 1;
	public static final int delete_begin = 3;
	public static final int delete_end = 4;
	public static final int like = 5;
	public static final int tag = 6;
	public static final int rotate = 7;

	public interface ImageMgrListener {
		public void onImageMgrNotify(int type, Object path);
	}

	private HashSet<WeakReference<ImageMgrListener>> mListeners = new HashSet<WeakReference<ImageMgrListener>>();

	public class ImageInfo {
		public String path;
		public Date date;
		// public String tag;
		// public boolean like;
		// public String name;
		public String dirName;
		public String dirPath;
	}

	public class ImageData {
		public String path;
		public boolean sel;
	}

	public class LineInfo {
		public boolean isTitle;// 0=tile,1=image
		public ArrayList<ImageData> imagearray;
		public Date date;
		public int childcount;// children num of group

		public int pos;
	}

	public class DirInfo {
		public ArrayList<ImageInfo> array = new ArrayList<ImageInfo>();
		public String dir;
		public String name;
	}

	public static ImageMgr GetInstance() {
		if (instance == null) {
			instance = new ImageMgr();
		}
		return instance;
	}
	private boolean mFirstRefresh = true;
	private void loadData() {
		mDb = SQLiteDatabase.openOrCreateDatabase(mContext.getFilesDir()
				.toString() + "/data.db3", null);

		mDb.execSQL("create table if not exists table_like(_id integer primary key autoincrement,"
				+ " path nvarchar(255))");

		Cursor cursorLike = mDb.rawQuery("select * from table_like", null);

		while (cursorLike.moveToNext()) {
			String path = cursorLike.getString(1);
			mArrayLike.add(path);
		}

		Uri uri = MediaStore.Images.Media.EXTERNAL_CONTENT_URI;
		// 获取ContentResolver
		ContentResolver contentResolver = mContext.getContentResolver();
		// 查询的字段
		String[] projection = { MediaStore.Images.Media.DATA,
				MediaStore.Images.Media.DATE_MODIFIED };
		// 条件
		String selection = MediaStore.Images.Media.MIME_TYPE + "=?";
		// 条件值(這裡的参数不是图片的格式，而是标准，所有不要改动)
		String[] selectionArgs = { "image/jpeg" };
		// 排序
		String sortOrder = MediaStore.Images.Media.DATE_MODIFIED + " desc";
		// 查询sd卡上的图片
		Cursor cursor = contentResolver.query(uri, null, null, null, sortOrder);

		ArrayList<ImageInfo> array = new ArrayList<ImageInfo>(cursor.getCount());
		ArrayList<LineInfo> array2 =  new ArrayList<LineInfo>(cursor.getCount()/AdapterTime.mColum);
		


			int index0 = cursor.getColumnIndex(Media.DATA);
			int index1 = cursor.getColumnIndex(Media.DATE_MODIFIED);
			
			LineInfo title = null;
			LineInfo line = null;

			GregorianCalendar c = null;
			
			int i= 0;
			while (cursor.moveToNext()) {
				ImageInfo info = new ImageInfo();

				byte[] data = cursor.getBlob(index0);

				info.path = new String(data, 0, data.length - 1);

				 File file = new File(info.path);
				 if (file.exists() )
				{
					long t = cursor.getLong(index1);

					info.date = new Date(t * 1000);

					// File dir = new File(info.path).getParentFile();
					int end = info.path.lastIndexOf(File.separator);
					int begin = info.path.lastIndexOf(File.separator, end - 1);
					info.dirName = info.path.substring(begin + 1, end);
					info.dirPath = info.path.substring(0, begin);

					array.add(info);
					
					if (title == null) {
						title = new LineInfo();
						title.isTitle = true;
						title.childcount = 0;
						title.date = info.date;
						title.pos = array2.size();
						array2.add(title);
						c = new GregorianCalendar();
						c.setTime(info.date);
						// .get(Calendar.DATE);

						line = new LineInfo();
						line.isTitle = false;
						line.date = info.date;
						line.imagearray = new ArrayList<ImageData>();
						ImageData idata = new ImageData();
						idata.sel = false;
						idata.path = info.path;
						line.imagearray.add(idata);
						line.pos = array2.size();
						array2.add(line);
						title.childcount++;
					} else {
						GregorianCalendar c2 = new GregorianCalendar();
						c2.setTime(info.date);
						if (c.get(Calendar.YEAR) == c2.get(Calendar.YEAR) &&
						
								c.get(Calendar.DAY_OF_YEAR) == c2
										.get(Calendar.DAY_OF_YEAR))

						{
							if (line.imagearray.size() == AdapterTime.mColum) {
								line = new LineInfo();
								line.isTitle = false;
								line.date = info.date;
								line.imagearray = new ArrayList<ImageData>();
								line.pos = array2.size();
								array2.add(line);

								ImageData idata = new ImageData();
								idata.sel = false;
								idata.path = info.path;
								line.imagearray.add(idata);

								title.childcount++;
							} else {
								ImageData idata = new ImageData();
								idata.sel = false;
								idata.path = info.path;
								line.imagearray.add(idata);

								title.childcount++;
							}
						} else {
							title = null;
							line = null;
							i--;
							continue;
						}
					}
					i++;
					if(mFirstRefresh)
					{
						if(i == 100 || i == 1000)
						{						
							Pair<Object, Object> p = new Pair<Object, Object>(array.clone(),
									array2.clone());
							notifyListeners(ImageMgr.refresh_end, p);
							
						}
						
					}
					
				}
			}
		
			mFirstRefresh = false;
		Pair<Object, Object> p = new Pair<Object, Object>(array,
				array2);
		notifyListeners(ImageMgr.refresh_end, p);
	}

	public static final int init_no = 0;
	public static final int init_doing = 1;
	public static final int init_complete = 2;
	private int mInitState = init_no;

	public int getInitState() {
		synchronized (mLock) {
			return mInitState;
		}
	}

	boolean mEnableRefresh = true;

	public void setEnableRefresh(boolean enable) {
		mEnableRefresh = enable;
	}

	private class MyHandler extends Handler {
		public void handleMessage(Message msg) {
			if (msg.what == ImageMgr.refresh_end) {
				Pair<Object, Object> p = (Pair<Object, Object>) msg.obj;
				mArray = (ArrayList<ImageInfo>) p.first;
				mArrayLineInfo = (ArrayList<LineInfo>) p.second;
				notifyListenersInternal(msg.what, null);
			} else {
				notifyListenersInternal(msg.what, msg.obj);
			}

		}
	}

	private void notifyListenersInternal(int type, Object obj) {
		Iterator it = mListeners.iterator();
		while (it.hasNext()) {
			WeakReference<ImageMgrListener> w = (WeakReference<ImageMgrListener>) it
					.next();
			if (w == null || w.get() == null) {
				it.remove();
				continue;
			}
			w.get().onImageMgrNotify(type, obj);
		}
	}

	private MyHandler mHandler;
	boolean mInited = false;

	public void init(final Context context) {
		if (mInited)
			return;
		mInited = true;
		mContext = context;
		mThread.start();
		mHandler = new MyHandler();
		refresh();

		context.getContentResolver().registerContentObserver(
				MediaStore.Images.Media.EXTERNAL_CONTENT_URI, true,
				new ContentObserver(new Handler()) {
					public void onChange(boolean selfChange) {
						refresh();
						Log.d(TAG, String.format(
								"ImageMgr ContentObserver onChange state=%d",
								getInitState()));

					}
				});
	}

	static final int task_refresh = 0;
	static final int task_delete = 1;

	class MyTask {
		int type;
		Object param;
	}

	LinkedList<MyTask> mTaskArray = new LinkedList<MyTask>();

	public void refresh() {
		if (mEnableRefresh == false)
			return;

		synchronized (mTaskArray) {
			for (MyTask task : mTaskArray) {
				if (task.type == task_refresh)
					return;
			}
		}
		addTask(task_refresh, null);

	}

	public ArrayList<LineInfo> parse(ArrayList<ImageInfo> array, int colum) {

		ArrayList<LineInfo> res = new ArrayList<LineInfo>();

		LineInfo title = null;
		LineInfo line = null;

		GregorianCalendar c = null;

		for (int i = 0; i < array.size(); i++) {
			ImageInfo info = array.get(i);
			if (title == null) {
				title = new LineInfo();
				title.isTitle = true;
				title.childcount = 0;
				title.date = info.date;
				title.pos = res.size();
				res.add(title);
				c = new GregorianCalendar();
				c.setTime(info.date);
				// .get(Calendar.DATE);

				line = new LineInfo();
				line.isTitle = false;
				line.date = info.date;
				line.imagearray = new ArrayList<ImageData>();
				ImageData data = new ImageData();
				data.sel = false;
				data.path = info.path;
				line.imagearray.add(data);
				line.pos = res.size();
				res.add(line);
				title.childcount++;
			} else {
				GregorianCalendar c2 = new GregorianCalendar();
				c2.setTime(info.date);
				if (c.get(Calendar.YEAR) == c2.get(Calendar.YEAR) &&
				// c.get(Calendar.MONTH) == c2.get(Calendar.MONTH)&&
						c.get(Calendar.DAY_OF_YEAR) == c2
								.get(Calendar.DAY_OF_YEAR))

				{
					if (line.imagearray.size() == colum) {
						line = new LineInfo();
						line.isTitle = false;
						line.date = info.date;
						line.imagearray = new ArrayList<ImageData>();
						line.pos = res.size();
						res.add(line);

						ImageData data = new ImageData();
						data.sel = false;
						data.path = info.path;
						line.imagearray.add(data);

						title.childcount++;
					} else {
						ImageData data = new ImageData();
						data.sel = false;
						data.path = info.path;
						line.imagearray.add(data);

						title.childcount++;
					}
				} else {
					title = null;
					line = null;
					i--;
					continue;
				}
			}

		}

		return res;
	}

	private void addTask(int type, Object obj) {
		synchronized (mTaskArray) {
			MyTask task = new MyTask();
			task.type = type;
			task.param = obj;
			mTaskArray.add(task);
			mTaskArray.notifyAll();
		}
	}

	Thread mThread = new Thread() {
		public void run() {
			while (true) {
				MyTask task = null;
				synchronized (mTaskArray) {
					if (mTaskArray.isEmpty()) {
						try {
							mTaskArray.wait();
						} catch (InterruptedException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
					}

					if (!mTaskArray.isEmpty()) {
						task = mTaskArray.getFirst();
						mTaskArray.removeFirst();
					}
				}
				if (task == null)
					continue;
				if (task.type == task_refresh) {
					notifyListeners(ImageMgr.refresh_begin, null);

					ArrayList<ImageInfo> array;
					ArrayList<LineInfo> arrayline;
					while (true) {

						synchronized (mLock) {
							mInitState = init_doing;
						}
						loadData();
					

						synchronized (mLock) {

							mInitState = init_complete;
						}
						boolean find = false;
						synchronized (mTaskArray) {
							for (int i = 0; i < mTaskArray.size(); i++) {
								task = mTaskArray.get(i);
								if (task.type == task_refresh) {
									mTaskArray.remove(i);
									find = true;
									break;
								}
							}
						}
						if (!find)
							break;
					}

					
				} else if (task.type == task_delete) {
					notifyListeners(ImageMgr.delete_begin, null);
					ArrayList<String> set = (ArrayList<String>) task.param;

					for (String path : set) {

						if (path == null)
							continue;

						File f = new File(path);
						if (f.exists() && f.isFile()) {
							boolean ret = f.delete();
							Log.d(TAG, String.format("deletefile %s ret=%d",
									path, ret ? 1 : 0));
							if (ret) {
								Helper.removeImageFromMediaStore(mContext, path);
							}
						}

					}

					notifyListeners(ImageMgr.delete_end, null);
				}
			}
		}
	};

	public void unInit() {
		if (mListeners != null)
			mListeners.clear();
		synchronized (mLock) {
			mArray.clear();
		}

		if (mDb != null) {
			mDb.close();
		}
	}

	public void deleteImage(String path) {// todo delete thumb
		ArrayList<String> set = new ArrayList<String>();
		set.add(path);
		deleteImage(set);
	}

	public void deleteImage(ArrayList<String> set) {
		addTask(task_delete, set);
	}

	public void addListener(ImageMgrListener listener) {

		mListeners.add(new WeakReference<ImageMgrListener>(listener));
		Log.d(TAG, String.format("addListener %d", mListeners.size()));
	}

	public void removeListener(ImageMgrListener listener) {
		for (WeakReference<ImageMgrListener> w : mListeners) {
			if (w != null && w.get() == listener) {
				mListeners.remove(w);
				break;
			}
		}
	}

	private void notifyListeners(int type, Object obj) {

		if (mHandler != null) {
			Message msg = new Message();
			msg.what = type;
			msg.obj = obj;
			mHandler.sendMessage(msg);
		}
	}

	public ImageInfo getImage(String path) {

		for (ImageInfo info : mArray) {
			if (info.path.equals(path))
				return info;
		}

		return null;
	}

	public class ImageFileFilter implements FileFilter {

		@Override
		public boolean accept(File pathname) {
			String filename = pathname.getName().toLowerCase();
			filename.toLowerCase();
			if ((pathname.isDirectory() && !pathname.isHidden())
					&& !filename.startsWith(".")
					&& !filename.contains("tencent")
					|| filename.endsWith(".jpg") || filename.endsWith(".png")
					|| filename.endsWith(".jpeg")) {
				return true;
			} else {
				return false;
			}
		}
	}

	public class FileComparator implements Comparator<ImageInfo> {
		public int compare(ImageInfo file1, ImageInfo file2) {
			return file2.date.compareTo(file1.date);
		}
	}

	public int getImageCount() {

		synchronized (mLock) {
			return mArray.size();
		}

	}

	public ArrayList<ImageInfo> GetLikeArray() {
		ArrayList<ImageInfo> array = new ArrayList<ImageInfo>();

		synchronized (mLock) {
			for (ImageInfo info : mArray) {

				if (mArrayLike.contains(info.path)) {
					array.add(info);
				}
			}
		}

		return array;
	}

	public void setLike(String path, boolean like) {
		ArrayList<String> set = new ArrayList<String>();
		set.add(path);
		setLike(set, like);
	}

	public boolean isLike(String path) {
		synchronized (mLock) {
			return mArrayLike.contains(path);
		}
	}

	public void setLike(ArrayList<String> set, boolean like) {
		for (String path : set) {

			synchronized (mLock) {
				if (like) {

					mArrayLike.add(path);
					if (mDb != null) {
						mDb.execSQL("insert into table_like values(null , ? )",
								new String[] { path });

					}

				} else {
					mArrayLike.remove(path);
					String sql = String.format(
							"delete from table_like where path='%s'", path);
					mDb.execSQL(sql);

				}
			}

		}
		this.notifyListeners(ImageMgr.like, null);
	}

	public void NotifyImageRotate(String path) {
		ImageInfo info = this.getImage(path);
		if (info != null)
			this.notifyListeners(ImageMgr.rotate, path);
	}

	public ArrayList<ImageInfo> getImageArray() {

		return mArray;
	}

	public ArrayList<LineInfo> getLineInfoArray() {
		return mArrayLineInfo;
	}

}
