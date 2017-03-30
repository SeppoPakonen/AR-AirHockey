#include "Analyzer.h"

NAMESPACE_UPP

Analyzer::Analyzer() {
	test = -1;
	
}

void Analyzer::SetObject(const cv::Mat& img_object) {
	obj_sz.cx = img_object.cols;
	obj_sz.cy = img_object.rows;
	
	// Detect keypoints
	keypoints_object.clear();
	if (detector_type == DETECTOR_ORB)
		orb_detector.detect( img_object, keypoints_object );
	else if (detector_type == DETECTOR_MSER)
		mser_detector.detect( img_object, keypoints_object );
	else if (detector_type == DETECTOR_FAST)
		fast_detector.detect( img_object, keypoints_object );
	else if (detector_type == DETECTOR_BRISK)
		brisk_detector.detect( img_object, keypoints_object );
	#ifdef flagNONFREE
	else if (detector_type == DETECTOR_SIFT)
		sift_detector.detect( img_object, keypoints_object );
	else if (detector_type == DETECTOR_SURF)
		surf_detector.detect( img_object, keypoints_object );
	#endif
	else Panic("Unknown type");
	
	// Calculate descriptors
	if (descriptor_type == DESCRIPTOR_ORB)
		orb_extractor.compute( img_object, keypoints_object, descriptors_object );
	else if (descriptor_type == DESCRIPTOR_FREAK)
		freak_extractor.compute( img_object, keypoints_object, descriptors_object );
	else if (descriptor_type == DESCRIPTOR_BRIEF)
		brief_extractor.compute( img_object, keypoints_object, descriptors_object );
	else if (descriptor_type == DESCRIPTOR_BRISK)
		brisk_extractor.compute( img_object, keypoints_object, descriptors_object );
	#ifdef flagNONFREE
	else if (descriptor_type == DESCRIPTOR_SIFT)
		sift_extractor.compute( img_object, keypoints_object, descriptors_object );
	else if (descriptor_type == DESCRIPTOR_SURF)
		surf_extractor.compute( img_object, keypoints_object, descriptors_object );
	#endif
	else Panic("Unknown type");
	
	
	ts.Reset();
}

void Analyzer::Start( int type, int image_count) {
	
	// Detector test
	if (test == 0) {
		detector_type = type;
		descriptor_type = -1;
	}
	// Descriptor test
	else if (test == 1) {
		detector_type = -1;
		descriptor_type = type;
	}
	// FPS test
	else {
		
	}
	
	keypoint_total = 0;
	keypoint_images = 0;
	enough_keypoint_frames = 0;
	this->image_count = image_count;
	
	ts.Reset();
}

void Analyzer::Stop() {
	elapsed = ts.Elapsed();
	average_time = (double) elapsed / image_count;
	average_keypoints = keypoint_total / image_count;
}

String Analyzer::GetTitle() {
	String s;
	
	if (detector_type == DETECTOR_ORB) s = "Detector ORB";
	if (detector_type == DETECTOR_MSER) s = "Detector MSER";
	if (detector_type == DETECTOR_FAST) s = "Detector FAST";
	if (detector_type == DETECTOR_BRISK) s = "Detector BRISK";
	if (detector_type == DETECTOR_SIFT) s = "Detector SIFT";
	if (detector_type == DETECTOR_SURF) s = "Detector SURF";
	
	if (detector_type != -1 && descriptor_type != -1) s += ", ";
	
	if (descriptor_type == DESCRIPTOR_ORB) s += "Extractor ORB";
	if (descriptor_type == DESCRIPTOR_BRISK) s += "Extractor BRISK";
	if (descriptor_type == DESCRIPTOR_FREAK) s += "Extractor FREAK";
	if (descriptor_type == DESCRIPTOR_BRIEF) s += "Extractor BRIEF";
	if (descriptor_type == DESCRIPTOR_SIFT) s += "Extractor SIFT";
	if (descriptor_type == DESCRIPTOR_SURF) s += "Extractor SURF";
	
	return s;
}

END_UPP_NAMESPACE
