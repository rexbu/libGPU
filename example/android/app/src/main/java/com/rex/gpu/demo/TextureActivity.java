package com.rex.gpu.demo;

import android.app.Activity;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.hardware.Camera;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.TextView;

import com.rex.gpu.GPU;
import com.rex.utils.CameraUtil;
import com.rex.gpu.GPURawBytesCallback;
import com.rex.gpu.GPUVideoFrame;
import com.rex.utils.FileUtil;

import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class TextureActivity extends Activity implements SurfaceHolder.Callback{

    protected SurfaceView surfaceView = null;
    protected ImageView imageView = null;
    protected SurfaceHolder surfaceHolder = null;
    protected GPUVideoFrame videoFrame = null;
    protected boolean isFront = true;
    Camera.Size videoSize;

    protected Button swtichButton;
    protected Button propsButton;
    protected Button shaperButton;
    protected Spinner filterSpinner;
    protected TextView smoothText;
    protected SeekBar smoothValueSeek;
    protected SeekBar shaperValueSeek;
    protected boolean isProps = true;
    protected boolean isShaper = true;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_texture);
        //滤镜文件拷贝到可读目录
        try {
            this.copyFilterFile();
        } catch (Exception e) {
            e.printStackTrace();
        }

        surfaceView = (SurfaceView) findViewById(R.id.camera_surfaceView);
        surfaceHolder = surfaceView.getHolder();
        imageView = (ImageView)findViewById(R.id.cmaera_iamgeView);

        isFront = true;
        initView();
    }

    @Override
    protected void onResume() {
        super.onResume();
        videoSize = CameraUtil.openCamera(1280, 720, isFront);
        surfaceHolder.addCallback(this);
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {
        try {
            videoFrame = new GPUVideoFrame(surfaceHolder.getSurface());
        } catch (Exception e) {
            e.printStackTrace();
            return;
        }

        if (isFront){
            videoFrame.setCameraPosition(GPUVideoFrame.CAMERA_FACING_FRONT);
        }
        else {
            videoFrame.setCameraPosition(GPUVideoFrame.CAMERA_FACING_BACK);
        }

        videoFrame.setOutputImageOritation(Configuration.ORIENTATION_PORTRAIT);
        //设置推流视频镜像
        videoFrame.setMirrorFrontVideo(false);
        //设置预览镜像，true时预览为镜像（正常画面），false时为非镜像(左右颠倒)
        videoFrame.setMirrorFrontPreview(true);
        //设置推流视频镜像
        videoFrame.setMirrorBackVideo(false);
        //设置预览镜像，true时预览为镜像（左右颠倒），false时为非镜像(正常画面)
        videoFrame.setMirrorBackPreview(true);
        // 预览图像尺寸，如果和surface尺寸不一致，可能会被surface缩放
        videoFrame.setViewOutputSize(540, 960);
        // 和surface尺寸比例不一致时候的填充方式
        videoFrame.setViewFillMode(GPU.GPU_FILL_RATIO);
        // 用于编码的图片尺寸
        videoFrame.setOutputSize(360, 640);
        videoFrame.setVideoSize(videoSize.width, videoSize.height);

        videoFrame.setSmoothStrength(0.9f);
        videoFrame.setYuv420PCallback(new GPURawBytesCallback() {
            @Override
            public void outputBytes(byte[] bytes) {
                imageView.setImageBitmap(yuv420p2RGBABitmap(bytes, 360, 640));
            }
        });


        // 设置logo
        videoFrame.setPreviewBlend("/data/data/"+ TextureActivity.this.getPackageName() +"/logo.png", 0.8f, 0.1f, 0.1f, 0.1f, false);
        videoFrame.setVideoBlend("/data/data/"+ TextureActivity.this.getPackageName() +"/logo.png", 0.8f, 0.5f, 0.1f, 0.1f, false);
        videoFrame.start();
        try {
            com.rex.utils.CameraUtil.mCamera.setPreviewTexture(videoFrame.surfaceTexture());
            com.rex.utils.CameraUtil.mCamera.startPreview();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int format, int width, int height) {
        Configuration configuration = this.getResources().getConfiguration(); //获取设置的配置信息
        if (configuration.orientation==Configuration.ORIENTATION_LANDSCAPE){
            videoFrame.setOutputImageOritation(Configuration.ORIENTATION_LANDSCAPE);
        }
        else{
            videoFrame.setOutputImageOritation(Configuration.ORIENTATION_PORTRAIT);
        }

        Log.e("gpu", "change surface:"+width+"/"+height);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
        if (videoFrame!=null){
            videoFrame.stop();
        }
        videoFrame.destroy();
        com.rex.utils.CameraUtil.releaseCamera();
        videoFrame = null;
    }

    // 切换摄像头做的操作
    protected void rotateCamera(){
        if(isFront){
            isFront=false;
            videoFrame.setCameraPosition(GPUVideoFrame.CAMERA_FACING_BACK);
        }
        else{
            isFront=true;
            videoFrame.setCameraPosition(GPUVideoFrame.CAMERA_FACING_FRONT);
        }
        com.rex.utils.CameraUtil.releaseCamera();
        videoSize = com.rex.utils.CameraUtil.openCamera(1280, 720, isFront);
        videoFrame.setVideoSize(videoSize.width, videoSize.height);

        try {
            com.rex.utils.CameraUtil.mCamera.setPreviewTexture(videoFrame.surfaceTexture());
            com.rex.utils.CameraUtil.mCamera.startPreview();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    protected void initView(){
        swtichButton = (Button)findViewById(R.id.switchCamera);
        filterSpinner = (Spinner)findViewById(R.id.filterSpinner);
        smoothText = (TextView)findViewById(R.id.smoothText);
        smoothValueSeek = (SeekBar)findViewById(R.id.smoothValueBar);
        smoothText.setText("磨皮：0.9");
        smoothValueSeek.setProgress(90);

        swtichButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                rotateCamera();
            }
        });

        // 设置滤镜
        filterSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                if (videoFrame!=null){
                    if (i==0){
                        videoFrame.closeExtraFilter();
                    }
                    else{
                        String filter = getResources().getStringArray(R.array.filter_name)[i];

                        videoFrame.setExtraFilter("/data/data/"+TextureActivity.this.getPackageName()+"/"+filter+".png");
                        TextView filter_view = findViewById(R.id.filterText);
                        filter_view.setText(filter);
                    }
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });
        smoothValueSeek.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            public int value;
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                value = i;
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                if (videoFrame!=null){
                    float strength = value*(float)1.0/100;
                    videoFrame.setSmoothStrength(strength);
                    smoothText.setText("磨皮："+ strength);
                }
            }
        });
    }
    public Bitmap yuv420p2RGBABitmap(byte[] data, int width, int height) {
        int frameSize = width * height;
        int[] rgba = new int[frameSize];
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++) {
                int y = (0xff & ((int) data[i * width + j]));
                int u = (0xff & ((int) data[frameSize + (i >> 1) * width / 2 + (j >> 1)]));
                int v = (0xff & ((int) data[frameSize + width * height / 4 + (i >> 1) * width / 2 + (j >> 1)]));
                y = y < 16 ? 16 : y;
                int r = Math.round(1.164f * (y - 16) + 1.596f * (v - 128));
                int g = Math.round(1.164f * (y - 16) - 0.813f * (v - 128) - 0.391f * (u - 128));
                int b = Math.round(1.164f * (y - 16) + 2.018f * (u - 128));
                r = r < 0 ? 0 : (r > 255 ? 255 : r);
                g = g < 0 ? 0 : (g > 255 ? 255 : g);
                b = b < 0 ? 0 : (b > 255 ? 255 : b);
                rgba[i * width + j] = 0xff000000 + (r << 16) + (g << 8) + b;
            }
        Bitmap bmp = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        bmp.setPixels(rgba, 0, width, 0, 0, width, height);
        return bmp;
    }

    public Bitmap nv212RGBABitmap(byte[] data, int width, int height) {
        int frameSize = width * height;
        int[] rgba = new int[frameSize];
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++) {
                int y = (0xff & ((int) data[i * width + j]));
                int v = (0xff & ((int) data[frameSize + (i >> 1) * width + (j & ~1) + 0]));
                int u = (0xff & ((int) data[frameSize + (i >> 1) * width + (j & ~1) + 1]));
                y = y < 16 ? 16 : y;
                int r = Math.round(1.164f * (y - 16) + 1.596f * (v - 128));
                int g = Math.round(1.164f * (y - 16) - 0.813f * (v - 128) - 0.391f * (u - 128));
                int b = Math.round(1.164f * (y - 16) + 2.018f * (u - 128));
                r = r < 0 ? 0 : (r > 255 ? 255 : r);
                g = g < 0 ? 0 : (g > 255 ? 255 : g);
                b = b < 0 ? 0 : (b > 255 ? 255 : b);
                rgba[i * width + j] = 0xff000000 + (r << 16) + (g << 8) + b;
            }
        Bitmap bmp = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        bmp.setPixels(rgba, 0, width, 0, 0, width, height);
        return bmp;
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
    }
}
