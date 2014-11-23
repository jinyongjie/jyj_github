package com.imagelib.app;

import java.util.HashMap;


public class AppSetting {
	HashMap<String,Integer> mData = new HashMap<String,Integer>();
	private AppSetting()
	{
		
	}
	static private AppSetting mInstance;
	static public AppSetting instance()
	{
		if(mInstance == null)
			mInstance = new AppSetting();
		return mInstance;
	}
	public int getInt(String key)
	{
		Integer i =  mData.get(key);
		if(i == null)
			return 0;
		return i;
	}
	public void setInt(String key,int value)
	{
		mData.put(key, value);
	}
}
