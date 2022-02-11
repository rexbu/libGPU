# libGPU
一个类似GPUImage的跨平台图片、视频处理库。

C++实现，完美支持安卓+ios，cpu占用率大约是GPUImage的1/3。

包含安卓+ios的美颜+滤镜+裁剪+转码功能

### 功能列表
* pipeline：多种filter串行叠加
* 滤镜：十几款滤镜
* 裁剪+缩放
* 叠加：随意添加元素、logo、水印等
* 转码，rgb <-> yuv <-> nv21、nv12、I420

### History
* 2018.3.27 适配opengles 3.0
* 2018.7.15 添加安卓、ios相机滤镜demo
* 2022.2.12 处理几个编译错误和滤镜加载错误，注意打开demo app前授予权限。

# English
An cross-platform open source framework for GPU-based image and video processing like GPUImage
