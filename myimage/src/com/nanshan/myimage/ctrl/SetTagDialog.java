package com.nanshan.myimage.ctrl;

import com.nanshan.myimage.R;
import com.nanshan.myimage.data.ImageMgr;
import com.nanshan.myimage.data.ImageMgr.ImageInfo;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.os.Parcelable;
import android.support.v4.view.PagerAdapter;
import android.support.v4.view.ViewPager;
import android.util.DisplayMetrics;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.GridView;
import android.widget.ImageView;

public class SetTagDialog extends Dialog
{
	String[] mTagList = {"自拍","朋友合照","家庭合照","美女们","帅哥们","宝宝","美食","聚餐","建筑","室内","风景","夜景","街景","天空","商品","植物","玩偶","表演","随拍","旅游"};
	int mImageId = -1;
	public SetTagDialog(Context context, int theme) {
		super(context, theme);
		// TODO Auto-generated constructor stub
	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		
		Button buttonClose = (Button)findViewById(R.id.button_close);
        buttonClose.setOnClickListener(new android.view.View.OnClickListener()
        {

			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				dismiss();
			}
        	
        });
        Button buttonOk = (Button)findViewById(R.id.button_ok);
        buttonOk.setOnClickListener(new android.view.View.OnClickListener()
        {

			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				
			}
        	
        });
        GridView gridview = (GridView)findViewById(R.id.grid);
        gridview.setAdapter(new MyAdapter(null));
	}

	   class MyAdapter extends BaseAdapter{ 
	        //上下文对象 
	        private Context context; 
	        //图片数组 

	        MyAdapter(Context context){ 
	            this.context = context; 
	        } 
	        public int getCount() { 
	           return mTagList.length;
	        } 
	 
	        public Object getItem(int item) { 
	            return item; 
	        } 
	 
	        public long getItemId(int id) { 
	            return id; 
	        } 
	         
	        //创建View方法 
	        public View getView(int position, View convertView, ViewGroup parent) { 
return null;
	        } 
	} 

	public void SetImageId(int id)
	{
		mImageId = id;
	}
}
