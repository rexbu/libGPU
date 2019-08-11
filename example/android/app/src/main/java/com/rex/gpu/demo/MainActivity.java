package com.rex.gpu.demo;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import com.rex.utils.FileUtil;

import java.io.FileOutputStream;
import java.io.InputStream;

public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button button = (Button)findViewById(R.id.textureButton);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(MainActivity.this, TextureActivity.class);
                MainActivity.this.startActivity(intent);
            }
        });
        findViewById(R.id.pictureButton).setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {
                Intent intent = new Intent(MainActivity.this, PictureActivity.class);
                MainActivity.this.startActivity(intent);
            }
        });

        //滤镜文件拷贝到可读目录
        try {
            copyFilterFile();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void copyFilterFile() throws Exception {
        String[] strings = this.getResources().getStringArray(R.array.filter_name);
        for (int i=1; i<strings.length; i++) {
            FileOutputStream fos = new FileOutputStream("/data/data/"+this.getPackageName()+"/"+strings[i]+".png");
            InputStream is = this.getAssets().open(strings[i]+".png");
            FileUtil.write(fos, is);
        }
        FileOutputStream fos = new FileOutputStream("/data/data/"+this.getPackageName()+"/logo.png");
        InputStream is = this.getAssets().open("logo.png");
        FileUtil.write(fos, is);
        fos = new FileOutputStream("/data/data/"+this.getPackageName()+"/suyan.jpeg");
        is = this.getAssets().open("suyan.jpeg");
        FileUtil.write(fos, is);
    }
}
