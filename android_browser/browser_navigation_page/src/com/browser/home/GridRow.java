package com.browser.home;

import com.example.browser_navigation_page.R;

import android.content.Context;
import android.util.AttributeSet;
import android.view.View;
import android.widget.LinearLayout;

public class GridRow extends LinearLayout{
	private int mColumCount = 5;
	private int mSpace = 0;
	public GridRow(Context context, AttributeSet attrs) {
		super(context, attrs);
		// TODO Auto-generated constructor stub
	}
	
	public void init(int count,int space)
	{
		mColumCount = count;
		mSpace = space;
	}
	@Override
	protected void onLayout(boolean changed, int l, int t, int r, int b) {
		// TODO Auto-generated method stub
		int size = ((r - l) - mSpace*(mColumCount - 1) )/mColumCount;
		for(int i= 0;i<this.getChildCount();i++)
		{
			View child = this.getChildAt(i);
			View image = child.findViewById(R.id.grid_item_image);
			LayoutParams param = (LayoutParams) image.getLayoutParams();
			param.width = size;
			param.height = size;
			if(i < getChildCount() - 1)
			{
				LinearLayout.LayoutParams param2 = (LayoutParams) child.getLayoutParams();
				param2.rightMargin = mSpace;
			}

		}
		super.onLayout(changed, l, t, r, b);
	}

}
