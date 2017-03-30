#include "Analyzer.h"

NAMESPACE_UPP

void Analyzer::SetTest(int i) {
	test = i;
	
	if (test == 1) {
		detector_type = DETECTOR_SURF;
	}
}

void Analyzer::ProcessDetectorTest(const cv::Mat& img_scene, int id) {
	
	
	// Skip if buffer is empty
	if (!img_scene.rows || !img_scene.cols) {LOG("ERROR. Image is empty."); return;}
	
	
	// Detect keypoints
	std::vector<cv::KeyPoint> keypoints_scene;
	#ifdef flagMSC15
	// TODO: remove this when opencv has vc14 libraries
	keypoints_scene.reserve(1000);
	#endif
	
	if (detector_type == DETECTOR_ORB)
		orb_detector.detect( img_scene, keypoints_scene );
	else if (detector_type == DETECTOR_MSER)
		mser_detector.detect( img_scene, keypoints_scene );
	else if (detector_type == DETECTOR_FAST)
		fast_detector.detect( img_scene, keypoints_scene );
	else if (detector_type == DETECTOR_BRISK)
		brisk_detector.detect( img_scene, keypoints_scene );
	#ifdef flagNONFREE
	else if (detector_type == DETECTOR_SIFT)
		sift_detector.detect( img_scene, keypoints_scene );
	else if (detector_type == DETECTOR_SURF)
		surf_detector.detect( img_scene, keypoints_scene );
	#endif
	else
		Panic("Wrong type");
	
	keypoint_images++;
	keypoint_total += keypoints_scene.size();
}

void Analyzer::CalculateKeypoints(Array<cv::Mat>& cv_images) {
	scene_keypoints.Clear();
	
	for(int i = 0; i < cv_images.GetCount(); i++) {
		cv::Mat& img_scene = cv_images[i];
		std::vector<cv::KeyPoint>& keypoints_scene = scene_keypoints.Add();
		
		#ifdef flagMSC15
		// TODO: remove this when opencv has vc14 libraries
		keypoints_scene.reserve(1000);
		#endif
		
		if (detector_type == DETECTOR_ORB)
			orb_detector.detect( img_scene, keypoints_scene );
		else if (detector_type == DETECTOR_MSER)
			mser_detector.detect( img_scene, keypoints_scene );
		else if (detector_type == DETECTOR_FAST)
			fast_detector.detect( img_scene, keypoints_scene );
		else if (detector_type == DETECTOR_BRISK)
			brisk_detector.detect( img_scene, keypoints_scene );
		#ifdef flagNONFREE
		else if (detector_type == DETECTOR_SIFT)
			sift_detector.detect( img_scene, keypoints_scene );
		else if (detector_type == DETECTOR_SURF)
			surf_detector.detect( img_scene, keypoints_scene );
		#endif
		else
			Panic("Wrong type");
		
		if (keypoints_scene.size() == 0) {
			LOG("WARNING: no keypoints found in image " << i);
		}
	}
}

void Analyzer::ProcessDescriptorTest(const cv::Mat& img_scene, int id) {
	
	// Skip if buffer is empty
	if (!img_scene.rows || !img_scene.cols) {LOG("ERROR. Image is empty."); return;}
	
	
	std::vector<cv::KeyPoint>& keypoints_scene = scene_keypoints[id];
	
	
	
	// Calculate descriptors (feature vectors)
	cv::Mat descriptors_scene;
	
	if (descriptor_type == DESCRIPTOR_ORB)
		orb_extractor.compute( img_scene, keypoints_scene, descriptors_scene );
	else if (descriptor_type == DESCRIPTOR_BRISK)
		brisk_extractor.compute( img_scene, keypoints_scene, descriptors_scene );
	else if (descriptor_type == DESCRIPTOR_FREAK)
		freak_extractor.compute( img_scene, keypoints_scene, descriptors_scene );
	else if (descriptor_type == DESCRIPTOR_BRIEF)
		brief_extractor.compute( img_scene, keypoints_scene, descriptors_scene );
	#ifdef flagNONFREE
	else if (descriptor_type == DESCRIPTOR_SIFT)
		sift_extractor.compute( img_scene, keypoints_scene, descriptors_scene );
	else if (descriptor_type == DESCRIPTOR_SURF)
		surf_extractor.compute( img_scene, keypoints_scene, descriptors_scene );
	#endif
	else Panic("Unknown type");
	
	
	
}




#if 0
void Analyzer::Process(const cv::Mat& img_scene, int id) {
	
	// Skip if buffer is empty
	if (!img_scene.rows || !img_scene.cols) return;
	if (!obj_sz.cx || !obj_sz.cy) return;
	
	// Detect keypoints
	std::vector<cv::KeyPoint> keypoints_scene;
	#ifdef flagMSC15
	// TODO: remove this when opencv has vc14 libraries
	keypoints_scene.reserve(1000);
	#endif
	
	if (detector_type == DETECTOR_ORB)
		orb_detector.detect( img_scene, keypoints_scene );
	else if (detector_type == DETECTOR_MSER)
		mser_detector.detect( img_scene, keypoints_scene );
	else if (detector_type == DETECTOR_FAST)
		fast_detector.detect( img_scene, keypoints_scene );
	else if (detector_type == DETECTOR_BRISK)
		brisk_detector.detect( img_scene, keypoints_scene );
	#ifdef flagNONFREE
	else if (detector_type == DETECTOR_SIFT)
		sift_detector.detect( img_scene, keypoints_scene );
	else if (detector_type == DETECTOR_SURF)
		surf_detector.detect( img_scene, keypoints_scene );
	#endif
	else
		Panic("Wrong type");
	
	int a = keypoints_object.size();
	int b = keypoints_scene.size();
	
	// Calculate descriptors (feature vectors)
	cv::Mat descriptors_scene;
	if (descriptor_type == DESCRIPTOR_ORB)
		orb_extractor.compute( img_scene, keypoints_scene, descriptors_scene );
	else if (descriptor_type == DESCRIPTOR_BRISK)
		brisk_extractor.compute( img_scene, keypoints_scene, descriptors_scene );
	else if (descriptor_type == DESCRIPTOR_FREAK)
		freak_extractor.compute( img_scene, keypoints_scene, descriptors_scene );
	else if (descriptor_type == DESCRIPTOR_BRIEF)
		brief_extractor.compute( img_scene, keypoints_scene, descriptors_scene );
	#ifdef flagNONFREE
	else if (descriptor_type == DESCRIPTOR_SIFT)
		sift_extractor.compute( img_scene, keypoints_scene, descriptors_scene );
	else if (descriptor_type == DESCRIPTOR_SURF)
		surf_extractor.compute( img_scene, keypoints_scene, descriptors_scene );
	#endif
	else Panic("Unknown type");
	
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
	
	keypoint_total += good_matches.size();
	keypoint_images++;
	
	for( int i = 0; i < good_matches.size(); i++ ) {
		// Get the keypoints from the good matches
		obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
		scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
	}
	
	// Update match area
	if (good_matches.size() >= 20) {
		enough_keypoint_frames++;
		
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
		Corners c;
		c.a.x = scene_corners[0].x;
		c.a.y = scene_corners[0].y;
		c.b.x = scene_corners[1].x;
		c.b.y = scene_corners[1].y;
		c.c.x = scene_corners[2].x;
		c.c.y = scene_corners[2].y;
		c.d.x = scene_corners[3].x;
		c.d.y = scene_corners[3].y;
		
		if (get_reference_corners) {
			reference_corners[id] = c;
		} else {
			Corners& ref_c = reference_corners[id];
			corner_dist.AddResult(Distance(ref_c.a, c.a));
			corner_dist.AddResult(Distance(ref_c.b, c.b));
			corner_dist.AddResult(Distance(ref_c.c, c.c));
			corner_dist.AddResult(Distance(ref_c.d, c.d));
		}
	}
}
#endif


END_UPP_NAMESPACE
