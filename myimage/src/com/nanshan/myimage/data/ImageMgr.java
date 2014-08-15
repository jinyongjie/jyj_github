package com.nanshan.myimage.data;

import java.io.File;
import java.io.FileFilter;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.Date;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map.Entry;
import java.util.WeakHashMap;

import com.nanshan.myimage.R;

import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.media.ExifInterface;
import android.media.ThumbnailUtils;
import android.os.Handler;
import android.os.Message;
import android.provider.MediaStore.Images.Media;
import android.util.DisplayMetrics;
import android.util.Log;

public class ImageMgr {
	
	private ArrayList<ImageInfo> mArrayImage = new ArrayList<ImageInfo>();
	private ArrayList<TimeInfo> mArrayTime = new ArrayList<TimeInfo>();
	private LinkedHashMap<String, DirInfo> mArrayDir = new LinkedHashMap<String, DirInfo>();
	private static ImageMgr instance = null;
	private ArrayList<ImageInfo> mArrayLike = new ArrayList<ImageInfo>();
	SQLiteDatabase mDb;

	public enum change_type
	{
		add,
		delete,
		add_like,
		delete_like,
		set_tag,
		rotate,
	}
	public interface ImageMgrListener {
		public void OnDataChange(change_type type,int id);
	}

	private ArrayList<ImageMgrListener> mListeners = new ArrayList<ImageMgrListener>();

	private ImageLoader mImageLoader = new ImageLoader();

	public class ImageInfo {
		public int id;
		public String path;
		public Date date;
		public boolean removed;// 删除只标记，提高查询效率
		public String tag;
		public boolean like;
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

	public void Init(Context context) {
		
		mArrayImage.clear();
		mArrayTime.clear();;
		mArrayDir.clear();
		mArrayLike.clear();
		
		mDb = SQLiteDatabase.openOrCreateDatabase(context.getFilesDir()
				.toString() + "/data.db3" , null);

		mDb.execSQL("create table if not exists table_like(_id integer primary key autoincrement,"
				+ " path nvarchar(255))");

		Cursor cursorDb = mDb.rawQuery("select * from table_like", null);
	
		HashSet<String> setLike = new HashSet<String>();
		while(cursorDb.moveToNext())
		{
			String path = cursorDb.getString(1);
			setLike.add(path);
		}
		// File root = new File("/sdcard");
		// Search(root,1);

		DirInfo dirinfo = new DirInfo();
		mArrayDir.put("Camera", dirinfo);
		dirinfo.dir = "/sdcard/DCIM/Camera";
		dirinfo.name = "Camera";

		Cursor cursor = context.getContentResolver().query(
				Media.EXTERNAL_CONTENT_URI, null, null, null, null);
		while (cursor.moveToNext()) {
			ImageInfo info = new ImageInfo();

			byte[] data = cursor.getBlob(cursor.getColumnIndex(Media.DATA));

			info.path = new String(data, 0, data.length - 1);
			// 将图片名添加到names集合中
			File file = new File(info.path);
			if (file.exists()) {
				info.date = new Date(file.lastModified());

				mArrayImage.add(info);

			}
		}
		Collections.sort(mArrayImage, new FileComparator());

		TimeInfo group = null;
		for (int i = 0; i < mArrayImage.size(); i++) {
			ImageInfo image = mArrayImage.get(i);
			mArrayImage.get(i).id = i;
			if (group == null) {
				group = new TimeInfo();
				group.date = image.date;
				mArrayTime.add(group);
			} else {
				Date last = group.date;
				Date cur = image.date;
				if (last.getYear() != cur.getYear()
						|| last.getMonth() != cur.getMonth()
						|| last.getDay() != cur.getDay()) {
					group = new TimeInfo();
					group.date = cur;
					mArrayTime.add(group);
				}

			}
			group.array.add(image);
			// dir
			File dir = new File(mArrayImage.get(i).path).getParentFile();

			dirinfo = mArrayDir.get(dir.getName());
			if (dirinfo == null) {
				dirinfo = new DirInfo();
				mArrayDir.put(dir.getName(), dirinfo);
				dirinfo.dir = dir.getAbsolutePath();
				dirinfo.name = dir.getName();
			}
			dirinfo.array.add(image);

			if(setLike.contains(image.path))
			{
				mArrayLike.add(image);
				image.like = true;
			}
			
		}

	}

	public int AddImage(String path) {
		if(path == null)
			return -1;
		if(path.contains("file://"))
		{
			path = path.substring(8);	
		}
		File file = new File(path);
		if (file == null || !file.exists()) {
			return -1;
		}
		for(int i = 0;i<mArrayImage.size();i++)
		{
			if(mArrayImage.get(i).path == file.getAbsolutePath())
			{
				return mArrayImage.get(i).id;
			}
		}
		ImageInfo info = new ImageInfo();
		info.id = mArrayImage.size();
		info.date = new Date(file.lastModified());
		info.path = path;
		mArrayImage.add(info);

		TimeInfo timeinfo = null;
		for (int i = 0; i < mArrayTime.size(); i++) {
			timeinfo = mArrayTime.get(i);
			if (info.date.compareTo(timeinfo.date) >= 0) {
				if (info.date.getYear() == timeinfo.date.getYear()
						&& info.date.getMonth() == timeinfo.date.getMonth()
						&& info.date.getDay() == timeinfo.date.getDay()) {
					timeinfo.array.add(0, info);
				} else {
					timeinfo = new TimeInfo();
					timeinfo.date = info.date;
					timeinfo.array.add(info);
					mArrayTime.add(i, timeinfo);
				}

				break;
			}
		}
		if (timeinfo == null) {
			timeinfo = new TimeInfo();
			timeinfo.date = info.date;
			timeinfo.array.add(info);
			mArrayTime.add(timeinfo);
		}

		DirInfo dirinfo = null;
		File dir = file.getParentFile();

		dirinfo = mArrayDir.get(dir.getName());
		if (dirinfo == null) {
			dirinfo = new DirInfo();
			mArrayDir.put(dir.getName(), dirinfo);
			dirinfo.dir = dir.getAbsolutePath();
			dirinfo.name = dir.getName();
		}
		dirinfo.array.add(0, info);
		NotifyListeners(change_type.add,info.id);
		return info.id;
	}

	public void RemoveImage(int id)
	{
		HashSet<Integer> hash = new HashSet<Integer>();
		hash.add(id);
		RemoveImage(hash);
	}
	public void RemoveImage(HashSet<Integer> items) {
		Iterator<Integer> itImage = items.iterator();
		while (itImage.hasNext()) {
			ImageInfo info = mArrayImage.get(itImage.next());
			info.removed = true;
			File f = new File(info.path);
			if (f.exists() && f.isFile()) {
				f.delete();
			}
		}
		Iterator<TimeInfo> itTime = mArrayTime.iterator();
		while (itTime.hasNext()) {
			TimeInfo timeinfo = itTime.next();
			Iterator<ImageInfo> it = timeinfo.array.iterator();
			while (it.hasNext()) {
				ImageInfo imageinfo = it.next();
				if (imageinfo.removed == true) {
					it.remove();
				}
			}
			if (timeinfo.array.isEmpty()) {
				itTime.remove();
			}
		}
		Iterator<Entry<String, DirInfo>> itDir = mArrayDir.entrySet()
				.iterator();
		while (itDir.hasNext()) {
			DirInfo dirinfo = itDir.next().getValue();
			Iterator<ImageInfo> it = dirinfo.array.iterator();
			while (it.hasNext()) {
				ImageInfo imageinfo = it.next();
				if (imageinfo.removed) {
					it.remove();
				}
			}
			if (dirinfo.array.isEmpty())
				itDir.remove();
		}
		NotifyListeners(change_type.delete,-1);
	}

	public void AddListener(ImageMgrListener listener) {
		mListeners.add(listener);
	}

	public void RemoveListener(ImageMgrListener listener) {
		for (int i = 0; i < mListeners.size(); i++) {
			if (mListeners.get(i) == listener) {
				mListeners.remove(i);
				break;
			}
		}
	}

	private void NotifyListeners(change_type type,int id) {
		for (int i = 0; i < mListeners.size(); i++) {
			if (mListeners.get(i) != null) {
				mListeners.get(i).OnDataChange(type,id);

			}
		}
	}

	public ArrayList<TimeInfo> GetGroups() {
		return mArrayTime;
	}

	public LinkedHashMap<String, DirInfo> GetDirArray() {
		return mArrayDir;
	}

	public DirInfo GetDirInfo(int index) {
		if (index < 0 || index >= mArrayDir.size())
			return null;
		Iterator<Entry<String, DirInfo>> it = mArrayDir.entrySet().iterator();
		int i = 0;
		while (it.hasNext()) {
			Entry<String, DirInfo> entry = it.next();
			if (i == index) {
				return entry.getValue();
			}
			i++;
		}
		return null;
	}

	public ImageInfo GetImage(int imageId) {
		if (imageId < 0 || imageId >= mArrayImage.size())
			return null;
		ImageInfo info = mArrayImage.get(imageId);

		if (info.removed == false)
			return info;
		else
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

	public int GetImageCount() {
		return mArrayImage.size();
	}
	public ArrayList<ImageInfo> GetLikeArray()
	{
		return mArrayLike;
	}
	public void AddLikeImage(int id)
	{
		ImageInfo info = this.GetImage(id);
		if(info != null)
		{
			info.like = true;
			mArrayLike.add(info);
			if(mDb != null)
			{
				mDb.execSQL("insert into table_like values(null , ? )"
						, new String[]{info.path});
			
			}
			 this.NotifyListeners(change_type.add_like, id);
		}
		
		 
	}
	public void RemoveLikeImage(int id)
	{
		ImageInfo info = this.GetImage(id);
		if(info != null)
		{
			for(int i = 0;i<mArrayLike.size();i++)
			{
				if(mArrayLike.get(i).id == id)
				{
					mArrayLike.remove(i);
					info.like = false;
					String sql =  String.format("delete from table_like where path='%s'",info.path);
					mDb.execSQL(sql);
					this.NotifyListeners(change_type.delete_like, id);
					break;
				}
			}
		}
		
	}
	public void NotifyImageRotate(int id)
	{
		this.NotifyListeners(change_type.rotate, id);
	}
}
