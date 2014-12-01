package com.example.browser_main;

import java.util.ArrayList;

import com.browser.home.HomePage;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;

public class MainActivity extends Activity implements OnClickListener {
	private MenuWindow mMenuWindow;
	private ViewGroup mContainer;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		//initTestRotate();
		//testPopup();
		findViewById(R.id.button_menu).setOnClickListener(this);
		mMenuWindow = (MenuWindow) findViewById(R.id.menu_widow);
		mContainer = (ViewGroup)findViewById(R.id.container);
		
		initHomePage();
	}

	private void initHomePage()
	{
		HomePage page = new HomePage(this,null);
		mContainer.addView(page);
		page.init();
	}
	private void initTestRotate()
	{
		TestRotate root = new TestRotate(this);
		setContentView(root);
		root.init();
	}


	@Override
	public void onClick(View v) {	
		// TODO Auto-generated method stub
		int id= v.getId();
		switch(id)
		{
		case R.id.button_menu:
			popup_menuwindow();
			break;
		}
	}
	private void popup_menuwindow()
	{
		if(mMenuWindow.getVisibility() == View.VISIBLE)
		{
			//mMenuWindow.setVisibility(View.GONE);
			mMenuWindow.animHide();
		}
		else
		{
			mMenuWindow.animShow();
			//mMenuWindow.setVisibility(View.VISIBLE);
		}
	}
}
