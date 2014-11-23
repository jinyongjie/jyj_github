package com.imagelib.ctrl;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;
import java.util.concurrent.Executors;





import com.imagelib.R;








import com.imagelib.app.ActivitySelDir;
import com.imagelib.ctrl.AdapterTime.EditModeListener;
import com.imagelib.ctrl.AdapterTime.IImageItem;
import com.imagelib.ctrl.PullToRefreshBase.OnRefreshListener;
import com.imagelib.data.Helper;
import com.imagelib.data.ImageLoader2;
import com.imagelib.data.ImageMgr;
import com.imagelib.data.ImageLoader2.ImageCallback;
import com.imagelib.data.ImageMgr.FileComparator;
import com.imagelib.data.ImageMgr.ImageInfo;
import com.imagelib.data.ImageMgr.ImageMgrListener;
import com.imagelib.data.ImageMgr.LineInfo;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.ColorDrawable;
import android.graphics.drawable.Drawable;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AbsListView;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.PopupWindow;
import android.widget.TextView;
import android.view.View.OnClickListener;
import android.view.ViewGroup.LayoutParams;

public class ListByTime extends FrameLayout implements OnClickListener {

	// private ArrayList<TimeInfo> mGroups = new ArrayList<TimeInfo>();
	private PullToRefreshListView mListView;
	private TextView mTextSelNum;
	private Button mButtonSelAll;
	private View mBarOp;
	private View mBarSel;
	private boolean mEnableToolbar = true;
	private AdapterTime mAdapter;
	private static final String TAG = "ListByTime";
	private TextView mEmpty;
	private View mProgress;
	private TextView mProgressText;

	public ListByTime(Context context) {
		super(context);
		// TODO Auto-generated constructor stub

	}

	public ListByTime(android.content.Context context,
			android.util.AttributeSet attrs) {
		super(context, attrs);

	}

	public void SetRefreshListener(OnRefreshListener refreshListener) {
		mListView.setOnRefreshListener(refreshListener);
	}

	public void setEnableToolbar(boolean enable) {
		mEnableToolbar = enable;
	}

	public void Init(int colum) {
		mTextSelNum = (TextView) findViewById(R.id.text_sel_num);
		mButtonSelAll = (Button) findViewById(R.id.button_selall);
		mBarOp = findViewById(R.id.bar_op);
		mBarSel = findViewById(R.id.bar_sel);
		mEmpty = (TextView) findViewById(R.id.empty);
		mProgress = findViewById(R.id.progress);
		mProgressText = (TextView) findViewById(R.id.tip);
		
		findViewById(R.id.button_more).setOnClickListener(this);
		findViewById(R.id.button_return).setOnClickListener(this);
		findViewById(R.id.button_del).setOnClickListener(this);
		findViewById(R.id.button_share).setOnClickListener(this);
		mButtonSelAll.setOnClickListener(this);

		mListView = (PullToRefreshListView) findViewById(R.id.list);
		mListView.getRefreshableView().setSelector(R.drawable.selector_grid);

		mAdapter = new AdapterTime(mListView.getRefreshableView(),colum) {
			protected IImageItem createImageItem() {
				return new ThumbImageView(getContext());
			}

			@Override
			protected void initItemData(IImageItem item) {
				// TODO Auto-generated method stub

			}
		};
		mListView.getRefreshableView().setAdapter(mAdapter);
		
		ImageMgr.GetInstance().addListener(mImageMgrListener);

		mAdapter.SetEditModeListener(new EditModeListener() {

			@Override
			public void onEditModeBegin() {
				// TODO Auto-generated method stub
				if (mEditModeListener != null)
					mEditModeListener.onEditModeBegin();

				updateSelectNum();
				if (mEnableToolbar)
					mBarOp.setVisibility(View.VISIBLE);

				mBarSel.setVisibility(View.VISIBLE);

			}

			@Override
			public void onEditModeFinish() {
				// TODO Auto-generated method stub
				if (mEditModeListener != null)
					mEditModeListener.onEditModeFinish();

				mBarSel.setVisibility(View.GONE);

				mBarOp.setVisibility(View.GONE);

			}

			@Override
			public void onSelCountChange(int count) {
				// TODO Auto-generated method stub
				updateSelectNum();
				if (mEditModeListener != null)
					mEditModeListener.onSelCountChange(count);
			}

		});
	}
	public void showScrollBar(boolean b)
	{
		mListView.getRefreshableView().setVerticalScrollBarEnabled(b);
	}
	public void setEmptyText(int r) {
		if (mEmpty != null)
			mEmpty.setText(r);
	}

	public void setEnablePullRefresh(boolean enable) {
		if (mListView != null) {
			mListView.setPullToRefreshEnabled(enable);
		}
	}
	public void setLineInfo(ArrayList<LineInfo> array,int count)
	{
		if (mAdapter != null)
			mAdapter.setData(array,count);
		
		
			
		
		updateSelectNum();
	}
	public void checkEmpty()
	{
		mListView.setEmptyView(mEmpty);
	}
	
	public void setArray(final ArrayList<ImageInfo> array) {
		/*
		 * new AsyncTask<Object,Object,Object>(){
		 * 
		 * @Override protected void onPreExecute() { // TODO Auto-generated
		 * method stub super.onPreExecute(); showProgress(true); }
		 * 
		 * @Override protected void onPostExecute(Object result) { // TODO
		 * Auto-generated method stub super.onPostExecute(result);
		 * 
		 * if (mAdapter != null) mAdapter.setData((ArrayList<LineInfo>) result);
		 * showProgress(false); if (ImageMgr.GetInstance().getInitState() ==
		 * ImageMgr.init_complete) { mListView.setEmptyView(mEmpty); } }
		 * 
		 * @Override protected Object doInBackground(Object... params) { // TODO
		 * Auto-generated method stub return mAdapter.parse(array); }
		 * 
		 * }.executeOnExecutor(Executors.newCachedThreadPool());
		 */
		this.setTip("初始化列表");
		showProgress(true);
		final Handler handler = new Handler() {

			@Override
			public void handleMessage(Message msg) {
				// TODO Auto-generated method stub
				super.handleMessage(msg);
				ArrayList<LineInfo> res = (ArrayList<LineInfo>) msg.obj;
				setLineInfo(res,res.size());
			
				showProgress(false);

			}

		};
		new Thread() {
			@Override
			public void run() {
				// TODO Auto-generated method stub
				super.run();
				Message msg = new Message();
				msg.what = 0;
				msg.obj = ImageMgr.GetInstance().parse(array,mAdapter.mColum);
				handler.sendMessage(msg);
			}

		}.start();
	}

	public void showProgress(boolean show) {
		if (mProgress != null) {
			if (show)
				mProgress.setVisibility(View.VISIBLE);
			else
				mProgress.setVisibility(View.GONE);
		}
	}

	public void setTip(String str) {
		mProgressText.setText(str);
	}

	public void removeImage(String path) {

	}

	private EditModeListener mEditModeListener = null;

	public void setEditModeListener(EditModeListener listener) {
		mEditModeListener = listener;

	}

	public boolean GetEditMode() {
		return mAdapter.GetEditMode();
	}

	private void updateSelectNum() {
		int count = mAdapter.getSelCount();
		if (mTextSelNum != null) {
			mTextSelNum
					.setText(String.format("已选择%d/%d张", count,mAdapter.getCanselCount()));
		}

		if (count == this.getCanselCount() && count != 0)
			mAdapter.updateSelButton(true, mButtonSelAll);
		else
			mAdapter.updateSelButton(false, mButtonSelAll);
	}

	public void setEditMode(boolean mode) {

		mAdapter.SetEditMode(mode);
	}

	public void delSelImages() {
		ArrayList<String> array = new ArrayList<String>();
		array.addAll(mAdapter.GetSelectItems());
		ImageMgr.GetInstance().deleteImage(array);
	}

	public int getSelCount() {
		return mAdapter.getSelCount();
	}

	public ArrayList<String> getSelItems() {
		return mAdapter.GetSelectItems();
	}

	public int getCanselCount() {
		return mAdapter.getCanselCount();
	}



	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		int id = v.getId();
		if (id == R.id.button_return) {

			this.setEditMode(false);
		} else if (v.getId() == R.id.button_del) {
			int selnum = getSelCount();
			if (selnum > 0) {
				showDelDialog(selnum);
			} else {
				Helper.showToast(this.getContext(), R.string.no_sel_image);
			}
		} else if (id == R.id.button_more) {
			showOpMenu(findViewById(R.id.button_more));

		} else if (id == R.id.button_selall) {
			int count = this.getSelCount();
			if (count == getCanselCount() && count != 0) {

				mAdapter.selAll(false);
			} else {
				mAdapter.selAll(true);

			}
		} else if (id == R.id.button_share) {
			/*
			 * if(true) { ArrayList<String> selItems =
			 * mAdapter.GetSelectItems(); String path = selItems.get(0); File f
			 * = new File(path); f.setLastModified(f.lastModified() +
			 * 24*60*60*1000);
			 * 
			 * return; }
			 */
			if (getSelCount() == 0) {
				Helper.showToast(this.getContext(), R.string.no_sel_image);
				return;
			}
			Intent intent = new Intent(Intent.ACTION_SEND_MULTIPLE);
			intent.setType("image/*");
			intent.putExtra(Intent.EXTRA_SUBJECT, "分享");

			ArrayList<Uri> imageUris = new ArrayList<Uri>();
			ArrayList<String> selItems = mAdapter.GetSelectItems();
			for (String path : selItems) {
				imageUris.add(Uri.fromFile(new File(path)));
			}
			intent.putParcelableArrayListExtra(Intent.EXTRA_STREAM, imageUris);

			intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);

			this.getContext().startActivity(Intent.createChooser(intent, "分享"));

			mAdapter.selAll(false);
		} else if (id == R.id.button_like) {
			if (getSelCount() == 0) {
				Helper.showToast(this.getContext(), R.string.no_sel_image);
				return;
			}
			ImageMgr.GetInstance().setLike(mAdapter.GetSelectItems(), true);
			Helper.showToast(this.getContext(), R.string.set_like_complete);
			mOpMenu.dismiss();
			mAdapter.selAll(false);
		} else if (id == R.id.button_move || id == R.id.button_copy) {
			if (getSelCount() == 0) {
				Helper.showToast(this.getContext(), R.string.no_sel_image);
				return;
			}
			Intent intent = new Intent();
			intent.setClass(this.getContext(), ActivitySelDir.class);
			if (id == R.id.button_move) {
				intent.putExtra("type", ActivitySelDir.type_move);
			} else {
				intent.putExtra("type", ActivitySelDir.type_copy);
			}

			ArrayList<String> list = new ArrayList<String>();
			list.addAll(mAdapter.GetSelectItems());
			intent.putStringArrayListExtra("list", list);
			this.getContext().startActivity(intent);

			mOpMenu.dismiss();
			mAdapter.selAll(false);
		}

	}

	private PopupWindow mOpMenu = null;

	public void showOpMenu(View v) {
		if (mOpMenu == null) {
			LayoutInflater inflater = LayoutInflater.from(this.getContext());

			View view = inflater.inflate(R.layout.menu_edit, null);

			mOpMenu = new PopupWindow(view, LayoutParams.WRAP_CONTENT,
					LayoutParams.WRAP_CONTENT, false);

			mOpMenu.setBackgroundDrawable(new BitmapDrawable());

			mOpMenu.setFocusable(true);
			mOpMenu.setOutsideTouchable(true);

			view.findViewById(R.id.button_copy).setOnClickListener(this);
			view.findViewById(R.id.button_like).setOnClickListener(this);
			view.findViewById(R.id.button_move).setOnClickListener(this);
		}

		mOpMenu.showAsDropDown(v, 0, -6 - Helper.dp2px(getContext(), 2));
		// mOpMenu.showAsDropDown(v,0, 0);

	}

	protected void showDelDialog(int num) {
		AlertDialog.Builder builder = new Builder(this.getContext());
		builder.setMessage(String.format("已选中%d张照片，确定要删除吗?", num));
		builder.setPositiveButton("确定", new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				dialog.dismiss();
				delSelImages();
				mAdapter.selAll(false);
			}

		});
		builder.setNegativeButton("取消", new DialogInterface.OnClickListener() {
			@Override
			public void onClick(DialogInterface dialog, int which) {
				dialog.dismiss();
			}
		});
		builder.create().show();
	}

	ImageMgrListener mImageMgrListener = new ImageMgrListener() {
		@Override
		public void onImageMgrNotify(int type, Object obj) {
			// TODO Auto-generated method stub
			 if (type == ImageMgr.refresh) {
				showProgress(false);
			} else if (type == ImageMgr.delete_begin) {
				mProgressText.setText(R.string.del_image);
				showProgress(true);
			} else if (type == ImageMgr.delete_end) {
				showProgress(false);
			} else if (type == ImageMgr.rotate) {
				mAdapter.updateImage((String) obj);
			}
		}
	};

	public boolean haveData() {
		return mAdapter.haveData();
	}
}
