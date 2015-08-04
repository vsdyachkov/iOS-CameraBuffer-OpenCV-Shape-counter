//
//  shape-detect.h
//  CameraBuffer
//
//  Created by Victor on 21.07.15.
//  Copyright (c) 2015 Nestline. All rights reserved.
//

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cmath>
#include <iostream>
#include <CoreGraphics/CoreGraphics.h>

int findRects(CGImageRef img);
