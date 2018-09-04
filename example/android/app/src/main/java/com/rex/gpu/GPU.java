package com.rex.gpu;

import android.graphics.Bitmap;
import android.view.Surface;
import com.rex.load.NativeLoad;

public class GPU {
	static {
		long so = NativeLoad.loadSo("libGPU.so");

        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "createTexture", "()I");
        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "destroyTexture", "(I)V");

        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "eglContext", "(Z)J");
        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "setRenderSurface", "(Landroid/view/Surface;)V");
        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "makeCurrent", "(J)V");
        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "destroyEGL", "(J)V");

        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "processTexture", "(II)V");
        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "processPicture", "(Ljava/lang/String;)V");
        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "processBytes", "([BIII)V");
        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "getBytes", "([B)V");
        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "getTexture", "()I");

        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "setOutputSize", "(II)V");
        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "setOutputFormat", "(I)V");

        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "setInputSize", "(II)V");
        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "setInputRotation", "(I)V");

        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "setPreviewMirror", "(Z)V");
        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "setOutputMirror", "(Z)V");
        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "setOutputRotation", "(I)V");

        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "setSmoothStrength", "(F)V");
        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "setWhitenStrength", "(F)V");

        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "setExtraFilter", "(Ljava/lang/String;)V");
        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "closeExtraFilter", "()V");
        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "setExtraParameter", "(F)V");
        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "setColorFilter", "(IF)V");
        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "setUnBlurRegion", "(III)V");

        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "setOutputView", "()V");
        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "removeOutputView", "()V");
        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "setViewFillMode", "(I)V");
        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "setViewOutputSize", "(II)V");

        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "setPreviewBlend", "(Ljava/lang/String;FFFFZ)V");
        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "setVideoBlend", "(Ljava/lang/String;FFFFZ)V");
        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "setPreviewBlendBitmap", "(Landroid/graphics/Bitmap;FFFFZ)V");
        NativeLoad.registJNIMethod(so, "com/rex/gpu/GPU", "setVideoBlendBitmap", "(Landroid/graphics/Bitmap;FFFFZ)V");
	}

    // processTexture函数要处理的texture类型，如果是surfaceTexture，则应该为OES类型
    public static final int GPU_TEXTURE_RGB = 0;
    public static final int GPU_TEXTURE_OES = 1;
    public static final int GPU_FILL_STRETCH = 0;       // 完全匹配，直接拉伸，默认填充选项
    public static final int GPU_FILL_RATIO = 1;         // 适配输出尺寸，可能有边框
    public static final int GPU_FILL_RATIOANDFILL = 2;  // 按照输出比例裁剪，不保留边框

    /// SurfaceTexture相关
    protected static native int createTexture();
    protected static native void destroyTexture(int texture);
    /// EGLContext
    protected native long eglContext(boolean active);
    public native void setRenderSurface(Surface surface);
    public native void makeCurrent(long context);
    public native void destroyEGL(long context);

    /// c处理
    public native void processTexture(int texture, int texture_type);
    public native void processPicture(String pic);
    public native void processBytes(byte[] bytes, int width, int height, int format);
    public native void getBytes(byte[] bytes);
    public native int getTexture();
    /// 输出
    public native void setOutputSize(int width, int height);
    public native void setOutputFormat(int format);
    /// 输入
    public native void setInputSize(int width, int height);
    public native void setInputRotation(int rotation);
    /// 镜像
    public native void setPreviewMirror(boolean mirror);
    public native void setOutputMirror(boolean mirror);
    public native void setOutputRotation(int rotation);

    /// 美颜
    public native void setSmoothStrength(float level);
    public native void setWhitenStrength(float level);

    /// 滤镜
    public native void setExtraFilter(String image);
    public native void closeExtraFilter();
    public native void setExtraParameter(float para);

    // 颜色滤镜
    public native void setColorFilter(int filter, float strength);
    // 设置不模糊的区域
    public native void setUnBlurRegion(int x, int y, int radius);
    public void clearColorFilter(){
        for (int i=0; i<=GPU_COLOR_VIGNETTE_FILTER; i++){
            setColorFilter(i, 0);
        }
    }
    /// 预览
    public native void setOutputView();
    public native void removeOutputView();
    public native void setViewFillMode(int mode);
    public native void setViewOutputSize(int width, int height);

    // blend可用于logo

    /**
     * 预览添加叠加渲染图片，可用于logo，仅用于预览，不会在视频流中生效
     * @param path  图片路径
     * @param x     叠加位置左上角x坐标，归一化坐标，0-1
     * @param y     叠加位置左上角y坐标，归一化坐标，0-1
     * @param w     叠加图片宽度，归一化坐标，0-1
     * @param h     叠加图片高度，归一化坐标，0-1
     * @param mirror 是否镜像
     */
    public native void setPreviewBlend(String path, float x, float y, float w, float h, boolean mirror);
    /**
     * 视频流添加叠加渲染图片，可用于logo，仅用于视频流，不会在预览窗口中生效
     * @param path  图片路径
     * @param x     叠加位置左上角x坐标，归一化坐标，0-1
     * @param y     叠加位置左上角y坐标，归一化坐标，0-1
     * @param w     叠加图片宽度，归一化坐标，0-1
     * @param h     叠加图片高度，归一化坐标，0-1
     * @param mirror 是否镜像
     */
    public native void setVideoBlend(String path, float x, float y, float w, float h, boolean mirror);

    public native void setPreviewBlendBitmap(Bitmap bitmap, float x, float y, float w, float h, boolean mirror);
    public native void setVideoBlendBitmap(Bitmap bitmap, float x, float y, float w, float h, boolean mirror);

    public void removePreviewBlend(){
        setPreviewBlend((String)null, 0, 0, 0, 0, false);
    }
    public void removeVideoBlend(){
        setVideoBlend((String)null, 0, 0, 0, 0, false);
    }

    protected long  mEGLContext = 0;
    protected int   mProcessMode = 0;
    protected static boolean init = false;

    protected GPU(boolean glcontext){
        mEGLContext = eglContext(glcontext);
    }

    protected GPU(Surface surface) throws Exception {
        mEGLContext = eglContext(true);
        if (surface!=null){
            setRenderSurface(surface);
        }
    }

    public void makeCurrent(){
        if (mEGLContext!=0) {
            makeCurrent(mEGLContext);
        }
    }

    public final static int GPU_COLOR_CONTRAST_FILTER = 0;       // 对比度
    public final static int GPU_COLOR_GAMMA_FILTER = 1;       // 曝光度
    public final static int GPU_COLOR_SATURATION_FILTER = 2;       // 饱和度
    public final static int GPU_COLOR_FADE_FILTER = 3;       // 褪色
    public final static int GPU_COLOR_BLUR_FILTER = 4;       // 模糊
    public final static int GPU_COLOR_SHARPNESS_FILTER = 5;       // 锐化
    public final static int GPU_COLOR_TEMPERATURE_FILTER = 6;      // 色温
    public final static int GPU_COLOR_TINT_FILTER = 7;       // 色调
    public final static int GPU_COLOR_HIGHLIGHTS_FILTER=8;       // 高光
    public final static int GPU_COLOR_SHADOWS_FILTER = 9;       // 阴影
    public final static int GPU_COLOR_VIGNETTE_FILTER = 10;      // 暗角
}