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

@interface ViewController (){
    GPUVideoCamera* videoCamera;
}

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    bs_log_init("stdout");
    
    GPUVideoCamera* videoCamera = [[GPUVideoCamera alloc] initWithSessionPreset:AVCaptureSessionPresetHigh position:AVCaptureDevicePositionFront view:self.view];
    [videoCamera setMirrorFrontFacingCamera:TRUE];
    [videoCamera setMirrorFrontPreview:TRUE];
    [videoCamera setOutputSize:CGSizeMake(480, 640)];
    [videoCamera setOutputImageOrientation:UIInterfaceOrientationPortrait];
    [videoCamera startCameraCapture];
}

-(void)viewDidDisappear:(BOOL)animated{
    [super viewDidDisappear:animated];
    [videoCamera stopCameraCapture];
}
- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


@end
