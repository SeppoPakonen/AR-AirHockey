#include "MediaStream.h"

NAMESPACE_UPP

CvVideoStream::CvVideoStream() {
	
}
	
void CvVideoStream::CopyTo(CvVideoStream& vs) {
	vs.buffer = buffer;
}

Image CvVideoStream::GetImage(Rect area) const {
	Rect src = RectC(0, 0, buffer.cols, buffer.rows);
	if (!src.Contains(area))
		return Image();
	
	Size sz = area.GetSize();
	ImageBuffer ib(sz);
	RGBA* it = ib.Begin();
	for(int y = 0; y < sz.cy; y++) {
		for(int x = 0; x < sz.cx; x++) {
			cv::Vec3b color = buffer.at<cv::Vec3b>(cv::Point(area.left + x, area.top + y));
			it->r = color[2];
			it->g = color[1];
			it->b = color[0];
			it->a = 255;
			it++;
		}
	}
	
	return ib;
};

END_UPP_NAMESPACE
