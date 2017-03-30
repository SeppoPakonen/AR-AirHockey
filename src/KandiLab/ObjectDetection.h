#ifndef _VisionCtrl_ObjectDetection_h_
#define _VisionCtrl_ObjectDetection_h_

#include "Common.h"

NAMESPACE_UPP

class CvCircleDetector : public CvVideoStream {
	CvVideoStream* src;
	bool blur;
	
public:
	CvCircleDetector();
	
	virtual void Process(int ms);
	
};

class CvColorBlobDetector : public CvVideoStream {
	CvVideoStream* src;
	Pointf last_pt;
	
public:
	CvColorBlobDetector();
	
	virtual void Process(int ms);
	
	Pointf GetLast() const {return last_pt;}
	
};


END_UPP_NAMESPACE
#endif
