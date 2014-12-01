package com.example.browser_main;

import android.app.Activity;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;

public class TabSwitchActivity extends Activity {
	TabSwitch mTabSwitch;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		LayoutInflater inflater = LayoutInflater.from(this);
		View v =  inflater.inflate(R.layout.tab_switch, null);
		setContentView(v);
		mTabSwitch = (TabSwitch) findViewById(R.id.tab_switch);
		mTabSwitch.init();
	}
	
}
