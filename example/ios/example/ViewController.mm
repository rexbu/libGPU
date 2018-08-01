//
//  ViewController.m
//  example
//
//  Created by Visionin on 17/5/20.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import "ViewController.h"
#import "GPUIOSView.h"
#include "GPU.h"
#include "GPUVideoCamera.h"

static bool initViewFlag = true;

@interface ViewController (){
    GPUVideoCamera* videoCamera;
    
    UITextView*     smoothView;
    UIImageView*    videoView;
    
    // 滤镜滑动框
    UICollectionView*   filterScrollView;
    NSMutableArray*     filterButtonArray;
    NSArray*            filterNameArray;
    
    UIInterfaceOrientation old_orientation;
}

@end

@implementation ViewController
BOOL canRotateToAllOrientations;

-(id) init{
    self = [super init];
    old_orientation = UIInterfaceOrientationUnknown;
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    bs_log_init("stdout");
    
    videoCamera = [[GPUVideoCamera alloc] initWithSessionPreset:AVCaptureSessionPresetHigh position:AVCaptureDevicePositionFront view:self.view];
    [videoCamera setMirrorFrontFacingCamera:TRUE];
    [videoCamera setMirrorFrontPreview:TRUE];
    [videoCamera setOutputSize:CGSizeMake(480, 640)];
    [videoCamera setOutputImageOrientation:UIInterfaceOrientationPortrait];
    
    __block typeof(self) parent = self;
    videoCamera.bgraPixelBlock = ^(CVPixelBufferRef pixelBuffer, CMTime time){
        // 获取处理后视频帧
        // 视频流预览
        if (parent->videoView!=nil) {
            // 此处对性能影响比较大，如果不用测试视频流是否正确，可以把以下代码关闭
            UIImage* image = [self pixelBuffer2Image:pixelBuffer];
//            dispatch_async(dispatch_get_main_queue(), ^(){
//                [parent->videoView setImage:image];
//            });
        }
    };
    
    [videoCamera startCameraCapture];
}

- (void)initView{
    // 注意所有view的初始化都要放在VSVideoFrame的初始化之后，否则会被preview覆盖
    // 视频流预览
    videoView = [[UIImageView alloc] initWithFrame:CGRectMake(0, 150, 135, 240)];
    [videoView setBackgroundColor:[UIColor whiteColor]];
    // [self.view addSubview:videoView];
    
    UIButton* record = [[UIButton alloc]initWithFrame:CGRectMake(10, 20, 60, 40)];
    [record setTitle:@"返回" forState:UIControlStateNormal];
    [record setBackgroundColor:[UIColor orangeColor]];
    [record addTarget:self action:@selector(back) forControlEvents:UIControlEventTouchUpInside];
    
    UIButton* stop = [[UIButton alloc]initWithFrame:CGRectMake(80, 20, 60, 40)];
    [stop setTitle:@"停止" forState:UIControlStateNormal];
    [stop setBackgroundColor:[UIColor orangeColor]];
    [stop addTarget:self action:@selector(stopVideo) forControlEvents:UIControlEventTouchUpInside];
    UIButton* rotate = [[UIButton alloc]initWithFrame:CGRectMake(150, 20, 60, 40)];
    [rotate setTitle:@"切换" forState:UIControlStateNormal];
    [rotate setBackgroundColor:[UIColor orangeColor]];
    [rotate addTarget:videoCamera action:@selector(rotateCamera) forControlEvents:UIControlEventTouchUpInside];
    
    [self.view addSubview:record];
    [self.view addSubview:stop];
    [self.view addSubview:rotate];
    
    UILabel* smoothLabel = [[UILabel alloc] initWithFrame:CGRectMake(10, 480, 40, 20)];
    [smoothLabel setText:@"美颜"];
    [self.view addSubview:smoothLabel];
    UISlider* smooth = [[UISlider alloc] initWithFrame:CGRectMake(60, 480, 280, 20)];
    smooth.minimumValue = 0.0;
    smooth.maximumValue = 1.0;
    smooth.value = 0.9;
    smooth.continuous = NO;
    [smooth addTarget:self action:@selector(smoothValueChanged:) forControlEvents:UIControlEventValueChanged];
    [self.view addSubview:smooth];
    
    smoothView = [[UITextView alloc] initWithFrame:CGRectMake(10, 80, 100, 30)];
    [smoothView setBackgroundColor:[UIColor clearColor]];
    NSString* text = [[NSString alloc] initWithFormat:@"磨皮:%0.2f", 0.5];
    [smoothView setText:text];
    [self.view addSubview:smoothView];
    
    // 滤镜滑动窗口
    filterNameArray = [NSArray arrayWithObjects:@"无", @"filter0", @"filter1", @"filter2", @"filter3", @"filter4", @"filter5", @"filter6", @"filter7", @"filter8", @"filter9",
                       @"filter10",@"filter11", @"filter13", @"filter15", @"filter16", @"filter17", @"filter18", @"filter20", @"filter21", @"filter86", nil];
    filterButtonArray = [[NSMutableArray alloc] init];
    UICollectionViewFlowLayout *flowLayout = [[UICollectionViewFlowLayout alloc] init];
    flowLayout.scrollDirection = UICollectionViewScrollDirectionHorizontal;
    filterScrollView = [[UICollectionView alloc] initWithFrame:CGRectMake(10, 530, 320, 40) collectionViewLayout:flowLayout];
    filterScrollView.backgroundColor = [UIColor clearColor];
    [self.view addSubview:filterScrollView];
    filterScrollView.delegate = self;
    filterScrollView.dataSource = self;
    
    [filterScrollView registerClass:[UICollectionViewCell class] forCellWithReuseIdentifier:@"myCell"];
    
    // 屏幕旋转
    [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(orientationChanged:) name:UIDeviceOrientationDidChangeNotification object:nil];
}

-(void)back{
    [videoCamera stopCameraCapture];
    videoCamera = nil;
    initViewFlag = true;
    [self dismissViewControllerAnimated:YES completion:nil];
}

-(void)stopVideo{
    [videoCamera stopCameraCapture];
    videoCamera = nil;
}

- (void)viewDidLayoutSubviews {
    //self.view setBounds:CGRectMake(0, 0, self.view, CGFloat height)
    if (initViewFlag) {
        [self initView];
        initViewFlag = false;
    }
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(void)smoothValueChanged:(id)sender{
    NSString* text = [[NSString alloc] initWithFormat:@"磨皮:%0.2f", [(UISlider*)sender value]];
    [smoothView setText:text];
    [videoCamera setSmoothStrength:[(UISlider*)sender value]];
}

-(void)setNoneSmooth{
    [videoCamera setSmoothStrength:0];
}

-(void)rotateCamera{
    [videoCamera rotateCamera];
}

- (void) viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    
    //    We're going onto the screen, enable auto rotations
    canRotateToAllOrientations = YES;
    [videoCamera startCameraCapture];
}

- (void) viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    //    We're now on the screen, disable auto rotations
    canRotateToAllOrientations = YES;
}

-(void) viewWillDisappear:(BOOL)animated{
    [super viewWillDisappear:animated];
    old_orientation = UIInterfaceOrientationUnknown;
}

-(void) viewDidDisappear:(BOOL)animated{
    [super viewDidDisappear:animated];
    [videoCamera stopCameraCapture];
}

- (BOOL) shouldAutorotate
{
    return YES;
}
- (UIInterfaceOrientationMask)supportedInterfaceOrientations{
    return UIInterfaceOrientationMaskLandscapeRight|UIInterfaceOrientationMaskLandscapeLeft|UIInterfaceOrientationMaskPortrait|UIInterfaceOrientationMaskPortraitUpsideDown;
    //return UIInterfaceOrientationMaskPortrait;
}

- (void)orientationChanged:(NSNotification*)nofication{
    //UIDeviceOrientation orientation=[[UIDevice currentDevice] orientation];
    UIInterfaceOrientation orientation = [[UIApplication sharedApplication] statusBarOrientation];
    if (old_orientation == orientation) {
        return;
    }
    
    [videoCamera setViewFrame:self.view.bounds];
    
    old_orientation = orientation;
    
    switch (orientation) {
        case UIInterfaceOrientationPortraitUpsideDown:
            [videoCamera setOutputImageOrientation:UIInterfaceOrientationPortraitUpsideDown];
            break;
        case UIInterfaceOrientationLandscapeRight:
            [videoCamera setOutputImageOrientation:UIInterfaceOrientationLandscapeRight];
            break;
        case UIInterfaceOrientationLandscapeLeft:
            [videoCamera setOutputImageOrientation:UIInterfaceOrientationLandscapeLeft];
            break;
        case UIInterfaceOrientationPortrait:
            [videoCamera setOutputImageOrientation:UIInterfaceOrientationPortrait];
            break;
        default:
            break;
    }
    [self initView];
}

// 列数
- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section
{
    return filterNameArray.count;
}
// 行数
- (NSInteger)numberOfSectionsInCollectionView:(UICollectionView *)collectionView
{
    return 1;
}

- (UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath {
    static NSString *cellID = @"myCell";
    UICollectionViewCell *cell = [collectionView dequeueReusableCellWithReuseIdentifier:cellID forIndexPath:indexPath];
    if (cell.contentView.subviews.count <= 0) {
        UIButton* button = [[UIButton alloc] initWithFrame:CGRectMake(0, 0, 60, 40)];
        [button setTitle:[filterNameArray objectAtIndex:indexPath.item] forState:UIControlStateNormal];
        [button setTitleColor:[UIColor blueColor] forState:UIControlStateNormal];
        [button setTag:indexPath.item];
        [button addTarget:self action:@selector(filterSelcted:) forControlEvents:UIControlEventTouchUpInside];
        [cell.contentView addSubview:button];
    }
    else{
        UIButton* button = (UIButton*)[cell.contentView.subviews objectAtIndex:0];
        [button setTitle:[filterNameArray objectAtIndex:indexPath.item] forState:UIControlStateNormal];
        [button setTag:indexPath.item];
    }
    
    return cell;
}

-(void)filterSelcted:(id)sender{
    UIButton* button = (UIButton*)sender;
    NSInteger tag = button.tag;
    if (tag==0) {
        [videoCamera closeExtraFilter];
    }
    else{
        [videoCamera setExtraFilter:[filterNameArray objectAtIndex:tag]];
    }
}

//配置每个item的size
- (CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout sizeForItemAtIndexPath:(NSIndexPath *)indexPath
{
    return CGSizeMake(60, 40);;
}

//配置item的边距
- (UIEdgeInsets)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout insetForSectionAtIndex:(NSInteger)section
{
    return UIEdgeInsetsMake(5, 5, 5, 5);;
}

//点击item时触发
- (void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath
{
    [collectionView cellForItemAtIndexPath:indexPath].backgroundColor = [UIColor orangeColor];
}

//当前ite是否可以点击
- (BOOL) collectionView:(UICollectionView *)collectionView shouldSelectItemAtIndexPath:(nonnull NSIndexPath *)indexPath
{
    return YES;
}

-(UIImage*) pixelBuffer2Image:(CVPixelBufferRef) pixelBuffer;{
    CIImage *coreImage = [CIImage imageWithCVPixelBuffer:pixelBuffer];
    
    CIContext *context = [CIContext contextWithOptions:[NSDictionary dictionaryWithObject:[NSNumber numberWithBool:YES] forKey:kCIContextUseSoftwareRenderer]];//CPU渲染
    CGImageRef cgimg = [context createCGImage:coreImage fromRect:[coreImage extent]];
    UIImage* image = [UIImage imageWithCGImage:cgimg];
    CFRelease(cgimg);
    return image;
}
@end
