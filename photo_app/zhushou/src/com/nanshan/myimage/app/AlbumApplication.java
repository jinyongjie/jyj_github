
package com.nanshan.myimage.app;

import android.app.Application;
import android.content.Intent;
import android.graphics.Color;
import android.util.Log;
import android.view.ViewConfiguration;





import java.lang.reflect.Field;

import com.imagelib.data.ImageLoader2;
import com.imagelib.data.ImageMgr;
import com.imagelib.app.AppSetting;
public class AlbumApplication extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
        Log.d("AlbumApplication","onCreate");

        ImageLoader2.GetInstance().init(this);
        ImageMgr.GetInstance().init(this);
        
        AppSetting setting = AppSetting.instance();
        setting.setInt("list_bkg", Color.WHITE);
        setting.setInt("list_time_date_color",0xff8b8b8b );
        setting.setInt("list_time_show_week",0 );
        setting.setInt("color_thumb_empty",0xffc8c8c8);
        setting.setInt("album_name_color",0xff323232 );
        setting.setInt("album_count_color",0xff8d8d8d );
    }
}
