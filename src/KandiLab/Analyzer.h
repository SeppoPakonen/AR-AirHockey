#ifndef _KandiLab_Analyzing_h_
#define _KandiLab_Analyzing_h_

#include "VisionCtrl.h"

NAMESPACE_UPP

class Analyzer {
	
	// OpenCV backend
	cv::OrbFeatureDetector orb_detector;
	cv::MSER mser_detector;
	cv::FastFeatureDetector fast_detector;
	cv::BRISK brisk_detector;
	cv::OrbDescriptorExtractor orb_extractor;
	cv::BRISK brisk_extractor;
	cv::FREAK freak_extractor;
	cv::BriefDescriptorExtractor brief_extractor;
	
	#ifdef flagNONFREE
	cv::SiftFeatureDetector sift_detector;
	cv::SiftDescriptorExtractor sift_extractor;
	cv::SurfFeatureDetector surf_detector;
	cv::SurfDescriptorExtractor surf_extractor;
	#endif
	
	std::vector<cv::KeyPoint> keypoints_object;
	cv::Mat descriptors_object;
	enum {DETECTOR_ORB, DETECTOR_MSER, DETECTOR_FAST, DETECTOR_BRISK, DETECTOR_SIFT, DETECTOR_SURF};
	enum {DESCRIPTOR_ORB, DESCRIPTOR_BRISK, DESCRIPTOR_BRIEF, DESCRIPTOR_FREAK, DESCRIPTOR_SIFT, DESCRIPTOR_SURF};
	
	Size obj_sz;
	int detector_type, descriptor_type;
	TimeStop ts;
	
	int keypoint_total, average_keypoints;
	int keypoint_images;
	int enough_keypoint_frames;
	int test;
	int elapsed;
	int image_count;
	double average_time;
	
	Array<std::vector<cv::KeyPoint> > scene_keypoints;
	
	
public:
	Analyzer();
	
	const TimeStop& GetTS() const {return ts;}
	void SetObject(const cv::Mat& img_scene);
	void SetTest(int i);
	
	void Start(int type, int image_count);
	void Stop();
	//void Process(const cv::Mat& img_scene, int id);
	void ProcessDetectorTest(const cv::Mat& img_scene, int id);
	void ProcessDescriptorTest(const cv::Mat& img_scene, int id);
	void CalculateKeypoints(Array<cv::Mat>& cv_images);
	String GetTitle();
	double GetAverageTime() const {return average_time;}
	int GetKeypoints() const {return keypoint_total;}
	int GetAverageKeypoints() const {return average_keypoints;}
	
	#ifndef flagNONFREE
	static int GetDetectorCount() {return DETECTOR_BRISK+1;}
	static int GetDescriptorCount() {return DESCRIPTOR_FREAK+1;}
	#else
	static int GetDetectorCount() {return DETECTOR_SURF+1;}
	static int GetDescriptorCount() {return DESCRIPTOR_SURF+1;}
	#endif
	
};

END_UPP_NAMESPACE

#endif
