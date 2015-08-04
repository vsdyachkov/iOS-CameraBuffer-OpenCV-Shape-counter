/**
 * Simple shape detector program.
 * It loads an image and tries to find simple shapes (rectangle, triangle, circle, etc) in it.
 * This program is a modified version of `squares.cpp` found in the OpenCV sample dir.
 */

#include "shape-detect.hpp"

/**
 * Helper function to find a cosine of angle between vectors
 * from pt0->pt1 and pt0->pt2
 */
static double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

void UIImageToMat(CGImageRef cgimage, cv::Mat& m)
{
    CGColorSpaceRef colorSpace = CGImageGetColorSpace(cgimage);
    CGFloat cols = CGImageGetWidth(cgimage);
    CGFloat rows = CGImageGetHeight(cgimage);
    m.create(rows, cols, CV_8UC4); // 8 bits per component, 4 channels
    CGContextRef contextRef = CGBitmapContextCreate(m.data, m.cols, m.rows, 8, m.step[0], colorSpace, kCGImageAlphaNoneSkipLast | kCGBitmapByteOrderDefault);
    CGContextDrawImage(contextRef, CGRectMake(0, 0, cols, rows), cgimage);
    CGContextRelease(contextRef);
    CGColorSpaceRelease(colorSpace);
}

int findRects(CGImageRef img)
{
    cv::Mat src;
    UIImageToMat(img, src);
    
//	cv::Mat src = cv::imread(path);
	if (src.empty())
		return -1;

	// Convert to grayscale
	cv::Mat gray;
	cv::cvtColor(src, gray, CV_BGR2GRAY);

	// Use Canny instead of threshold to catch squares with gradient shading
	cv::Mat bw;
	cv::Canny(gray, bw, 0, 50, 5);

	// Find contours
	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(bw.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	std::vector<cv::Point> approx;
	cv::Mat dst = src.clone();
    
    int tris = 0;
    int rects = 0;
    int pentas = 0;
    int hexas = 0;
    int cirs = 0;
    int contourses = 0;

    for (int i = 0; i < contours.size(); i++)
    {
        
        contourses++;
        
        // Approximate contour with accuracy proportional
        // to the contour perimeter
        cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);
        
        // Skip small or non-convex objects
        if (std::fabs(cv::contourArea(contours[i])) < 100 || !cv::isContourConvex(approx))
            continue;
        
        if (approx.size() == 3)
        {
            tris++;
        }
        else if (approx.size() >= 4 && approx.size() <= 6)
        {
            // Number of vertices of polygonal curve
            int vtc = approx.size();
            
            // Get the cosines of all corners
            std::vector<double> cos;
            for (int j = 2; j < vtc+1; j++)
                cos.push_back(angle(approx[j%vtc], approx[j-2], approx[j-1]));
            
            // Sort ascending the cosine values
            std::sort(cos.begin(), cos.end());
            
            // Get the lowest and the highest cosine
            double mincos = cos.front();
            double maxcos = cos.back();
            
            // Use the degrees obtained above and the number of vertices
            // to determine the shape of the contour
            if (vtc == 4 && mincos >= -0.1 && maxcos <= 0.3)
                rects++;
            else if (vtc == 5 && mincos >= -0.34 && maxcos <= -0.27)
                pentas++;
            else if (vtc == 6 && mincos >= -0.55 && maxcos <= -0.45)
                hexas++;
        }
        else
        {
            // Detect and label circles
            double area = cv::contourArea(contours[i]);
            cv::Rect r = cv::boundingRect(contours[i]);
            int radius = r.width / 2;
            
            if (std::abs(1 - ((double)r.width / r.height)) <= 0.2 &&
                std::abs(1 - (area / (CV_PI * std::pow(radius, 2)))) <= 0.2)
                cirs++;
        }
    }
    
    printf("\n3[%d] 4[%d] 5[%d] 6[%d] 0[%d] contours[%d]\n\n", tris, rects, pentas, hexas, cirs, contourses);
    
	return rects;
}

