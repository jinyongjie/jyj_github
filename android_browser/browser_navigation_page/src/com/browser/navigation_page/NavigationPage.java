package com.browser.navigation_page;


import com.example.browser_navigation_page.R;

import android.content.Context;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.widget.FrameLayout;
import android.widget.LinearLayout;

public class NavigationPage extends FrameLayout {

	public NavigationPage(Context context, AttributeSet attrs) {
		super(context, attrs);
		// TODO Auto-generated constructor stub
	}

	public void init()
	{
		LayoutInflater inflater = LayoutInflater.from(getContext());
		inflater.inflate(R.layout.navigation_page, this);
	}
}
