#ifndef _VisionCtrl_VideoCapture_h_
#define _VisionCtrl_VideoCapture_h_

#include "Common.h"
#include "VideoStream.h"

NAMESPACE_UPP

class VideoCapture : public CvVideoStream {
	
	cv::VideoCapture cap;
	int id;
	Size target_sz;
	int frame_counter;
	String export_path;
	
public:
	VideoCapture();
	
	static int GetCount();
	bool Open(int i) {id = i; cap.open(i); return cap.isOpened();}
	void Close() {cap.release();}
	
	Size GetSize() {return Size(cap.get(CV_CAP_PROP_FRAME_WIDTH), cap.get(CV_CAP_PROP_FRAME_HEIGHT));}
	void SetSize(Size sz);
	VideoCapture& SetExportDirectory(String path);
	
	virtual void Process(int ms);
};

END_UPP_NAMESPACE

#endif
