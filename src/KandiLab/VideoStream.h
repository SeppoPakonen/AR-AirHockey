#ifndef _MediaStream_VideoStream_h_
#define _MediaStream_VideoStream_h_

#include <Core/Core.h>
#include <Draw/Draw.h>
#include "Engine.h"

NAMESPACE_UPP

class CvVideoStream : public Module {
	
protected:
	cv::Mat buffer;
	
public:
	CvVideoStream();
	
	virtual void CopyTo(CvVideoStream& vs);
	
	CvVideoStream& operator << (CvVideoStream& vs) {vs.CopyTo(*this); return *this;}
	
	
	const cv::Mat& GetBuffer() const {return buffer;}
	Size GetSize() const {return Size(buffer.cols, buffer.rows);}
	Image GetImage(Rect area) const;
	
};


END_UPP_NAMESPACE

#endif
