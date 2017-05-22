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

@interface ViewController (){
    GPULineFilter* line;
    GPUIOSView* view;
}

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    bs_log_init("stdout");
    
    line = new GPULineFilter();
    view = new GPUIOSView(self.view.bounds);
    [self.view addSubview:view->uiview()];
    
    line->addTarget(view);
    gpu_point_t p[3];
    p[0].x = 50;
    p[0].y = 80;
    p[1].x = 600;
    p[1].y = 700;
    p[2].x = 20;
    p[2].y = 1000;
    line->setPoints(p, 3);
    
    gpu_colorf_t c[3];
    c[0].r = 0;
    c[0].g = 0;
    c[0].b = 0;
    c[0].a = 1.0;
    c[1].r = 0.4;
    c[1].g = 0.4;
    c[1].b = 0.4;
    c[1].a = 0.8;
    c[2].r = 0;
    c[2].g = 0;
    c[2].b = 0;
    c[2].a = 1.0;
    line->setColors(c, 3);
    // 线条宽度
    line->setLineWidth(30);
    
    // 循环画
    [NSTimer scheduledTimerWithTimeInterval:0.1 target:self selector:@selector(newFrame) userInfo:nil repeats:YES];
}

-(void)newFrame{
    line->newFrame();
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


@end
