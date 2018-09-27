package com.rex.gpu.demo;

import android.app.Activity;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.hardware.Camera;
import android.support.v7.app.AppCompatActivity;
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
import com.rex.gpu.GPURawBytesCallback;
import com.rex.gpu.GPUVideoFrame;
import com.rex.utils.CameraUtil;
import com.rex.utils.FileUtil;

import java.io.IOException;

public class PictureActivity extends Activity implements SurfaceHolder.Callback{
    protected SurfaceView surfaceView = null;
    protected ImageView imageView = null;
    protected SurfaceHolder surfaceHolder = null;

    protected GPUVideoFrame videoFrame = null;
    protected boolean isFront = true;

    protected Button swtichButton;
    protected Button propsButton;
    protected Button shaperButton;
    protected Spinner filterSpinner;
    protected Spinner ratioSpinner;
    protected TextView smoothText;
    protected SeekBar smoothValueSeek;
    protected SeekBar whitenValueSeek;

    protected String pic;
    protected byte[] picBytes;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_texture);

        surfaceView = (SurfaceView) findViewById(R.id.camera_surfaceView);
        surfaceHolder = surfaceView.getHolder();
        surfaceHolder.addCallback(this);

        imageView = (ImageView)findViewById(R.id.cmaera_iamgeView);
        picBytes = new byte[1280*1280*4];
        initView();
    }

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {
        try {
            // 如果不需要预览，则可以直接传入null参数
            videoFrame = new GPUVideoFrame(surfaceHolder.getSurface());
        } catch (Exception e) {
            e.printStackTrace();
            return;
        }

        //videoFrame.setOutputImageOritation(Configuration.ORIENTATION_PORTRAIT);
        videoFrame.setInputRotation(GPUVideoFrame.GPUNoRotation);
        // 预览图像尺寸，如果和surface尺寸不一致，可能会被surface缩放
        // videoFrame.setViewOutputSize(540, 960);
        // 和surface尺寸比例不一致时候的填充方式
        // videoFrame.setViewFillMode(GPU.GPU_FILL_RATIO);
        // 用于编码的图片尺寸
        videoFrame.setOutputSize(360, 640);
        videoFrame.setOutputFormat(GPUVideoFrame.GPU_NV12);

        videoFrame.setSmoothStrength(0.9f);
        videoFrame.setWhitenStrength(0.9f);
        // 颜色滤镜
        // videoFrame.clearColorFilter();
        // videoFrame.setColorFilter(GPU.GPU_COLOR_BLUR_FILTER, 1);

        // 预览设置logo，使用路径方式
        videoFrame.setPreviewBlend("/data/data/"+ this.getPackageName() +"/logo.png", 20, 40, 160, 240, false);
        // 视频流设置logo，使用bitmap方式
        Bitmap logo = FileUtil.readImage("/data/data/"+ this.getPackageName() +"/logo.png");
        videoFrame.setVideoBlendBitmap(logo, 20, 40, 160, 240, false);
        videoFrame.start();

        pic = "/data/data/"+this.getPackageName()+"/suyan.jpeg";
        videoFrame.processPicture(pic);
        videoFrame.getBytes(picBytes);
        imageView.setImageBitmap(nv122RGBABitmap(picBytes, 360, 640));
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
        videoFrame = null;
    }

    protected void initView(){
        swtichButton = (Button)findViewById(R.id.switchCamera);
        filterSpinner = (Spinner)findViewById(R.id.filterSpinner);
        ratioSpinner = findViewById(R.id.ratioSpinner);
        smoothText = (TextView)findViewById(R.id.smoothText);
        smoothValueSeek = (SeekBar)findViewById(R.id.smoothValueBar);
        whitenValueSeek = findViewById(R.id.whitenValueBar);
        smoothText.setText("磨皮：0.9");
        smoothValueSeek.setProgress(90);
        whitenValueSeek.setProgress(90);

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

                        videoFrame.setExtraFilter("/data/data/"+getBaseContext().getPackageName()+"/"+filter+".png");
                        TextView filter_view = findViewById(R.id.filterText);
                        filter_view.setText(filter);
                    }

                    // 每次操作都要处理
                    videoFrame.processPicture(pic);
                    videoFrame.getBytes(picBytes);
                    imageView.setImageBitmap(nv122RGBABitmap(picBytes, videoFrame.outputWidth, videoFrame.outputHeight));
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });

        // 设置显示比例
        ratioSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                if (videoFrame!=null){
                    switch (i){
                        case 0:
                            //videoFrame.setViewFillMode(GPU.GPU_FILL_RATIOANDFILL);
                            videoFrame.setViewFillMode(GPU.GPU_FILL_RATIO);
                            videoFrame.setFrameSize(720, 1280);
                            videoFrame.setOutputSize(720,1280);
                            videoFrame.processPicture(pic);
                            videoFrame.getBytes(picBytes);
                            imageView.setImageBitmap(nv122RGBABitmap(picBytes, 720, 1280));
                            break;
                        case 1: // 1:1
                            videoFrame.setViewFillMode(GPU.GPU_FILL_RATIO);
                            videoFrame.setFrameSize(640, 640);
                            videoFrame.setOutputSize(640,640);
                            videoFrame.processPicture(pic);
                            videoFrame.getBytes(picBytes);
                            imageView.setImageBitmap(nv122RGBABitmap(picBytes, 640, 640));
                            break;
                        case 2: // 3:2
                            videoFrame.setViewFillMode(GPU.GPU_FILL_RATIO);
                            videoFrame.setFrameSize(640, 720);
                            videoFrame.setOutputSize(640,720);
                            videoFrame.processPicture(pic);
                            videoFrame.getBytes(picBytes);
                            imageView.setImageBitmap(nv122RGBABitmap(picBytes, 640, 720));
                            break;
                        case 3: // 16:9
                            videoFrame.setViewFillMode(GPU.GPU_FILL_RATIO);
                            videoFrame.setFrameSize(540, 960);
                            videoFrame.setOutputSize(540,960);
                            videoFrame.processPicture(pic);
                            videoFrame.getBytes(picBytes);
                            imageView.setImageBitmap(nv122RGBABitmap(picBytes, 540, 960));
                            break;
                    }
                }
                // 每次操作都要处理
                if (videoFrame != null) {

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

                    // 每次操作都要处理
                    videoFrame.processPicture(pic);
                    videoFrame.getBytes(picBytes);
                    imageView.setImageBitmap(nv122RGBABitmap(picBytes, videoFrame.outputWidth, videoFrame.outputHeight));
                }
            }
        });
        whitenValueSeek.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
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
                    videoFrame.setWhitenStrength(strength);
                    // 每次操作都要处理
                    videoFrame.processPicture(pic);
                    videoFrame.getBytes(picBytes);
                    imageView.setImageBitmap(nv122RGBABitmap(picBytes, videoFrame.outputWidth, videoFrame.outputHeight));
                }
            }
        });

        final Button blankButton = findViewById(R.id.blankButton);
        blankButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(blankButton.getText().toString().equals("留白")){
                    blankButton.setText("填充");
                    videoFrame.setBlank(0, 0, 0, 0);

                }
                else{
                    blankButton.setText("留白");
                    videoFrame.setBlank(50, 255, 0, 0);
                }
                videoFrame.processPicture(pic);
                videoFrame.getBytes(picBytes);
                imageView.setImageBitmap(nv122RGBABitmap(picBytes, videoFrame.outputWidth, videoFrame.outputHeight));
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
    public Bitmap nv122RGBABitmap(byte[] data, int width, int height) {
        int frameSize = width * height;
        int[] rgba = new int[frameSize];
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++) {
                int y = (0xff & ((int) data[i * width + j]));
                int u = (0xff & ((int) data[frameSize + (i >> 1) * width + (j & ~1) + 0]));
                int v = (0xff & ((int) data[frameSize + (i >> 1) * width + (j & ~1) + 1]));
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
    public Bitmap rgba2Bitmap(byte[] data, int width, int height) {
        int frameSize = width * height;
        int[] rgba = new int[frameSize];
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++) {
                int r = 0xff & ((int) data[(i * width + j)*4]);
                int g = 0xff & ((int) data[(i * width + j)*4 + 1]);
                int b = 0xff & ((int) data[(i * width + j)*4 + 2]);
                rgba[i * width + j] = 0xff000000 + (r << 16) + (g << 8) + b;
            }
        Bitmap bmp = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        bmp.setPixels(rgba, 0, width, 0, 0, width, height);
        return bmp;
    }
}
