package com.example.myimage.data;

import java.io.File;
import java.io.FileFilter;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.ref.WeakReference;
import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.Date;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map.Entry;
import java.util.WeakHashMap;

import com.example.myimage.app.ActivityMain;
import com.example.myimage.R;

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
import android.view.View;

public class ImageMgr {
	private static final String TAG = "ImageMgr";
	private HashMap<String, ImageInfo> mImages = new HashMap<String, ImageInfo>();

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
		public String tag;
		public boolean like;
		public String name;
		public String dirName;
		public String dirPath;
	}

	public class TimeInfo {
		public ArrayList<ImageInfo> array = new ArrayList<ImageInfo>();
		public Date date;

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

	private HashMap<String, ImageInfo> loadData(Context context) {

		HashMap<String, ImageInfo> map = new HashMap<String, ImageInfo>();

		mDb = SQLiteDatabase.openOrCreateDatabase(context.getFilesDir()
				.toString() + "/data.db3", null);

		mDb.execSQL("create table if not exists table_like(_id integer primary key autoincrement,"
				+ " path nvarchar(255))");

		Cursor cursorLike = mDb.rawQuery("select * from table_like", null);

		HashSet<String> setLike = new HashSet<String>();
		while (cursorLike.moveToNext()) {
			String path = cursorLike.getString(1);
			setLike.add(path);
		}
		HashMap<String, String> setTag = null;
		if (mEnableTag) {
			mDb.execSQL("create table if not exists table_tag(_id integer primary key autoincrement,"
					+ " path nvarchar(255)," + " tag nvarchar(64))");

			Cursor cursorTag = mDb.rawQuery("select * from table_tag", null);

			setTag = new HashMap<String, String>();
			while (cursorTag.moveToNext()) {
				String path = cursorTag.getString(1);
				String tag = cursorTag.getString(2);
				setTag.put(path, tag);
			}
		}
		Uri uri = MediaStore.Images.Media.EXTERNAL_CONTENT_URI;
		Cursor cursor = context.getContentResolver().query(uri, null, null,
				null, null);
		if (cursor != null) {
			while (cursor.moveToNext()) {
				ImageInfo info = new ImageInfo();

				byte[] data = cursor.getBlob(cursor.getColumnIndex(Media.DATA));

				info.path = new String(data, 0, data.length - 1);

				File file = new File(info.path);
				if (file.exists() && file.length() > 0) {
					info.date = new Date(file.lastModified());
					info.name = file.getName();

					File dir = new File(info.path).getParentFile();
					info.dirName = dir.getName();
					info.dirPath = dir.getAbsolutePath();

					if (setLike.contains(info.path)) {
						info.like = true;
					}
					if (mEnableTag) {
						String tag = setTag.get(info.path);
						if (tag != null) {
							info.tag = tag;
						}
					}
					map.put(info.path, info);
				}
			}
		}

		return map;

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
			notifyListenersInternal(msg.what, msg.obj);
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
		if(mInited)
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
					while (true) {
						refreshInternal();
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

					notifyListeners(ImageMgr.refresh_end, null);
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

	private void refreshInternal() {
		Log.d(TAG, "refreshInternal");
		synchronized (mLock) {
			mInitState = init_doing;
		}
		HashMap<String, ImageInfo> map = loadData(mContext);

		synchronized (mLock) {
			mImages.clear();
			mImages = map;
			mInitState = init_complete;
		}
	}

	public void unInit() {
		if (mListeners != null)
			mListeners.clear();
		synchronized (mLock) {
			mImages.clear();
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

	private void notifyListeners(int type, String path) {

		if (mHandler != null) {
			Message msg = new Message();
			msg.what = type;
			msg.obj = path;
			mHandler.sendMessage(msg);
		}
	}

	public ImageInfo getImage(String path) {
		ImageInfo info = null;
		synchronized (mLock) {
			info = mImages.get(path);
		}
		return info;
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
		int n = 0;
		synchronized (mLock) {
			n = mImages.size();
		}
		return n;
	}

	public ArrayList<ImageInfo> GetLikeArray() {
		ArrayList<ImageInfo> array = new ArrayList<ImageInfo>();

		synchronized (mLock) {
			for (Entry<String, ImageInfo> entry : mImages.entrySet()) {
				ImageInfo info = entry.getValue();
				if (info.like) {
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

	public void setLike(ArrayList<String> set, boolean like) {
		for (String path : set) {
			ImageInfo info = this.getImage(path);
			if (info != null) {
				info.like = like;
			}

			if (like) {

				if (mDb != null) {
					mDb.execSQL("insert into table_like values(null , ? )",
							new String[] { path });

				}

			} else {

				String sql = String.format(
						"delete from table_like where path='%s'", path);
				mDb.execSQL(sql);

			}

		}
		this.notifyListeners(ImageMgr.like, null);
	}

	public void NotifyImageRotate(String path) {
		ImageInfo info = this.getImage(path);
		if (info != null)
			this.notifyListeners(ImageMgr.rotate, path);
	}

	public void setTag(String path, String tag) {
		ImageInfo info = this.getImage(path);
		if (info != null) {
			String sql = String.format(
					"select * from table_tag where path='%s'", info.path);
			Cursor cursorTag = mDb.rawQuery(sql, null);
			boolean exist = cursorTag != null && cursorTag.getCount() > 0;
			info.tag = tag;

			if (tag != null && tag.length() > 0) {
				if (exist) {
					mDb.execSQL("update table_tag set tag=? where path=?",
							new String[] { info.tag, info.path });
				} else {
					mDb.execSQL("insert into table_tag values(null , ?,? )",
							new String[] { info.path, info.tag });
				}
			} else {
				if (exist) {
					sql = String.format(
							"delete from table_tag where path='%s'", info.path);
					mDb.execSQL(sql);
				}
			}
			this.notifyListeners(ImageMgr.tag, path);
		}
	}

	public ArrayList<ImageInfo> getImageArray() {

		ArrayList<ImageInfo> array = new ArrayList<ImageInfo>();
		synchronized (mLock) {
			if (mImages == null)
				return null;
			for (Entry<String, ImageInfo> entry : mImages.entrySet()) {
				array.add(entry.getValue());
			}
		}

		return array;
	}

}
