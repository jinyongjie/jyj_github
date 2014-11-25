package com.browser.home.model;

import android.graphics.Bitmap;

public class GridItemData {

	private transient String mTitle;
	private String mUrl;
	private transient String mIcon;
	private transient Bitmap mBitmap;

	private transient int mFgcolor;
	private transient int mBgcolor;
	private transient String mId;

	public void setFgcolor(int paramInt) {
		this.mFgcolor = paramInt;
	}

	public void setBitmap(Bitmap paramBitmap) {
		this.mBitmap = paramBitmap;
	}

	public void setTitle(String paramString) {
		this.mTitle = paramString;
	}

	public String getTitle() {
		return this.mTitle;
	}

	public void setBgcolor(int paramInt) {
		this.mBgcolor = paramInt;
	}

	public void setUrl(String paramString) {
		this.mUrl = paramString;
	}

	public String getUrl() {
		return this.mUrl;
	}

	public void setIcon(String paramString) {
		this.mIcon = paramString;
	}

	public String getIcon() {
		return this.mIcon;
	}

	public void setId(String paramString) {
		this.mId = paramString;
	}

	public Bitmap e() {
		return this.mBitmap;
	}

	public int getFgcolor() {
		return this.mFgcolor;
	}

	public int getBgcolor() {
		return this.mBgcolor;
	}

	public String j() {
		return this.mId;
	}

}