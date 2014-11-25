package com.browser.home;

import android.content.Context;
import android.graphics.Color;
import android.graphics.PorterDuff.Mode;
import android.graphics.drawable.Drawable;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.widget.ImageView;

public class ColorImageView extends ImageView {
	private boolean mPressed = false;

	public ColorImageView(Context paramContext, AttributeSet paramAttributeSet) {
		super(paramContext, paramAttributeSet);
		this.setClickable(true);
	}

	public boolean onFilterTouchEventForSecurity(MotionEvent paramMotionEvent) {
		if (paramMotionEvent.getAction() == 1)
			setPressed(false);
		return super.onFilterTouchEventForSecurity(paramMotionEvent);
	}

	public void setPressed(boolean paramBoolean) {
		Drawable localDrawable = getDrawable();
		if ((localDrawable != null) && (this.mPressed != paramBoolean)) {
			this.mPressed = paramBoolean;
			if (this.mPressed)

				localDrawable.setColorFilter(Color.rgb(153, 153, 153),
						Mode.MULTIPLY);
			else
				localDrawable.clearColorFilter();

			invalidate();
		}

		super.setPressed(paramBoolean);
	}
}
