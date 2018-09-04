//
//  ImageViewController.m
//  example
//
//  Created by Rex on 2018/8/27.
//  Copyright © 2018年 Rex. All rights reserved.
//

#import "ImageViewController.h"
#import "GPUIOSView.h"
#include "GPU.h"
#include "GPUVideoFrame.h"

@interface ImageViewController (){
    GPUVideoFrame*  videoFrame;
    UIImage*        logo;
    UIImage*        image;
    UITextView*     smoothView;
    UIImageView*    videoView;
    UIButton*       ratioButton;
    
    // 滤镜滑动框
    UICollectionView*   filterScrollView;
    NSMutableArray*     filterButtonArray;
    NSArray*            filterNameArray;
    
    UIInterfaceOrientation old_orientation;
    int ratioIndex;
    
    GPUPicture*         picture;
    GPUPicture*         logo_picture;
    GPUSmoothFilter*    smoothFilter;
    GPUWhiteningFilter* whitenFilter;
    GPUColorFilter*     colorFilter;
    GPULookupFilter*    lookupFilter;
    GPUFilter*          extraFilter;
    GPUBlend2Filter*    blendFilter;
    GPUIOSView*         view;
}

@end

@implementation ImageViewController

-(id) init{
    self = [super init];
    old_orientation = UIInterfaceOrientationUnknown;
    ratioIndex = 0;
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    bs_log_init("stdout");
    // 很重要，ios中需要framebuffer初始化为GPUIOSBufferCache
    GPUIOSBufferCache::shareInstance();
    // 两种初始化GPUPicture的方式
    NSString* path = [[NSBundle mainBundle] pathForResource:@"suyan" ofType:@"jpeg"];
    picture = new GPUPicture(path.UTF8String);
    logo = [UIImage imageNamed:@"logo.png"];
    logo_picture = new GPUPicture(logo.CGImage);
    
    smoothFilter = new GPUSmoothFilter();
    smoothFilter->setExtraParameter(0.9);
    whitenFilter = new GPUWhiteningFilter();
    whitenFilter->setStrength(0.9);
    colorFilter = new GPUColorFilter();
    blendFilter = new GPUBlend2Filter();
    // 用于设置滤镜
    lookupFilter = new GPULookupFilter();
    
    view = new GPUIOSView(self.view.bounds);
    // 显示设置，按照图片比例等比缩放，预览view可能出现黑框
    [view->uiview() setFillMode:GPUFillModePreserveAspectRatio];
    // 将view添加到界面
    [self.view addSubview:view->uiview()];
    // 设置blend
    gpu_rect_t rect = {20, 20, 180, 300};
    blendFilter->setBlendImage(logo_picture, rect, false);
    
    picture->addTarget(smoothFilter);
    smoothFilter->addTarget(whitenFilter);
    whitenFilter->addTarget(lookupFilter);
    lookupFilter->addTarget(colorFilter);
    colorFilter->addTarget(blendFilter);
    blendFilter->addTarget(view);
    // 颜色滤镜
    // colorFilter->setBlur(1);
    picture->processImage();
    
    // 获取处理后的图片,从最后一个filter中获取pixelBuffer
    glFinish();
    CVPixelBufferRef pixelbuffer = ((GPUIOSFrameBuffer*)blendFilter->m_outbuffer)->getPixelBuffer();
    UIImage* image = [self pixelBuffer2Image:pixelbuffer];
    
    // 注意和view的相互覆盖
    [self initView];
}

-(void)dealloc{
    // 注意退出时要销毁申请的C++对象
    delete picture;
    delete logo_picture;
    delete smoothFilter;
    delete whitenFilter;
    delete lookupFilter;
    delete blendFilter;
    delete view;
}

- (void)initView{
    // 注意所有view的初始化都要放在VSVideoFrame的初始化之后，否则会被preview覆盖
    
    UIButton* record = [[UIButton alloc]initWithFrame:CGRectMake(10, 20, 60, 40)];
    [record setTitle:@"返回" forState:UIControlStateNormal];
    [record setBackgroundColor:[UIColor orangeColor]];
    [record addTarget:self action:@selector(back) forControlEvents:UIControlEventTouchUpInside];
    
    ratioButton = [[UIButton alloc]initWithFrame:CGRectMake(220, 20, 60, 40)];
    [ratioButton setTitle:@"比例" forState:UIControlStateNormal];
    [ratioButton setBackgroundColor:[UIColor orangeColor]];
    [ratioButton addTarget:self action:@selector(ratioSwitch) forControlEvents:UIControlEventTouchUpInside];
    
    [self.view addSubview:record];
    [self.view addSubview:ratioButton];
    
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
    
    UILabel* whitenLabel = [[UILabel alloc] initWithFrame:CGRectMake(10, 530, 40, 20)];
    [whitenLabel setText:@"美白"];
    [self.view addSubview:whitenLabel];
    UISlider* whiten = [[UISlider alloc] initWithFrame:CGRectMake(60, 530, 280, 20)];
    whiten.minimumValue = 0.0;
    whiten.maximumValue = 1.0;
    whiten.value = 0.9;
    whiten.continuous = NO;
    [whiten addTarget:self action:@selector(whitenValueChanged:) forControlEvents:UIControlEventValueChanged];
    [self.view addSubview:whiten];
    
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
    filterScrollView = [[UICollectionView alloc] initWithFrame:CGRectMake(10, 580, 320, 40) collectionViewLayout:flowLayout];
    filterScrollView.backgroundColor = [UIColor clearColor];
    [self.view addSubview:filterScrollView];
    filterScrollView.delegate = self;
    filterScrollView.dataSource = self;
    
    [filterScrollView registerClass:[UICollectionViewCell class] forCellWithReuseIdentifier:@"myCell"];
}

-(void)ratioSwitch{
    ratioIndex = (++ratioIndex)%3;
    gpu_size_t size = picture->m_image_size;
    switch (ratioIndex) {
        case 0: // 原始尺寸
            [ratioButton setTitle:@"比例" forState:UIControlStateNormal];
            // 获取图片原始尺寸
            lookupFilter->setOutputSize(size.width, size.height);
            lookupFilter->setFillMode(GPUFillModeStretch);
            picture->processImage();
            break;
        case 1: // 1：1
            [ratioButton setTitle:@"1 : 1" forState:UIControlStateNormal];
            lookupFilter->setFillMode(GPUFillModePreserveAspectRatioAndFill);
            lookupFilter->setOutputSize(720, 720);
            picture->processImage();
            break;
        case 2: // 3:2
            [ratioButton setTitle:@"3 : 2" forState:UIControlStateNormal];
            lookupFilter->setFillMode(GPUFillModePreserveAspectRatioAndFill);
            lookupFilter->setOutputSize(640, 960);
            picture->processImage();
            break;
        case 3: // 16:9
            [ratioButton setTitle:@"16: 9" forState:UIControlStateNormal];
            lookupFilter->setFillMode(GPUFillModePreserveAspectRatioAndFill);
            lookupFilter->setOutputSize(540, 960);
            picture->processImage();
            break;
        default:
            break;
    }
}

-(void)back{
    videoFrame = nil;
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(void)smoothValueChanged:(id)sender{
    NSString* text = [[NSString alloc] initWithFormat:@"磨皮:%0.2f", [(UISlider*)sender value]];
    [smoothView setText:text];
    smoothFilter->setExtraParameter([(UISlider*)sender value]);
    picture->processImage();
}
-(void)whitenValueChanged:(id)sender{
    whitenFilter->setStrength([(UISlider*)sender value]);
    picture->processImage();
}

-(void) viewWillDisappear:(BOOL)animated{
    [super viewWillDisappear:animated];
    old_orientation = UIInterfaceOrientationUnknown;
}

- (BOOL) shouldAutorotate
{
    return YES;
}
- (UIInterfaceOrientationMask)supportedInterfaceOrientations{
    return UIInterfaceOrientationMaskLandscapeRight|UIInterfaceOrientationMaskLandscapeLeft|UIInterfaceOrientationMaskPortrait|UIInterfaceOrientationMaskPortraitUpsideDown;
    //return UIInterfaceOrientationMaskPortrait;
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
        // 滤镜失效
        lookupFilter->stopLookup();
        picture->processImage();
    }
    else{
        // 滤镜生效
        NSString* path = [[NSBundle mainBundle]pathForResource:[filterNameArray objectAtIndex:tag] ofType:@"png"];
        lookupFilter->setLookupImage(path.UTF8String);
        picture->processImage();
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
