#include "VisionCtrl.h"

NAMESPACE_UPP

inline double Degree(const Pointf& a, const Pointf& b, const Pointf& c) {
	Pointf v0, v1;
	v0.x = b.x - a.x;
	v0.y = b.y - a.y;
	v1.x = c.x - b.x;
	v1.y = c.y - b.y;
	double angle = atan2(v0.y - v1.y, v0.x - v1.x);
	double degree = angle * 180 / M_PI;
	return degree;
}


CvImageMatchStream::CvImageMatchStream() : detector(400) {
	SetInCount(1);
	SetOutCount(1);
	src = NULL;
	mixer = NULL;
	match_corners.SetCount(4, Pointf(0, 0));
	
}

void CvImageMatchStream::SetObject(const Image& im) {
	
	// Copy image to cv::Mat
	obj_sz = im.GetSize();
	cv::Mat img_object(obj_sz.cy, obj_sz.cx, IPL_DEPTH_16U);
	const RGBA* src = im.Begin();
	cv::Vec3b color;
	for(int y = 0; y < obj_sz.cy; y++) {
		for(int x = 0; x < obj_sz.cx; x++) {
			color[0] = src->b;
			color[1] = src->g;
			color[2] = src->r;
			img_object.at<cv::Vec3b>(cv::Point(x, y)) = color;
			src++;
		}
	}
	
	// Detect keypoints
	detector.detect( img_object, keypoints_object );
	
	// Calculate descriptors
	extractor.compute( img_object, keypoints_object, descriptors_object );
	
	// Set buffer image to same
	buffer = img_object;
	
}

void CvImageMatchStream::Process(int ms) {
	
	const Jack& vid_in = in[0];
	
	if (vid_in.IsEmpty()) {
		src = NULL;
		return;
	}
	
	if (src == NULL) {
		src = vid_in.Get<CvVideoStream>();
		if (!src) return;
	}
	
	// Get source image
	const cv::Mat& img_scene = src->GetBuffer();
	
	// Skip if buffer is empty
	if (!img_scene.rows || !img_scene.cols) return;
	if (!obj_sz.cx || !obj_sz.cy) return;
	
	// Detect keypoints
	std::vector<cv::KeyPoint> keypoints_scene;
	#ifdef flagMSC15
	// TODO: remove this when opencv has vc14 libraries
	keypoints_scene.reserve(1000);
	#endif
	detector.detect( img_scene, keypoints_scene );
	int a = keypoints_object.size();
	int b = keypoints_scene.size();
	
	// Calculate descriptors (feature vectors)
	cv::Mat descriptors_scene;
	extractor.compute( img_scene, keypoints_scene, descriptors_scene );
	
	// Matching descriptor vectors using FLANN matcher
	cv::BFMatcher matcher;
	std::vector< cv::DMatch > matches;
	#ifdef flagMSC15
	// TODO: remove this when opencv has vc14 libraries
	matches.reserve(1000);
	#endif
	try {
		matcher.match( descriptors_object, descriptors_scene, matches );
	}
	catch (...) {
		return;
	}
	
	double max_dist = 0; double min_dist = 100;
	
	// Quick calculation of max and min distances between keypoints
	for( int i = 0; i < descriptors_object.rows; i++ ) {
		double dist = matches[i].distance;
		if( dist < min_dist ) min_dist = dist;
		if( dist > max_dist ) max_dist = dist;
	}
	
	// Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
	std::vector< cv::DMatch > good_matches;
	
	for( int i = 0; i < descriptors_object.rows; i++ ) {
		if( matches[i].distance < 3*min_dist ) {
			good_matches.push_back( matches[i]);
		}
	}
	
	// Localize the object
	std::vector<cv::Point2f> obj;
	std::vector<cv::Point2f> scene;
	
	for( int i = 0; i < good_matches.size(); i++ ) {
		// Get the keypoints from the good matches
		obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
		scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
	}
	
	// Get corner references
	Pointf& tl = match_corners[0];
	Pointf& tr = match_corners[1];
	Pointf& br = match_corners[2];
	Pointf& bl = match_corners[3];
	
	
	// Update match area
	if (good_matches.size() >= 20) {
		
		cv::Mat H = cv::findHomography( obj, scene, CV_RANSAC );
		
		// Get the corners from the image_1 ( the object to be "detected" )
		std::vector<cv::Point2f> obj_corners(4);
		obj_corners[0] = cvPoint(0,0);
		obj_corners[1] = cvPoint( obj_sz.cx, 0 );
		obj_corners[2] = cvPoint( obj_sz.cx, obj_sz.cy );
		obj_corners[3] = cvPoint( 0, obj_sz.cy );
		std::vector<cv::Point2f> scene_corners(4);
		
		perspectiveTransform( obj_corners, scene_corners, H);
		
		// Copy scene corners from cv::Point to Upp::Point
		tl = GetAverage(0, scene_corners[0]);
		tr = GetAverage(1, scene_corners[1]);
		br = GetAverage(2, scene_corners[2]);
		bl = GetAverage(3, scene_corners[3]);
	}
	else if (tl_av.GetCount() == 0) return;
	
	// Copy scene image to buffer
	double lx = (double)(bl.x - tl.x) / (double)(obj_sz.cy-1);
	double ly = (double)(bl.y - tl.y) / (double)(obj_sz.cy-1);
	double rx = (double)(br.x - tr.x) / (double)(obj_sz.cy-1);
	double ry = (double)(br.y - tr.y) / (double)(obj_sz.cy-1);
	
	double im_ly = tl.y;
	double im_lx = tl.x;
	double im_ry = tr.y;
	double im_rx = tr.x;
	
	cv::Vec3b black;
	black[0] = 0;
	black[1] = 0;
	black[2] = 0;
	
	for (int y = 0; y < obj_sz.cy; y++) {
		
		double im_x = im_lx;
		double im_y = im_ly;
		
		double im_tx = (double)(im_rx - im_lx) / (double)(obj_sz.cx-1);
		double im_ty = (double)(im_ry - im_ly) / (double)(obj_sz.cx-1);
		
		for (int x = 0; x < obj_sz.cx; x++) {
			
			int ix = im_x;
			int iy = im_y;
			
			if (ix >= 0 && ix < img_scene.cols && iy >= 0 && iy < img_scene.rows) {
				buffer.at<cv::Vec3b>(cv::Point(x,y)) =
					img_scene.at<cv::Vec3b>(cv::Point(ix, iy));
			} else {
				buffer.at<cv::Vec3b>(cv::Point(x,y)) = black;
			}
			
			im_x += im_tx;
			im_y += im_ty;
		}
		
		im_lx += lx;
		im_ly += ly;
		im_rx += rx;
		im_ry += ry;
	}
	
	// Set mixer overlay coordinates
	if (mixer) {
		mixer->Set(mixer_ch, tl, tr, br, bl);
	}
	
	
}

Pointf CvImageMatchStream::GetAverage(int i, const cv::Point2f& pt) {
	int max;
	Pointf p(pt.x, pt.y);
	Vector<Pointf>* vec = 0;
	switch (i) {
		case 0: vec = &tl_av; break;
		case 1: vec = &tr_av; break;
		case 2: vec = &br_av; break;
		case 3: vec = &bl_av; break;
		default: return p;
	}
	Pointf av = p;
	for(int i = 0; i < vec->GetCount(); i++) {
		av += (*vec)[i];
	}
	av /= vec->GetCount() + 1;
	while (vec->GetCount() >= 5) vec->Remove(0);
	vec->Add(p);
	return av;
}

END_UPP_NAMESPACE
