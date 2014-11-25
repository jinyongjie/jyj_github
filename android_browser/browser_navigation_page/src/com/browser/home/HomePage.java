package com.browser.home;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.List;


import com.browser.home.model.GridItemData;
import com.browser.home.model.GridModel;
import com.example.browser_navigation_page.R;

import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.GradientDrawable;
import android.net.Uri;
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
	private static final int mColumCount = 5;
	public HomePage(Context context, AttributeSet attrs) {
		super(context, attrs);
		// TODO Auto-generated constructor stub
	}

	public void init() {
		LayoutInflater inflater = LayoutInflater.from(getContext());
		inflater.inflate(R.layout.home_view, this);
		mGridLayout = (LinearLayout) findViewById(R.id.grid_layout);
		initGrid();
	}
	public static int dp2px(Context context, float dpValue) {
		final float scale = context.getResources().getDisplayMetrics().density;
		return (int) (dpValue * scale + 0.5f);
	}
	private void initGrid() {
		ArrayList<GridItemData> localList = (ArrayList<GridItemData>) GridModel
				.getGridList(getContext());

		LayoutInflater inflater = LayoutInflater.from(getContext());
		int colum = mColumCount;
		int space = dp2px(getContext(),10);
		int line = localList.size() / colum
				+ ((localList.size() % colum) > 0 ? 1 : 0);

		for (int i = 0; i < line; i++) {
			GridRow row = new GridRow(getContext(),null);
			row.init(colum,space);
			row.setOrientation(LinearLayout.HORIZONTAL);
			LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
					LinearLayout.LayoutParams.MATCH_PARENT,
					LinearLayout.LayoutParams.WRAP_CONTENT);
			mGridLayout.addView(row, params);
			for (int j = 0; j < colum ; j++) {
				GridItemData data = null;
				if((i*colum+j) < localList.size())
				{
					data = localList.get(i * colum + j);
				}
				
				LinearLayout item = (LinearLayout) inflater.inflate(
						R.layout.home_grid_item, null);

				params = new LinearLayout.LayoutParams(
						LinearLayout.LayoutParams.WRAP_CONTENT,
						LinearLayout.LayoutParams.WRAP_CONTENT);
				params.bottomMargin = space;
				row.addView(item, params);

				
				TextView text = (TextView) item
						.findViewById(R.id.grid_item_title);
				if(data != null)
				{
					text.setText(data.getTitle());
				}
				else
				{
					text.setText("Ìí¼Ó");
				}
				

				GridItemImage image = (GridItemImage) item
						.findViewById(R.id.grid_item_image);
				if(data != null)
				{
					String icon = data.getIcon();
					int index = icon.indexOf("/");// todo
					String sub = icon.substring(index + 2);
					InputStream is = null;
					try {
						is = getContext().getAssets().open(sub);
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
					Bitmap bmp = BitmapFactory.decodeStream(is);
					Drawable background = createBackgroundDrawable(
							data.getBgcolor(), 50);
					
					
					image.setDrawable(background, bmp);
					image.setTag(data);
					image.setOnClickListener(new OnClickListener(){

						@Override
						public void onClick(View v) {
							// TODO Auto-generated method stub
							GridItemData data = (GridItemData) v.getTag();
							
							 Intent it = new Intent(Intent.ACTION_VIEW, Uri.parse(data.getUrl()));  
						       // it.setClassName("com.android.browser", "com.android.browser.BrowserActivity");  
						        getContext().startActivity(it);  
						        
						}
						
					});
				}
				else
				{
					Drawable background = createBackgroundDrawable(
							Color.WHITE, 40);
					Bitmap bmp = BitmapFactory.decodeResource(getResources(), R.drawable.shortcut_add);
					image.setDrawable(background, bmp);
				}


			}
		}
	}

	public static Drawable createBackgroundDrawable(int color, int radius) {
		GradientDrawable localGradientDrawable = new GradientDrawable();
		localGradientDrawable.setColor(color);
		localGradientDrawable.setCornerRadius(radius);
		return localGradientDrawable;
	}
}
