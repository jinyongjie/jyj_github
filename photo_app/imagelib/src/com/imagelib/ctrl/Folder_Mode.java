package com.imagelib.ctrl;

import java.util.ArrayList;

import com.imagelib.data.ImageMgr.DirInfo;

import android.view.View;

public interface Folder_Mode {
	void setData(ArrayList<DirInfo> data);
	View getView();
	String getSelDir();
	void init(boolean selmode);
	void setEmptyView(View view);
}
