//
//  AppDelegate.m
//  CameraBuffer
//
//  Created by Victor on 20.07.15.
//  Copyright (c) 2015 Nestline. All rights reserved.
//

#import "AppDelegate.hpp"
#import "shape-detect.hpp"

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    
    UIImage* img = [UIImage imageNamed:@"IMG_0194.JPG"];
    NSLog(@"rects: %d", findRects(img.CGImage));
    
    return YES;
}

@end
