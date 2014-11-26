package com.example.browser_main;

import com.browser.home.HomePage;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;

public class MainActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
	//	setContentView(R.layout.activity_main);

		initTestRotate();
	}

	private void initHomePage()
	{
		HomePage page = new HomePage(this,null);
		setContentView(page);
		page.init();
	}
	private void initTestRotate()
	{
		TestRotate root = new TestRotate(this);
		setContentView(root);
		root.init();
	}
}
