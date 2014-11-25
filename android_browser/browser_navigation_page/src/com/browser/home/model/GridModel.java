package com.browser.home.model;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.List;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import android.content.Context;
import android.graphics.Color;
import android.text.TextUtils;

public class GridModel {
	public static List getGridList(Context context)
	{
		try {
			return parse(new String(readBytes(context.getAssets().open("home_grid/home_grid.json")), "UTF-8"));
		} catch (UnsupportedEncodingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return null;
	}
	public static byte[] readBytes(InputStream paramInputStream) {
		if (paramInputStream == null)
			return null;
		ByteArrayOutputStream localByteArrayOutputStream = new ByteArrayOutputStream();
		byte[] arrayOfByte = new byte[5120];
		try {
			while (paramInputStream.read(arrayOfByte) != -1)
				localByteArrayOutputStream.write(arrayOfByte);
		} catch (IOException localIOException) {
		}
		return localByteArrayOutputStream.toByteArray();
	}

	public static List parse(String paramString) {
		ArrayList localArrayList = new ArrayList();
		if (TextUtils.isEmpty(paramString))
			return localArrayList;
		JSONArray localJSONArray = null;
		try {
			localJSONArray = new JSONObject(paramString).optJSONArray("data");
		} catch (JSONException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		if ((localJSONArray == null) || (localJSONArray.length() == 0))
			return localArrayList;
		int i = 0;
		while (i < localJSONArray.length()) {
			JSONObject localJSONObject = localJSONArray.optJSONObject(i);
			if (localJSONObject == null)
				;
			 
				i++;

				if (localJSONObject.optString("grid_type").equals("normal")) {
					GridItemData localo = new GridItemData();
					localo.setBgcolor(parseColor(
							localJSONObject.optString("bgcolor"), 16777215));
					localo.setFgcolor(parseColor(
							localJSONObject.optString("fgcolor"), 0));
					localo.setIcon(localJSONObject.optString("icon"));
					localo.setId(localJSONObject.optString("id"));

					localo.setUrl(localJSONObject.optString("url"));
					localo.setTitle(localJSONObject.optString("title"));
					localArrayList.add(localo);
				}
			
		}
		return localArrayList;
	}

	public static int parseColor(String paramString, int paramInt) {
		try {
			int i = Color.parseColor(paramString);
			return i;
		} catch (Exception localException) {
		}
		return paramInt;
	}
}
