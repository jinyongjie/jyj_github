package com.example.myimage.ctrl;


import java.io.File;
import java.io.FileFilter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

import com.example.myimage.data.Helper;
import com.example.myimage.data.ImageMgr;
import com.example.myimage.data.Size;
import com.example.myimage.data.ImageMgr.FileComparator;
import com.example.myimage.R;

import android.app.Dialog;
import android.content.Context;
import android.graphics.Rect;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.TextView;

public class DialogChooseDir extends Dialog implements View.OnClickListener{
	public interface Listener
	{
		public void onOk(String str);
	}
	private Listener mListener;
	private Context mContext;
	public DialogChooseDir(Context context) {
		super(context);
		mContext = context;
		this.setContentView(R.layout.dlg_seldir);
		// TODO Auto-generated constructor stub
	}
	private ListView mList;
	private TextView mPath;
	MyAdapter mAdapter;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		mPath = (TextView) findViewById(R.id.path);
		findViewById(R.id.button_new).setOnClickListener(this);
		findViewById(R.id.button_ok).setOnClickListener(this);
		findViewById(R.id.button_cancel).setOnClickListener(this);
		
		
		mList = (ListView) findViewById(R.id.list);
		mAdapter = new MyAdapter();
		mAdapter.setDir("/sdcard");
		mList.setAdapter(mAdapter);
		
		Size sz = Helper.getScreenSize(mContext);
		this.getWindow().setLayout(sz.width*9/10, sz.height*9/10);
	}
	public void setListener(Listener l)
	{
		mListener = l;
	}
	class MyAdapter extends BaseAdapter implements View.OnClickListener
	{
		private String mCurPath;
		
		ArrayList<String> mData = new ArrayList<String>();
		public String getDir()
		{
			return mCurPath;
		}
		public void setDir(String dir)
		{
			
			mData.clear();
		
			File file = new File(dir);
			
			
			if(file.exists() && file.isDirectory())
			{
				
				mCurPath = file.getAbsolutePath();
				mPath.setText(mCurPath);
				
				File[] children = file.listFiles(new FileFilter()
				{
					@Override
					public boolean accept(File pathname) {
						// TODO Auto-generated method stub
						if(pathname != null && pathname.isDirectory())
							return true;
						
						return false;
				}});
				if(children != null)
				for(File f:children)
				{
					mData.add(f.getName());
				}
				Collections.sort(mData, new Comparator<String>()
						{
							@Override
							public int compare(String lhs, String rhs) {
								// TODO Auto-generated method stub
								
								return lhs.compareToIgnoreCase(rhs);
							}
					
						});
				if(file.getParentFile() != null)
				{
					
					mData.add(0, "..");
				}
			}
			notifyDataSetChanged();
		}
	
		@Override
		public int getCount() {
			// TODO Auto-generated method stub
			return mData.size();
		}

		@Override
		public Object getItem(int position) {
			// TODO Auto-generated method stub
			return mData.get(position);
		}

		@Override
		public long getItemId(int position) {
			// TODO Auto-generated method stub
			
			return position;
		}

		@Override
		public View getView(int position, View convertView, ViewGroup parent) {
			// TODO Auto-generated method stub
			Button button;
			if(convertView == null)
			{
				button = new Button(mContext);
				button.setBackgroundResource(R.drawable.selector_button);
				button.setTextColor(mContext.getResources().getColor(R.color.textWhite));
				button.setGravity(Gravity.LEFT|Gravity.CENTER_VERTICAL);
				
				button.setOnClickListener(this);
			}
			else
			{
				button = (Button) convertView;
			}
			
			button.setText(mData.get(position));
			Drawable d = mContext.getResources().getDrawable(
					R.drawable.folder);
			d.setBounds(new Rect(0, 0, d.getMinimumWidth(), d
					.getMinimumHeight()));
			button.setCompoundDrawables(d, null, null, null);
			
			return button;
		}

		@Override
		public void onClick(View v) {
			// TODO Auto-generated method stub
			Button btn = (Button)v;
			String str = btn.getText().toString();
			if(str.equals(".."))
			{
				File f = new File(mCurPath);
				setDir(f.getParent());
			}
			else
			{
				
				setDir(mCurPath + File.separator + str);
			}
		}
		
	}
	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		int id = v.getId();
		if(id == R.id.button_new)
		{
			
			final Dialog dialog = new Dialog(mContext);
		
			dialog.setContentView(R.layout.dialog_new_dir);
			dialog.setCancelable(true);
			
			dialog.getWindow().setLayout(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
			dialog.findViewById(R.id.ok).setOnClickListener(new View.OnClickListener(){

				@Override
				public void onClick(View v) {
					// TODO Auto-generated method stub
					EditText edit = (EditText) dialog.findViewById(R.id.edit);
					String str = edit.getText().toString();
					
					if(str != null )
					{
						str.trim();
						if(str.length() != 0)
						{
							String dir = mAdapter.getDir() + File.separator+ str;
							File file = new File(dir);
							if(!file.isDirectory())
							{
								file.mkdir();

							}
							
							if(mListener != null)
							{
								mListener.onOk(dir);
							}
							dismiss();
						}
					}
					dialog.dismiss();
				}
				
			});
			dialog.findViewById(R.id.cancel).setOnClickListener(new View.OnClickListener(){

				@Override
				public void onClick(View v) {
					// TODO Auto-generated method stub
					dialog.dismiss();
				}
				
			});
			
			dialog.show();
			
		}
		else if(id == R.id.button_ok)
		{
			if(mListener != null)
			{
				mListener.onOk(mAdapter.getDir());
			}
			dismiss();
		}
		else if(id == R.id.button_cancel)
		{
			this.dismiss();
			
		}
	}

}
