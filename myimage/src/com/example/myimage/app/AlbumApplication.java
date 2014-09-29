
package com.example.myimage.app;

import android.app.Application;
import android.content.Intent;
import android.util.Log;
import android.view.ViewConfiguration;

import com.example.myimage.data.ImageLoader2;
import com.example.myimage.data.ImageMgr;

import java.lang.reflect.Field;

public class AlbumApplication extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
        Log.d("AlbumApplication","onCreate");

        ImageLoader2.GetInstance().init(this);
        ImageMgr.GetInstance().init(this);
    }
}
