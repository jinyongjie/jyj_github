package com.browser.home;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.List;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import com.browser.home.model.GridItemData;
import com.example.browser_navigation_page.R;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.GradientDrawable;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.LinearLayout.LayoutParams;

public class HomePage extends FrameLayout {
	private LinearLayout mGridLayout;

	public HomePage(Context context, AttributeSet attrs) {
		super(context, attrs);
		// TODO Auto-generated constructor stub
	}

	public void init() {
		LayoutInflater inflater = LayoutInflater.from(getContext());
		inflater.inflate(R.layout.home_view, this);
		mGridLayout = (LinearLayout) findViewById(R.id.grid_layout);
		initData();
		
	}

	private void initData() {
		try {
			ArrayList<GridItemData> localList = (ArrayList<GridItemData>) parse(new String(readBytes(getContext()
					.getAssets().open("home_grid/home_grid.json")), "UTF-8"));
			
			LayoutInflater inflater = LayoutInflater.from(getContext());
			int colum = 5;
			int line = localList.size()/colum + ((localList.size()%colum) > 0 ?1:0);
			
			for(int i = 0;i<line;i++)
			{
				LinearLayout lineitem = new LinearLayout(getContext());
				lineitem.setOrientation(LinearLayout.HORIZONTAL);
				LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT,LinearLayout.LayoutParams.WRAP_CONTENT);
				mGridLayout.addView(lineitem,params);
				for(int j = 0;j<colum && (i*colum + j) < localList.size();j++)
				{
					GridItemData data = localList.get(i*colum+j);
					LinearLayout item = (LinearLayout) inflater.inflate(R.layout.home_grid_item,null);
					
					params = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT,LinearLayout.LayoutParams.WRAP_CONTENT);
					params.weight = 1;
					lineitem.addView(item,params);
					
					TextView text = (TextView) item.findViewById(R.id.grid_item_title);
					text.setText(data.getTitle());
					
					GridItemImage image = (GridItemImage) item.findViewById(R.id.grid_item_image);
					String icon = data.getIcon();
					int index = icon.indexOf("/");//todo
					String sub = icon.substring(index + 2);
					InputStream is = getContext().getAssets().open(sub);
					Bitmap bmp = BitmapFactory.decodeStream(is);
					Drawable background = createBackgroundDrawable(data.getBgcolor(),40);
					//image.setBackgroundDrawable(background);
					BitmapDrawable foreground = new BitmapDrawable(bmp);
					foreground.setBounds(0, 0, bmp.getWidth(), bmp.getHeight());
					image.setDrawable(background, foreground);
					
				}
				
			}
			
		} catch (UnsupportedEncodingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
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
	
	  public static Drawable createBackgroundDrawable(int color, int radius)
	  {
	    GradientDrawable localGradientDrawable = new GradientDrawable();
	    localGradientDrawable.setColor(color);
	    localGradientDrawable.setCornerRadius(radius);
	    return localGradientDrawable;
	  }
}
