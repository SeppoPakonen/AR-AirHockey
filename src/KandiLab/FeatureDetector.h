#ifndef _VisionCtrl_FeatureDetector_h_
#define _VisionCtrl_FeatureDetector_h_

#include "Common.h"
#include "VideoMixer.h"

NAMESPACE_UPP

class CvImageMatchStream : public CvVideoStream {
	CvVideoStream* src;
	Size obj_sz;
	cv::OrbFeatureDetector detector;
	cv::OrbDescriptorExtractor extractor;
	std::vector<cv::KeyPoint> keypoints_object;
	cv::Mat descriptors_object;
	Vector<Pointf> match_corners, tl_av, tr_av, br_av, bl_av;
	CvVideoMixer* mixer;
	int mixer_ch;
	
	Pointf GetAverage(int i, const cv::Point2f& pt);
	
public:
	CvImageMatchStream();
	
	const Vector<Pointf>& GetMatch() const {return match_corners;}
	
	void SetObject(const Image& im);
	void SetMixer(CvVideoMixer& mix, int ch) {mixer = &mix; mixer_ch = ch;}
	
	virtual void Process(int ms);
	
};

END_UPP_NAMESPACE

#endif
