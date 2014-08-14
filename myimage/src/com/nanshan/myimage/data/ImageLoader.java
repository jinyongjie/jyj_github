package com.nanshan.myimage.data;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Stack;
import java.util.WeakHashMap;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import android.app.ActivityManager;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.graphics.BitmapFactory;
import android.media.ExifInterface;
import android.media.ThumbnailUtils;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.os.StatFs;
import android.util.Log;
import android.widget.ImageView;

public class ImageLoader {
	public interface ImageCallback {
		public void OnImageLoad(Bitmap bitmap, String path);
	}

	public interface ReleaseBitmapListener {
		public void ReleaseBitmap();
	}

	ArrayList<ReleaseBitmapListener> mReleaseListeners = new ArrayList<ReleaseBitmapListener>();

	class ImageTask {
		public int type;//0 load 1rotae
		public int id;
		public String path;
		public int width;
		public int height;
		public Bitmap bitmap;
		public boolean thumb;
		public ImageCallback callback;
	}

	class CancelTask {
		public int id;
		public int width;
		public int height;
	}

	final Object mLock = new Object();
	List<ImageTask> mTaskList = new LinkedList<ImageTask>();
	ImageHandler mHander;
	ImageMemoryCache mMemCache = null;

	static final String TAG = "cache";

	public ImageLoader() {

	}

	public Bitmap GetImage(int id, String path, int width, int height,
			boolean thumb, ImageCallback callback) {

		synchronized (mLock) {

			Iterator<ImageTask> it = mTaskList.iterator();
			while (it.hasNext()) {
				ImageTask task = it.next();
				if (task.id == id && task.width == width
						&& task.height == height && task.callback == callback) {

					return null;
				}
			}

			ImageTask task = new ImageTask();
			task.type = 1;
			task.path = path;
			task.width = width;
			task.height = height;
			task.callback = callback;
			task.id = id;
			task.thumb = thumb;

			mTaskList.add(task);

			// if (mTaskArray.size() == 1)
			{
				// Log.i(TAG,String.format("task add id=%d size=%d",
				// id,mTaskList.size()));
				mLock.notifyAll();
			}
		}

		return null;
	}

	public void Cancel(int id, int width, int height, ImageCallback callback) {

		synchronized (mLock) {

			Iterator<ImageTask> it = mTaskList.iterator();
			while (it.hasNext()) {
				ImageTask task = it.next();
				if (task.id == id && task.width == width
						&& task.height == height && task.callback == callback) {
					// Log.i(TAG,
					// String.format("task cancel suc id=%d size=%d",id,
					// mTaskList.size()));
					it.remove();
					break;
				}
			}

		}
	}

	/*
	 * public void RemoveFromCache(int id, int width, int height) { String key =
	 * GetKey(id, width, height); mMemCache.remove(key); }
	 */
	private static ImageLoader instance = null;

	public static ImageLoader GetInstance() {
		if (instance == null) {

			instance = new ImageLoader();
		}
		return instance;
	}

	public void Init(Context context) {
		mMemCache = new ImageMemoryCache(context);
		mHander = new ImageHandler();

		for (int i = 0; i < 4; i++) {
			new ImageThread().start();
		}
	}

	class ImageHandler extends Handler {
		public void handleMessage(Message msg) {
			if (msg.what == 0) {
				ImageTask task = (ImageTask) msg.obj;
				if (task.callback != null)
					task.callback.OnImageLoad(task.bitmap, task.path);
			} else if (msg.what == 1) {

				for (int i = 0; i < mReleaseListeners.size(); i++) {
					mReleaseListeners.get(i).ReleaseBitmap();
				}

				if (msg.obj != null) {
					synchronized (msg.obj) {
						msg.obj.notifyAll();
					}

				}

			}

		}
	}

	class ImageThread extends Thread {
		public void run() {
			while (true) {
				ImageTask task = null;
				synchronized (mLock) {

					if (mTaskList.isEmpty()) {
						try {
							mLock.wait();

							if (mTaskList.isEmpty()) {
								continue;
							}

						} catch (InterruptedException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}

					}

					task = mTaskList.get(mTaskList.size() - 1);

					mTaskList.remove(mTaskList.size() - 1);

				}
				if (task != null) {
					task.bitmap = getBitmap(task.path, task.width, task.height,
							task.thumb,1);

					if (task.bitmap == null) {

						mMemCache.clear();

						Message message = new Message();
						message.what = 1;
						Object lock = new Object();
						message.obj = lock;

						synchronized (lock) {
							mHander.sendMessage(message);// 必须放这里,否则hander可能先执行
							try {
								lock.wait();
							} catch (InterruptedException e) {
								// TODO Auto-generated catch block
								e.printStackTrace();
							}
						}

						System.gc();
						int be = 1;
						while(be <= 8 && task.bitmap == null)
						{
							task.bitmap = getBitmap(task.path, task.width,
									task.height, task.thumb,be);
							be*=2;
						}
						

					}
					if (task.bitmap != null) {
						String key = GetKey(task.id, task.width, task.height);
						mMemCache.put(key, task.bitmap);
					}

					Message message = new Message();
					message.what = 0;
					message.obj = task;
					mHander.sendMessage(message);
				}

			}
		}
	}

	public static int getDigree(String imgpath) {

		if (!imgpath.toLowerCase().endsWith(".jpg"))
			return 0;
		int digree = 0;
		ExifInterface exif = null;
		try {
			exif = new ExifInterface(imgpath);
		} catch (IOException e) {
			e.printStackTrace();
			exif = null;
		}
		if (exif != null) {
			// 读取图片中相机方向信息
			int ori = exif.getAttributeInt(ExifInterface.TAG_ORIENTATION,
					ExifInterface.ORIENTATION_UNDEFINED);
			// 计算旋转角度
			switch (ori) {
			case ExifInterface.ORIENTATION_ROTATE_90:
				digree = 90;
				break;
			case ExifInterface.ORIENTATION_ROTATE_180:
				digree = 180;
				break;
			case ExifInterface.ORIENTATION_ROTATE_270:
				digree = 270;
				break;
			default:
				digree = 0;
				break;
			}
		}

		return digree;
	}

	private Bitmap getBitmap(String imagePath, int width, int height,
			boolean thumb,int be0) {

		if (width == 0 || height == 0)
			return null;

		Bitmap bitmap = null;

		BitmapFactory.Options options = new BitmapFactory.Options();
		options.inJustDecodeBounds = true;
		bitmap = BitmapFactory.decodeFile(imagePath, options);

		int h2 = options.outHeight;
		int w2 = options.outWidth;

		int digree = getDigree(imagePath);
		if (digree == 90 || digree == 270) {
			int temp = h2;
			h2 = w2;
			w2 = temp;
		}
		int be = 1;
		//if(thumb)
		{
			int beWidth = w2 / width;
			int beHeight = h2 / height;
			
			if (beWidth > beHeight) {
				be = beWidth;
			} else {
				be = beHeight;
			}

			if (be < 1) {
				be = 1;
			}
			if(be < be0)
			{
				be = be0;
			}
		}
		
		
		options.inJustDecodeBounds = false; // 设为 false
		options.inSampleSize = be;
	//	options.inPreferredConfig = Bitmap.Config.RGB_565;

		try {
			try {
				FileInputStream is = new FileInputStream(imagePath);
				bitmap = BitmapFactory.decodeFileDescriptor(is.getFD(), null,
						options);

				
				if (digree != 0 && bitmap != null) {
					// 旋转图片
					Matrix m = new Matrix();
					m.postRotate(digree);
					bitmap = Bitmap.createBitmap(bitmap, 0, 0,
							bitmap.getWidth(), bitmap.getHeight(), m, true);
				}
				if (thumb) {
					bitmap = ThumbnailUtils.extractThumbnail(bitmap, width,
							height, ThumbnailUtils.OPTIONS_RECYCLE_INPUT);
				}

			} catch (OutOfMemoryError err) {
				err.printStackTrace();
			}

			return bitmap;

		} catch (Exception err) {
			err.printStackTrace();
			return null;
		}

	}

	public class ImageMemoryCache {
		private MyLruCache<String, Bitmap> mLruCache; // 硬引用缓存

		public ImageMemoryCache(Context context) {
			int memClass = ((ActivityManager) context
					.getSystemService(Context.ACTIVITY_SERVICE))
					.getMemoryClass();
			int cacheSize = 1024 * 1024 * memClass / 2; // 硬引用缓存容量，为系统可用内存的1/4

			mLruCache = new MyLruCache<String, Bitmap>(cacheSize) {

				@Override
				protected int sizeOf(String key, Bitmap value) {

					if (value != null)
						return value.getRowBytes() * value.getHeight();
					else
						return 0;
				}

				@Override
				protected void entryRemoved(boolean evicted, String key,
						Bitmap oldValue, Bitmap newValue) {

				}
			};

		}

		/**
		 * 从缓存中获取图片
		 */
		public Bitmap get(String url) {
			Bitmap bitmap;
			// 先从硬引用缓存中获取
			synchronized (mLruCache) {
				bitmap = mLruCache.get(url);
				if (bitmap != null) {
					// 如果找到的话，把元素移到LinkedHashMap的最前面，从而保证在LRU算法中是最后被删除
					// mLruCache.remove(url);
					// mLruCache.put(url, bitmap);
					return bitmap;
				}
			}
			// 如果硬引用缓存中找不到，到软引用缓存中找

			return null;
		}

		/**
		 * 添加图片到缓存
		 */
		public void put(String url, Bitmap bitmap) {
			if (bitmap != null) {
				synchronized (mLruCache) {
					mLruCache.put(url, bitmap);
					// Log.i(TAG,String.format("memcache put %d",
					// mLruCache.size()));
				}
			}
		}

		public void clear() {
			synchronized (mLruCache) {
				mLruCache.evictAll();
			}
		}

	}

	String GetKey(int id, int width, int height) {
		return String.format("%d_%d_%d", id, width, height);
	}

	public void AddReleaseListener(ReleaseBitmapListener listener) {
		mReleaseListeners.add(listener);
	}

	public void RemoveReleaseListener(ReleaseBitmapListener listener) {
		for (int i = 0; i < mReleaseListeners.size(); i++) {
			if (mReleaseListeners.get(i) == listener) {
				mReleaseListeners.remove(i);
				return;
			}
		}
	}
}
