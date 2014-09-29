package com.example.myimage.data;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.lang.ref.WeakReference;
import java.math.BigInteger;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Stack;
import java.util.WeakHashMap;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;



import com.example.myimage.data.ImageMgr.ImageInfo;




import android.app.ActivityManager;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.graphics.Matrix;
import android.graphics.BitmapFactory;
import android.media.ExifInterface;
import android.media.ThumbnailUtils;
import android.opengl.GLES10;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.os.StatFs;
import android.util.Config;
import android.util.Log;
import android.widget.ImageView;

public class ImageLoader2 {
	public interface ImageCallback {
		public void onImageLoadSuccess(Bitmap bitmap, String path, int width,
				int height);

		public void onImageLoadFail(String path, int width, int height);

		public void onImageRotate(String path);
	}

	public interface ReleaseBitmapListener {
		public void ReleaseBitmap();
	}

	private LinkedList<WeakReference<ReleaseBitmapListener>> mReleaseListeners = new LinkedList<WeakReference<ReleaseBitmapListener>>();

	class ImageTask {
		public int type;
		public String path;
		public int width;
		public int height;
		public Bitmap bitmap;
		public int quality;
		public ImageCallback callback;

		static final int type_load = 0;
		static final int type_rotate = 1;
	}

	private final Object mLock = new Object();
	private List<ImageTask> mTaskList = new LinkedList<ImageTask>();

	private ImageHandler mHander;
	private ImageMemoryCache mMemCache = new ImageMemoryCache((int) Runtime
			.getRuntime().maxMemory() / 8);
	private FileCache mFileCache = null;
	private static final String TAG = "cache";

	public ImageLoader2() {

	}

	public final static int quality_thumb = 0;
	public final static int quality_low = 1;
	public final static int quality_normal = 2;
	public final static int quality_high = 3;

	public Bitmap requestImage(String path, int width, int height, int quality,
			ImageCallback callback) {
		if (path == null || path.length() == 0)
			return null;
		synchronized (mLock) {

			ImageTask task = null;
			boolean find = false;
			Iterator<ImageTask> it = mTaskList.iterator();
			while (it.hasNext()) {
				task = it.next();
				if (task.callback == callback) {
					find = true;
					break;
				}
			}

			if (!find) {
				task = new ImageTask();
				task.type = ImageTask.type_load;
				task.path = path;
				task.width = width;
				task.height = height;
				task.callback = callback;
				task.quality = quality;

				mTaskList.add(task);
			} else {
				task.type = ImageTask.type_load;
				task.path = path;
				task.width = width;
				task.height = height;
				task.callback = callback;
				task.quality = quality;
			}

			// if (mTaskArray.size() == 1)
			{
				// Log.i(TAG,String.format("task add id=%d size=%d",
				// id,mTaskList.size()));
				mLock.notifyAll();
			}
		}

		return null;
	}

	/*
	 * public void RemoveFromCache(int id, int width, int height) { String key =
	 * GetKey(id, width, height); mMemCache.remove(key); }
	 */
	private static ImageLoader2 instance = null;

	public static ImageLoader2 GetInstance() {
		if (instance == null) {

			instance = new ImageLoader2();
		}
		return instance;
	}

	private static final int thread_count = 4;
	private ImageThread[] mThreads = new ImageThread[thread_count];

	public void init(Context context) {
		if (mHander != null)
			return;

		mHander = new ImageHandler();
		mFileCache = new FileCache(context);

		mStopped = false;
		for (int i = 0; i < thread_count; i++) {
			mThreads[i] = new ImageThread();
			mThreads[i].start();
		}
	}

	private boolean mStopped = false;

	public void unInit() {
		if (mReleaseListeners != null)
			mReleaseListeners.clear();

		for (int i = 0; i < thread_count; i++) {
			if (mThreads[i] != null) {
				mThreads[i] = null;
			}
		}
		synchronized (mLock) {
			mStopped = true;
			mLock.notifyAll();
			mTaskList.clear();// todo
		}
		if (mMemCache != null)
			mMemCache.clear();

	}

	private class ImageHandler extends Handler {
		public void handleMessage(Message msg) {
			if (msg.what == 0) {
				ImageTask task = (ImageTask) msg.obj;

				if (task.callback != null) {
					if (task.type == ImageTask.type_load) {
						if (task.bitmap != null)
							task.callback.onImageLoadSuccess(task.bitmap,
									task.path, task.width, task.height);
						else
							task.callback.onImageLoadFail(task.path,
									task.width, task.height);
					}

					else if (task.type == ImageTask.type_rotate) {
						task.callback.onImageRotate(task.path);
						ImageMgr.GetInstance().NotifyImageRotate(task.path);
					}

				}

			} else if (msg.what == 1) {

				for (WeakReference<ReleaseBitmapListener> l : mReleaseListeners) {
					if(l != null && l.get() != null)
					l.get().ReleaseBitmap();
				}

				if (msg.obj != null) {
					synchronized (msg.obj) {
						msg.obj.notifyAll();
					}

				}

			}

		}
	}

	private class ImageThread extends Thread {
		public void run() {
			while (!mStopped) {
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
					if (task.type == ImageTask.type_load) {
						String key = getKey(task.path, task.width,
								task.height);
						task.bitmap = mMemCache.get(key);
						if(task.bitmap == null)
						{
							BitmapFactory.Options options = new BitmapFactory.Options();
							options.inJustDecodeBounds = true;
							BitmapFactory.decodeFile(task.path, options);

							if (task.quality == quality_thumb) {
								task.bitmap = getThumbBitmap(task.path, task.width,
										task.height, options.outWidth,
										options.outHeight);
							} else if (task.quality == quality_normal
									|| task.quality == quality_low) {
								task.bitmap = getLowBitmap(task.path, task.width,
										task.height, options.outWidth,
										options.outHeight);
							} else {
								task.bitmap = getHighBitmap(task.path, task.width,
										task.height, 1, options.outWidth,
										options.outHeight);
							}

							if (task.bitmap == null) {
								Log.d(TAG, String.format("decode fail", task.width,
										task.height));
								File f = new File(task.path);
								if (f != null && f.isFile() && f.exists()) {
									releaseMemory();

									if (task.quality == quality_thumb) {
										task.bitmap = getThumbBitmap(task.path,
												task.width, task.height,
												options.outWidth, options.outHeight);
									} else if (task.quality == quality_normal
											|| task.quality == quality_low) {
										task.bitmap = getLowBitmap(task.path,
												task.width, task.height,
												options.outWidth, options.outHeight);
									} else {
										int be = 1;
										while (be <= 8 && task.bitmap == null) {

											be *= 2;
											task.bitmap = getHighBitmap(task.path,
													task.width, task.height, be,
													options.outWidth,
													options.outHeight);
										}
									}
								}
							}
							if (task.bitmap != null) {
								
								mMemCache.put(key, task.bitmap);
							}
						}
						
						Message message = new Message();
						message.what = 0;
						message.obj = task;
						mHander.sendMessage(message);
					} else if (task.type == ImageTask.type_rotate) {
						if (rotateImage(task.path) == false) {
							releaseMemory();
							rotateImage(task.path);
						}
						Message message = new Message();
						message.what = 0;
						message.obj = task;
						mHander.sendMessage(message);
					}
				}
			}
		}
	}

	private void releaseMemory() {
		Message message = new Message();
		message.what = 1;
		Object lock = new Object();
		message.obj = lock;

		synchronized (lock) {
			mHander.sendMessage(message);// ���������,����hander������ִ��
			try {
				lock.wait();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		mMemCache.clear();
		System.gc();
	}

	private boolean rotateImage(String path) {
		try {
			FileInputStream is = new FileInputStream(path);
			Bitmap bitmap = BitmapFactory.decodeFileDescriptor(is.getFD(),
					null, null);

			if (bitmap != null) {
				// ��תͼƬ
				Matrix m = new Matrix();
				m.postRotate(90);
				bitmap = Bitmap.createBitmap(bitmap, 0, 0, bitmap.getWidth(),
						bitmap.getHeight(), m, true);
			}

			FileOutputStream out = new FileOutputStream(path);

			bitmap.compress(Bitmap.CompressFormat.PNG, 90, out);// todo
			out.flush();
			/*
			 * if (path.toLowerCase().endsWith(".png") == true) {
			 * bitmap.compress(Bitmap.CompressFormat.PNG, 90, out); out.flush();
			 * } else { bitmap.compress(Bitmap.CompressFormat.JPEG, 90, out);
			 * out.flush(); }
			 */
			out.close();
		} catch (OutOfMemoryError err) {
			err.printStackTrace();
			return false;
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return true;
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

			int ori = exif.getAttributeInt(ExifInterface.TAG_ORIENTATION,
					ExifInterface.ORIENTATION_UNDEFINED);

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

	private Bitmap getHighBitmap(String imagePath, int width, int height,
			int be, int raww, int rawh) {

		if (width == 0 || height == 0)
			return null;

		Bitmap bitmap = null;

		BitmapFactory.Options options = new BitmapFactory.Options();

		int h2 = rawh;
		int w2 = raww;

		int digree = getDigree(imagePath);
		if (digree == 90 || digree == 270) {
			int temp = h2;
			h2 = w2;
			w2 = temp;
		}

		options.inJustDecodeBounds = false; // ��Ϊ false
		options.inSampleSize = be;
		// options.inPreferredConfig = Bitmap.Config.RGB_565;

		try {
			try {
				FileInputStream is = new FileInputStream(imagePath);
				bitmap = BitmapFactory.decodeFileDescriptor(is.getFD(), null,
						options);

				Matrix m = null;

				if (w2 > width || h2 > height) {
					m = new Matrix();
					float rw = (float) width / w2;
					float rh = (float) height / h2;
					float r = rw < rh ? rw : rh;

					m.postScale(r, r);

				}
				if (digree != 0 && bitmap != null) {
					if (m == null)
						m = new Matrix();
					m.postRotate(digree);
				}
				if (m != null) {
					bitmap = Bitmap.createBitmap(bitmap, 0, 0,
							bitmap.getWidth(), bitmap.getHeight(), m, true);
				}

			} catch (OutOfMemoryError err) {
				err.printStackTrace();
				return null;
			}

			return bitmap;

		} catch (Exception err) {
			err.printStackTrace();
			return null;
		}

	}

	private Bitmap getLowBitmap(String imagePath, int width, int height,
			int raww, int rawh) {

		if (width == 0 || height == 0)
			return null;

		Bitmap bitmap = null;

		BitmapFactory.Options options = new BitmapFactory.Options();

		int h2 = rawh;
		int w2 = raww;

		int digree = getDigree(imagePath);
		if (digree == 90 || digree == 270) {
			int temp = h2;
			h2 = w2;
			w2 = temp;
		}
		int be = 1;

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

		options.inJustDecodeBounds = false; // ��Ϊ false
		options.inSampleSize = be;
		// options.inPreferredConfig = Bitmap.Config.RGB_565;

		try {
			try {
				FileInputStream is = new FileInputStream(imagePath);
				bitmap = BitmapFactory.decodeFileDescriptor(is.getFD(), null,
						options);

				if (digree != 0 && bitmap != null) {
					Matrix m = new Matrix();
					m.postRotate(digree);

					bitmap = Bitmap.createBitmap(bitmap, 0, 0,
							bitmap.getWidth(), bitmap.getHeight(), m, true);
				}

			} catch (OutOfMemoryError err) {
				err.printStackTrace();
				return null;
			}

			return bitmap;

		} catch (Exception err) {
			err.printStackTrace();
			return null;
		}

	}

	private byte[] getMD5(byte[] data) {
		byte[] hash = null;
		try {
			MessageDigest digest = MessageDigest.getInstance(HASH_ALGORITHM);
			digest.update(data);
			hash = digest.digest();
		} catch (NoSuchAlgorithmException e) {
		
		}
		return hash;
	}

	private static final String HASH_ALGORITHM = "MD5";
	private static final int RADIX = 10 + 26; // 10 digits + 26 letters

	private String getFileKey(String imageUri) {
		File f = new File(imageUri);
		String path  = String.format("%s_%d", imageUri,f.lastModified());
		byte[] md5 = getMD5(path.getBytes());
		BigInteger bi = new BigInteger(md5).abs();
		return bi.toString(RADIX);
	}

	private Bitmap getThumbBitmap(String imagePath, int width, int height,
			int raww, int rawh) {

		if (width == 0 || height == 0)
			return null;

		Bitmap bitmap = null;

		try {
			String key = getFileKey(imagePath);
			File fCache = mFileCache.getFile(key);

			if (fCache != null) {
				BitmapFactory.Options options = new BitmapFactory.Options();
				options.inJustDecodeBounds = false; // ��Ϊ false
				options.inSampleSize = 1;
				FileInputStream fi = 	new FileInputStream(fCache.getAbsoluteFile());
				
				bitmap = BitmapFactory.decodeFileDescriptor(fi.getFD(),null,options);
			
				if(bitmap != null)
					return bitmap;
			}

			BitmapFactory.Options options = new BitmapFactory.Options();

			int h2 = rawh;
			int w2 = raww;

			int digree = getDigree(imagePath);
			if (digree == 90 || digree == 270) {
				int temp = h2;
				h2 = w2;
				w2 = temp;
			}
			int be = 1;

			int beWidth = w2 / width;
			int beHeight = h2 / height;

			if (beWidth > beHeight) {
				be = beHeight;
			} else {
				be = beHeight;
			}

			if (be < 1) {
				be = 1;
			}

			options.inJustDecodeBounds = false; // ��Ϊ false
			options.inSampleSize = be;
			// options.inPreferredConfig = Bitmap.Config.RGB_565;
			FileInputStream is = new FileInputStream(imagePath);
			bitmap = BitmapFactory.decodeFileDescriptor(is.getFD(), null,
					options);

			if (digree != 0 && bitmap != null) {
				Matrix m = new Matrix();
				m.postRotate(digree);

				bitmap = Bitmap.createBitmap(bitmap, 0, 0, bitmap.getWidth(),
						bitmap.getHeight(), m, true);
			}
			Log.d(TAG,String.format("thumb w=%d h=%d rw=%d rh=%d be=%d", width,height,raww,rawh,be));
			bitmap = ThumbnailUtils.extractThumbnail(bitmap, width, height,
					ThumbnailUtils.OPTIONS_RECYCLE_INPUT);

			if(bitmap != null)
			{
				mFileCache.put(key, bitmap);
			}
			return bitmap;

		} catch (OutOfMemoryError err) {
			err.printStackTrace();
			return null;
		} catch (Exception err) {
			err.printStackTrace();
			return null;
		}

	}

	public class ImageMemoryCache {
		private MyLruCache<String, Bitmap> mLruCache; // Ӳ���û���

		public ImageMemoryCache(int size) {
			Log.d(TAG, String.format("ImageMemoryCache size =%d", size));
			mLruCache = new MyLruCache<String, Bitmap>(size) {

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
		 * �ӻ����л�ȡͼƬ
		 */
		public Bitmap get(String url) {
			Bitmap bitmap;
			// �ȴ�Ӳ���û����л�ȡ
			synchronized (mLruCache) {
				bitmap = mLruCache.get(url);
				if (bitmap != null) {
					// ����ҵ��Ļ�����Ԫ���Ƶ�LinkedHashMap����ǰ�棬�Ӷ�֤��LRU�㷨�������ɾ��
					// mLruCache.remove(url);
					// mLruCache.put(url, bitmap);
					return bitmap;
				}
			}
			// ���Ӳ���û������Ҳ������������û�������

			return null;
		}

		/**
		 * ���ͼƬ������
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

	public void clearCache() {
		mMemCache.clear();
	}

	private String getKey(String path, int width, int height) {
		return String.format("%s_%d_%d", path, width, height);
	}

	public void addReleaseMemoryListener(ReleaseBitmapListener listener) {
		Iterator it = mReleaseListeners.iterator();
		while(it.hasNext())
		{
			WeakReference<ReleaseBitmapListener> w = (WeakReference<ReleaseBitmapListener>) it.next();
			if(w.get() == null)
			{
				it.remove();
				continue;
			}
			if(w.get() == listener)
			{
				return;
			}
		}
		
		mReleaseListeners.add(new WeakReference<ReleaseBitmapListener>(listener));
	}

	public void removeReleaseListener(ReleaseBitmapListener listener) {
		Iterator it = mReleaseListeners.iterator();
		while(it.hasNext())
		{
			WeakReference<ReleaseBitmapListener> w = (WeakReference<ReleaseBitmapListener>) it.next();
			if(w.get() == null)
			{
				it.remove();
				continue;
			}
			if(w.get() == listener)
			{
				it.remove();
				break;
			}
		}

	}

	public void rotateImage(String path, ImageCallback callback) {

		synchronized (mLock) {
			ImageTask task = new ImageTask();
			task.type = ImageTask.type_rotate;
			task.path = path;
			task.width = 0;
			task.height = 1;
			task.callback = callback;
			task.quality = quality_normal;

			mTaskList.add(task);

			// if (mTaskArray.size() == 1)
			{
				// Log.i(TAG,String.format("task add id=%d size=%d",
				// id,mTaskList.size()));
				mLock.notifyAll();
			}
		}

	}

	static final String mCacheDir = android.os.Environment.getExternalStorageDirectory() + "/myimage/cache";
	public static String getCacheDir()
	{
		return mCacheDir;
	}
	public class FileCache {

		private static final String TAG = "MemoryCache";

		private File cacheDir; // the directory to save images

		/**
		 * Constructor
		 * 
		 * @param context
		 *            The context related to this cache.
		 * */
		public FileCache(Context context) {
			// Find the directory to save cached images
			if (android.os.Environment.getExternalStorageState().equals(
					android.os.Environment.MEDIA_MOUNTED))
				cacheDir = new File(
						mCacheDir);
			else
				cacheDir = context.getCacheDir();
			if (!cacheDir.exists())
				cacheDir.mkdirs();

			Log.d(TAG, "cache dir: " + cacheDir.getAbsolutePath());
		}

		/**
		 * Search the specific image file with a unique key.
		 * 
		 * @param key
		 *            Should be unique.
		 * @return Returns the image file corresponding to the key.
		 * */
		public File getFile(String key) {
			File f = new File(cacheDir, key);
			if (f.exists()) {
			//	Log.d(TAG, "the file you wanted exists " + f.getAbsolutePath());
				return f;
			} else {
			//	Log.d(TAG,"the file you wanted does not exists: "+ f.getAbsolutePath());
			}

			return null;
		}

		/**
		 * Put a bitmap into cache with a unique key.
		 * 
		 * @param key
		 *            Should be unique.
		 * @param value
		 *            A bitmap.
		 * */
		public void put(String key, Bitmap value) {
			File f = new File(cacheDir, key);
			if (!f.exists())
				try {
					f.createNewFile();
				} catch (IOException e) {
					e.printStackTrace();
				}
			// Use the util's function to save the bitmap.
			if (saveBitmap(f, value))
				Log.d(TAG, "Save file to sdcard successfully!");
			else
				Log.w(TAG, "Save file to sdcard failed!");

		}

		/**
		 * Clear the cache directory on sdcard.
		 * */
		public void clear() {
			File[] files = cacheDir.listFiles();
			for (File f : files)
				f.delete();
		}

		public boolean saveBitmap(File file, Bitmap bitmap) {
			if (file == null || bitmap == null)
				return false;
			try {
				BufferedOutputStream out = new BufferedOutputStream(
						new FileOutputStream(file));
				return bitmap.compress(CompressFormat.JPEG, 100, out);
			} catch (FileNotFoundException e) {
				e.printStackTrace();
				return false;
			}
		}
	}
}
