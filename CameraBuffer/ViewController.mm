//
//  ViewController.m
//  CameraBuffer
//
//  Created by Victor on 20.07.15.
//  Copyright (c) 2015 Nestline. All rights reserved.
//

#import "ViewController.hpp"
#import <AVFoundation/AVFoundation.h>
#import "shape-detect.hpp"

@interface ViewController () <AVCaptureVideoDataOutputSampleBufferDelegate>

@end

@implementation ViewController
{
    AVCaptureSession* captureSession;
    AVCaptureVideoPreviewLayer* captureVideoPreviewLayer;
    AVCaptureVideoDataOutput* videoDataOutput;
    
    __weak IBOutlet UIImageView *myImage;
}

-(void) viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    
    // Create session and preview layer
    
    captureSession = [[AVCaptureSession alloc] init];
    captureSession.sessionPreset = AVCaptureSessionPresetHigh;
    
    // Create INPUT
    
    AVCaptureDevice *device = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
    NSError *error = nil;
    AVCaptureDeviceInput *input = [AVCaptureDeviceInput deviceInputWithDevice:device error:&error];
    if(!input) NSLog(@"ERROR: %@", error);
    
    // Create OUTPUT
    
    videoDataOutput = [[AVCaptureVideoDataOutput alloc] init];
    [videoDataOutput setAlwaysDiscardsLateVideoFrames:YES];
    [videoDataOutput setVideoSettings:[NSDictionary dictionaryWithObject:[NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA] forKey:(NSString*)kCVPixelBufferPixelFormatTypeKey]];
    [videoDataOutput setSampleBufferDelegate:self queue:dispatch_get_main_queue()];
    
    // Session configuration
    
    [captureSession addInput:input];
    [captureSession addOutput:videoDataOutput];
    [captureSession commitConfiguration];
    [captureSession startRunning];
}


- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection
{
    UIImage* image = [self imageFromSampleBuffer:sampleBuffer];
    myImage.image = image;
}

- (UIImage *) imageFromSampleBuffer:(CMSampleBufferRef) sampleBuffer
{
    // Get CGImageRef
    CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    CVPixelBufferLockBaseAddress(imageBuffer, 0);
    void *baseAddress = CVPixelBufferGetBaseAddress(imageBuffer);
    size_t bytesPerRow = CVPixelBufferGetBytesPerRow(imageBuffer);
    size_t width = CVPixelBufferGetWidth(imageBuffer);
    size_t height = CVPixelBufferGetHeight(imageBuffer);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    
    // Create a bitmap graphics context with the sample buffer data
    CGContextRef context = CGBitmapContextCreate(baseAddress, width, height, 8, bytesPerRow, colorSpace, kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst);
    CGImageRef quartzImage = CGBitmapContextCreateImage(context);
    
    // Unlock the pixel buffer
    CVPixelBufferUnlockBaseAddress(imageBuffer,0);
    
    // Free up the context and color space
    CGContextRelease(context);
    CGColorSpaceRelease(colorSpace);
    
    // Create an image object from the Quartz image
    UIImage *image = [UIImage imageWithCGImage:quartzImage];
    
    // Release the Quartz image
    CGImageRelease(quartzImage);
    
    return image;
}

@end
