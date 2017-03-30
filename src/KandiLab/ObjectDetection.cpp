#include "VisionCtrl.h"

NAMESPACE_UPP
using namespace cv;

CvCircleDetector::CvCircleDetector() {
	SetInCount(1);
	SetOutCount(1);
	src = NULL;
	blur = false;
}

void CvCircleDetector::Process(int ms) {
	
	const Jack& vid_in = in[0];
	
	if (vid_in.IsEmpty()) {
		src = NULL;
		return;
	}
	
	if (src == NULL) {
		src = vid_in.Get<CvVideoStream>();
		if (!src) return;
	}
	
	const cv::Mat& img_scene = src->GetBuffer();
	if (!img_scene.rows || !img_scene.cols)
		return;
	
	/// Convert it to gray
	cvtColor( img_scene, buffer, CV_BGR2GRAY );
	
	// Reduce the noise so we avoid false circle detection
	if (blur)
		cv::GaussianBlur(buffer, buffer, cv::Size(9, 9), 2, 2 );
	
	std::vector<cv::Vec3f> circles;
	
	// Apply the Hough Transform to find the circles
	cv::HoughCircles( buffer, circles, CV_HOUGH_GRADIENT, 1, buffer.rows/8, 200, 100, 0, 0 );
	
	// Draw the circles detected
	for( size_t i = 0; i < circles.size(); i++ )
	{
		cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		
		// circle center
		cv::circle( buffer, center, 3, cv::Scalar(0,255,0), -1, 8, 0 );
		
		// circle outline
		cv::circle( buffer, center, radius, cv::Scalar(0,0,255), 3, 8, 0 );
	}
}






bool IsRed(byte r, byte g, byte b)
{
    byte rgbMin, rgbMax;
	byte h, s, v;
	
    rgbMin = r < g ? (r < b ? r : b) : (g < b ? g : b);
    rgbMax = r > g ? (r > b ? r : b) : (g > b ? g : b);

    v = rgbMax;
    if (v == 0)
    {
        h = 0;
        s = 0;
        return false;
    }

    s = 255 * long(rgbMax - rgbMin) / v;
    if (s == 0)
    {
        h = 0;
        return false;
    }

    if (rgbMax == r)
        h = 0 + 43 * (g - b) / (rgbMax - rgbMin);
    else if (rgbMax == g)
        h = 85 + 43 * (b - r) / (rgbMax - rgbMin);
    else
        h = 171 + 43 * (r - g) / (rgbMax - rgbMin);
	
	// low  hsv(0, 100, 100)
	// high hsv(179, 255, 255)
	if (h >= 0 && h <= 179 && s >= 100 && s <= 255 && v >= 100 && v <= 255)
		return true;
	else
		return false;
}
    

CvColorBlobDetector::CvColorBlobDetector() : last_pt(0,0) {
	SetInCount(1);
	SetOutCount(1);
	src = NULL;
}

void CvColorBlobDetector::Process(int ms) {
	
	const Jack& vid_in = in[0];
	
	if (vid_in.IsEmpty()) {
		src = NULL;
		return;
	}
	
	if (src == NULL) {
		src = vid_in.Get<CvVideoStream>();
		if (!src) return;
	}
	
	const cv::Mat& img_scene = src->GetBuffer();
	if (!img_scene.rows || !img_scene.cols)
		return;
	
	// Subtract other channels from red channel and treshold
	int max_b = 0;
	buffer = img_scene.clone();
	for(int y = 0; y < buffer.rows; y++) {
		for(int x = 0; x < buffer.cols; x++) {
			cv::Vec3b& color = buffer.at<cv::Vec3b>(cv::Point(x, y));
			byte b = 0;
			if (IsRed(color[2], color[1], color[0]))
				b = 255;
			color[2] = b;
			color[1] = b;
			color[0] = b;
			if (max_b < b) max_b = b;
		}
	}
	
	cv::SimpleBlobDetector::Params params;
	params.minDistBetweenBlobs = 50.0f;
	params.filterByInertia = false;
	params.filterByConvexity = false;
	params.filterByColor = false;
	params.filterByCircularity = false;
	params.filterByArea = true;
	params.minArea = 50.0f;
	params.maxArea = 50000.0f;
	
	cv::SimpleBlobDetector blob_detector(params);
	
	// detect
	vector<cv::KeyPoint> keypoints;
	#ifdef flagMSC15
	// TODO: remove this when opencv has vc14 libraries
	keypoints.reserve(1000);
	#endif
	blob_detector.detect(buffer, keypoints);
	
	
	// extract the x y coordinates of the keypoints
	for (int i = 0; i < keypoints.size() && i < 3; i++){
		double x = (double)keypoints[i].pt.x;
		double y = (double)keypoints[i].pt.y;
		cv::circle( buffer, cv::Point(x, y), 3, cv::Scalar(0,0,255), -1, 8, 0 );
	}
	
	if (keypoints.size()) {
		last_pt.x = (double)keypoints[0].pt.x / buffer.cols;
		last_pt.y = (double)keypoints[0].pt.y / buffer.rows;
	}
}


END_UPP_NAMESPACE
