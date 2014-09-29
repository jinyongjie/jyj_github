package com.example.myimage.data;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.lang.ref.SoftReference;
import java.math.BigDecimal;
import java.math.RoundingMode;
import java.net.HttpURLConnection;
import java.net.URL;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.Date;

import com.example.myimage.app.ActivityGallery;

import android.app.Activity;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.provider.MediaStore.Images.Media;
import android.util.DisplayMetrics;
import android.util.Log;
import android.util.Pair;
import android.view.View;
import android.widget.Toast;

public class Helper {
	public static int dp2px(Context context, float dpValue) {
		final float scale = context.getResources().getDisplayMetrics().density;
		return (int) (dpValue * scale + 0.5f);
	}

	public static int px2dp(Context context, float pxValue) {
		final float scale = context.getResources().getDisplayMetrics().density;
		return (int) (pxValue / scale + 0.5f);
	}

	public static void insertImage2MediaStore(Context context, String path) {
		File f = new File(path);
		if (f != null && f.exists()) {
			ContentValues values = new ContentValues();
			values.put(Media.DISPLAY_NAME, f.getName());
			values.put(Media.DESCRIPTION, f.getName());
			values.put(Media.MIME_TYPE, "image/jpeg");
			values.put(Media.DATA, path);

			Uri uri = context.getContentResolver().insert(
					Media.EXTERNAL_CONTENT_URI, values);
		}
	}
	public static void removeImageFromMediaStore(Context context, String path) {
	
		//	context.getContentResolver().query(Media.EXTERNAL_CONTENT_URI, projection, selection, selectionArgs, sortOrder)
		String str = String.format("%s='%s'", Media.DATA,path);
			context.getContentResolver().delete(Media.EXTERNAL_CONTENT_URI,str  , null);
		
	}

	public static void openImage(Context context, String path) {
		ArrayList<String> array = new ArrayList<String>();
		array.add(path);
		ActivityGallery.start(context,0,array);
	}

	public static String executeHttpGet(String strurl) {
		String result = null;
		URL url = null;
		HttpURLConnection connection = null;
		InputStreamReader in = null;
		try {
			url = new URL(strurl);
			connection = (HttpURLConnection) url.openConnection();
			in = new InputStreamReader(connection.getInputStream());
			BufferedReader bufferedReader = new BufferedReader(in);
			StringBuffer strBuffer = new StringBuffer();
			String line = null;
			while ((line = bufferedReader.readLine()) != null) {
				strBuffer.append(line);
			}
			result = strBuffer.toString();
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			if (connection != null) {
				connection.disconnect();
			}
			if (in != null) {
				try {
					in.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}

		}
		return result;
	}

	public static void statistic(String str) {
		new AsyncTask<String, String, String>() {

			@Override
			protected String doInBackground(String... params) {
				// TODO Auto-generated method stub
				executeHttpGet(params[0]);
				return null;
			}
		}.execute(str);

	}

	public static Size getScreenSize(Context context)
	{
		DisplayMetrics dm = new DisplayMetrics();
		((Activity) context).getWindowManager().getDefaultDisplay()
				.getMetrics(dm);	
		return  new Size(dm.widthPixels,dm.heightPixels);

	}
	public static Size getImageSize(String path)
	{
		BitmapFactory.Options options = new BitmapFactory.Options();
		options.inJustDecodeBounds = true;
		BitmapFactory.decodeFile(path, options);
		return new Size(options.outWidth,options.outHeight);
	}
	public static void runAsync(final Runnable r1,final Runnable r2)
	{
		final Handler handler = new Handler()
		{
			@Override
			public void handleMessage(Message msg) {
				// TODO Auto-generated method stub
				//super.handleMessage(msg);
				r2.run();
			}
		};
		
		new Thread()
		{
			@Override
			public void run() {
				// TODO Auto-generated method stub
				//super.run();
				r1.run();
			
				handler.sendEmptyMessage(0);
			}
			
		}.start();
	}
	static final String TAG="Helper";
	public static boolean moveFile(String oldpath,String newpath)
	{
		if(oldpath.compareToIgnoreCase(newpath) == 0)
			return true;
		File oldfile = new File(oldpath);
		File newfile = new File(newpath);
		if(newfile.exists())
			newfile.delete();
		boolean ret = oldfile.renameTo(newfile);
		Log.d(TAG,String.format("movefile rename %d", ret?1:0));
		if(ret == false)
		{
			copyFile(oldpath,newpath);
			oldfile.delete();
		}
		newfile = new File(newpath);
		return newfile.exists();
	}
	 public static boolean copyFile(String oldPath, String newPath) {   
		 if(oldPath.compareToIgnoreCase(newPath) == 0)
				return false;
		 
	       try {   
	           int bytesum = 0;   
	           int byteread = 0;   
	           File oldfile = new File(oldPath);   
	           if (oldfile.exists()) { //文件存在时   
	               InputStream inStream = new FileInputStream(oldPath); //读入原文件   
	               FileOutputStream fs = new FileOutputStream(newPath);   
	               byte[] buffer = new byte[1444];   
	               int length;   
	               while ( (byteread = inStream.read(buffer)) != -1) {   
	                   bytesum += byteread; //字节数 文件大小   
	                     
	                   fs.write(buffer, 0, byteread);   
	               }   
	              
	               fs.flush();
	               fs.close();
	               inStream.close();   
	               
	               File newfile = new File(newPath);
	               if(newfile.exists())
	               {
	            	  boolean ret = newfile.setLastModified(oldfile.lastModified());
	            	  Log.d(TAG,String.format("copy setlastmodify %d", ret?1:0));
	            	 
	            	   return true;
	               }
	              
	           }   
	       }   
	       catch (Exception e) {   
	           
	           e.printStackTrace();   
	  
	       }   
	  
	       return false;
	       
	   } 
	 
	    public static boolean ensureVisbility(final View v, final int wanttedVisibility) {
	        if (v.getVisibility() != wanttedVisibility) {
	            v.setVisibility(wanttedVisibility);
	            return true;
	        } else {
	            return false;
	        }
	    }

	    public static boolean ensureVisbility(final int wanttedVisibility, final View... vs) {
	        boolean changed = false;
	        for (final View v : vs) {
	            if (v.getVisibility() != wanttedVisibility) {
	                v.setVisibility(wanttedVisibility);
	                changed = true;
	            } else {
	                changed = false;
	            }
	        }
	        return changed;
	    }

	    public static void showToast(Context context, int s) {
	        showToast(context, context.getString(s));
	    }

	    public static void showToast(final Context context, final String s) {


	        if (Looper.getMainLooper() == Looper.myLooper()) {
	            Toast toast = Toast.makeText(context, s, Toast.LENGTH_SHORT);
	            hideToast();
	            toast.show();
	            sToastRef = new SoftReference<Toast>(toast);
	        } else {
	            if (context instanceof Activity) {
	                ((Activity) context).runOnUiThread(new Runnable() {
	                    @Override
	                    public void run() {
	                        showToast(context, s);
	                    }
	                });
	            }
	        }
	    }

	    public static void showToast(final Context context, Toast toast, final String s) {
	        if (Looper.getMainLooper() == Looper.myLooper()) {
	            hideToast();
	            toast.show();
	            sToastRef = new SoftReference<Toast>(toast);
	        } else {
	            if (context instanceof Activity) {
	                ((Activity) context).runOnUiThread(new Runnable() {
	                    @Override
	                    public void run() {
	                        showToast(context, s);
	                    }
	                });
	            }
	        }
	    }
	    private static SoftReference<Toast> sToastRef = null;

	    public static void hideToast() {
	        if (sToastRef != null) {
	            Toast previousToast = sToastRef.get();
	            if (previousToast != null) {
	                previousToast.cancel();
	            }
	        }
	    }
	    public static double Rounding(double d) {
	        BigDecimal bd = new BigDecimal(d);
	        bd.setScale(1, RoundingMode.HALF_UP);
	        return bd.floatValue();
	    }
	    public static String formatFileSize(long fileS) {
	        String fileSizeString = "";
	        if (fileS <= 0) {
	            return "0 KB";
	        }
	        if (fileS < 1024) {
	            fileSizeString = "1 KB";
	        } else if (fileS < 1048576) {
	            long inte = fileS / 1024;
	            if (inte > 100) {
	                DecimalFormat df = new DecimalFormat("##0");
	                fileSizeString = df.format(Rounding((double) fileS / 1024)) + " KB";
	            } else if (inte > 10) {
	                DecimalFormat df = new DecimalFormat("##0.0");
	                fileSizeString = df.format(Rounding((double) fileS / 1024)) + " KB";
	            } else {
	                DecimalFormat df = new DecimalFormat("##0.00");
	                fileSizeString = df.format(Rounding((double) fileS / 1024)) + " KB";
	            }
	        } else if (fileS < 1073741824) {
	            long inte = fileS / 1048576;
	            if (inte > 100) {
	                DecimalFormat df = new DecimalFormat("##0");
	                fileSizeString = df.format(Rounding((double) fileS / 1048576)) + " MB";
	            } else if (inte > 10) {
	                DecimalFormat df = new DecimalFormat("##0.0");
	                fileSizeString = df.format(Rounding((double) fileS / 1048576)) + " MB";
	            } else {
	                DecimalFormat df = new DecimalFormat("##0.00");
	                fileSizeString = df.format(Rounding((double) fileS / 1048576)) + " MB";
	            }
	        } else if (fileS < 1099511627776L) {
	            long inte = fileS / 1073741824;
	            if (inte > 100) {
	                DecimalFormat df = new DecimalFormat("##0");
	                fileSizeString = df.format(Rounding((double) fileS / 1073741824)) + " GB";
	            } else if (inte > 10) {
	                DecimalFormat df = new DecimalFormat("##0.0");
	                fileSizeString = df.format(Rounding((double) fileS / 1073741824)) + " GB";
	            } else {
	                DecimalFormat df = new DecimalFormat("##0.0");
	                fileSizeString = df.format(Rounding((double) fileS / 1073741824)) + " GB";
	            }
	        } else {
	            long inte = fileS / 1099511627776L;
	            if (inte > 100) {
	                DecimalFormat df = new DecimalFormat("##0");
	                fileSizeString = df.format((double) fileS / 1099511627776L) + " TB";
	            } else if (inte > 10) {
	                DecimalFormat df = new DecimalFormat("##0.0");
	                fileSizeString = df.format((double) fileS / 1099511627776L) + " TB";
	            } else {
	                DecimalFormat df = new DecimalFormat("##0.00");
	                fileSizeString = df.format((double) fileS / 1099511627776L) + " TB";
	            }
	        }
	        return fileSizeString;
	    }
	/* 
    static {
	        System.loadLibrary("helper");
	    }
	    */
	//public static native String  nativeMain();
	//public static native int  nativeCopyFile(String src,String dest);
	//public static native int  nativeTouch(String src,String dest);
	//public static native int setLastModifiedImpl(String path,long time);
	//   private static native boolean setLastModifiedImpl(String path, long time);
}
