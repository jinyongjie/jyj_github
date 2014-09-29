package com.example.myimage.ctrl;

import com.example.myimage.photoview.PhotoView;

import android.content.Context;
import android.graphics.Canvas;
import android.opengl.GLES10;
import android.util.AttributeSet;
import android.util.Log;

public class PhotoView2 extends PhotoView {

	public interface IMaxSizeListener
	{
		public void OnMaxSizeChange();
	};
	private IMaxSizeListener mListener;
	int mMaxSize = 0;
	public PhotoView2(Context context, AttributeSet attr) {
		super(context, attr);
		// TODO Auto-generated constructor stub
	}
	public void SetMaxSizeListener(IMaxSizeListener l)
	{
		mListener = l;
	}
	@Override
	protected void onDraw(Canvas canvas) {
		// TODO Auto-generated method stub
		if(mMaxSize == 0)
		{
			int[] maxTextureSize = new int[1];
			GLES10.glGetIntegerv(GLES10.GL_MAX_TEXTURE_SIZE, maxTextureSize, 0);
			mMaxSize = maxTextureSize[0];
			if(mListener != null)
			{
				mListener.OnMaxSizeChange();
			}
		}
		super.onDraw(canvas);
	}
	public int GetMaxBitmapSize()
	{
		return mMaxSize;
	}
}
