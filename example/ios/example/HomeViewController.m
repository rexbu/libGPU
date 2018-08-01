//
//  HomeViewController.m
//  example
//
//  Created by Rex on 2018/7/24.
//  Copyright © 2018年 Rex. All rights reserved.
//

#import "HomeViewController.h"
#import "ViewController.h"

@implementation HomeViewController

-(id) init{
    self = [super init];
    
    
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    CGSize size = self.view.bounds.size;
    UIButton* button  = [[UIButton alloc] initWithFrame:CGRectMake(size.width/2-60, size.height/2-30, 120, 60)];
    [button setTitle:@"点击进入" forState:UIControlStateNormal];
    [button setBackgroundColor:[UIColor blueColor]];
    [button addTarget:self action:@selector(enter) forControlEvents:UIControlEventTouchDown];
    [self.view addSubview:button];
}

-(void)enter{
    ViewController* view = [[ViewController alloc] init];
    [self presentViewController:view animated:YES completion:nil];
}
@end
