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
    GPUBezierFilter* line;
    GPUIOSView* view;
}

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    bs_log_init("stdout");
    
    line = new GPUBezierFilter();
    view = new GPUIOSView(self.view.bounds);
    [self.view addSubview:view->uiview()];
    
    line->addTarget(view);
    
    gpu_point_t p[5];
    p[0].x = 50;
    p[0].y = 80;
    p[1].x = 400;
    p[1].y = 500;
    p[2].x = 100;
    p[2].y = 800;
    p[3].x = 300;
    p[3].y = 1000;
    p[4].x = 150;
    p[4].y = 1150;
    line->setPoints(p, 5);
    
    gpu_colorf_t c[3];
    c[0].r = 0;
    c[0].g = 0;
    c[0].b = 0;
    c[0].a = 1.0;
    c[1].r = 0.7;
    c[1].g = 0.4;
    c[1].b = 0.5;
    c[1].a = 0.7;
    c[2].r = 0;
    c[2].g = 0;
    c[2].b = 0;
    c[2].a = 1.0;
    // 线条宽度
    //line->setLineWidth(5);
    line->setColors(c[0], c[1]);
    
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
