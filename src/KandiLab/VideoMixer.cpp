#include "MediaStream.h"

NAMESPACE_UPP

CvVideoMixer::CvVideoMixer() {
	src = NULL;
	
}

CvVideoMixer& CvVideoMixer::Add(CvVideoStream& s, Rect area) {
	RectSource& src = rect_sources.Add();
	src.a = &s;
	src.b = area;
	return *this;
}

CvVideoMixer& CvVideoMixer::Add(CvVideoStream& s, Point tl, Point tr, Point br, Point bl) {
	PolygonSource& src = poly_sources.Add();
	src.a = &s;
	src.b.a = tl;
	src.b.b = tr;
	src.b.c = br;
	src.b.d = bl;
	return *this;
}

CvVideoMixer& CvVideoMixer::Set(int i, Point tl, Point tr, Point br, Point bl) {
	PolygonSource& src = poly_sources[i];
	src.b.a = tl;
	src.b.b = tr;
	src.b.c = br;
	src.b.d = bl;
	return *this;
}

CvVideoMixer& CvVideoMixer::SetSize(Size sz) {
	buffer = cv::Mat(sz.cy, sz.cx, IPL_DEPTH_16U);
	return *this;
}

CvVideoMixer& CvVideoMixer::SetExportDirectory(String path) {
	frame_counter = 0;
	export_path = path;
	return *this;
}

void CvVideoMixer::Process(int ms) {
	
	// Skip if buffer is empty
	if (!buffer.rows || !buffer.cols) return;
	
	for(int i = 0; i < rect_sources.GetCount(); i++) {
		RectSource& src = rect_sources[i];
		const CvVideoStream& vs = *src.a;
		const Rect& r = src.b;
		const cv::Mat& src_buf = vs.GetBuffer();
		
		if (!src_buf.cols || !src_buf.rows) continue;
		
		int w = r.Width();
		int h = r.Height();
		int type = src_buf.type();
		for(int y = 0; y < h; y++) {
			int y2 = y + r.top;
			int src_y = y * src_buf.rows / h;
			for(int x = 0; x < w; x++) {
				int x2 = x + r.left;
				if (x2 < 0 || x2 >= buffer.cols || y2 < 0 || y2 >= buffer.rows)
					continue;
				int src_x = x * src_buf.cols / w;
				if (type >= IPL_DEPTH_16U) {
					buffer.at<cv::Vec3b>(cv::Point(x2, y2)) =
						src_buf.at<cv::Vec3b>(cv::Point(src_x, src_y));
				}
				else {
					byte gray = src_buf.at<byte>(cv::Point(src_x, src_y));
					buffer.at<cv::Vec3b>(cv::Point(x2, y2)) = cv::Vec3b(gray, gray, gray);
				}
			}
		}
	}
	
	for(int i = 0; i < poly_sources.GetCount(); i++) {
		PolygonSource& src = poly_sources[i];
		const CvVideoStream& vs = *src.a;
		const Polygon& poly = src.b;
		const cv::Mat& src_buf = vs.GetBuffer();
		
		if (!src_buf.cols || !src_buf.rows) continue;
		
		Pointf tl = poly.a;
		Pointf tr = poly.b;
		Pointf br = poly.c;
		Pointf bl = poly.d;
		
		
		std::vector<cv::Point2f> corn_src, corn_dst;
		corn_src.push_back(cv::Point2f(0, 0));
		corn_src.push_back(cv::Point2f(src_buf.cols, 0));
		corn_src.push_back(cv::Point2f(src_buf.cols, src_buf.rows));
		corn_src.push_back(cv::Point2f(0, src_buf.rows));
		corn_dst.push_back(cv::Point2f(tl.x, tl.y));
		corn_dst.push_back(cv::Point2f(tr.x, tr.y));
		corn_dst.push_back(cv::Point2f(br.x, br.y));
		corn_dst.push_back(cv::Point2f(bl.x, bl.y));
		
		cv::Mat M;
		try {
			M = cv::getPerspectiveTransform(corn_src, corn_dst);
		}
		catch (...) {
			continue;
		}
		
		cv::Mat dst;
		cv::warpPerspective(src_buf, dst, M, buffer.size());
		
		for(int y = 0; y < buffer.rows; y++) {
			for(int x = 0; x < buffer.cols; x++) {
				cv::Vec3b color = dst.at<cv::Vec3b>(cv::Point(x, y));
				if (color[0] > 10 || color[1] > 10 || color[2] > 10)
					buffer.at<cv::Vec3b>(cv::Point(x, y)) = color;
			}
		}
	}
	
	
	if (!export_path.IsEmpty()) {
		String file = export_path + DIR_SEPS + Format("%05d.png", frame_counter);
		imwrite(file.Begin(), buffer);
	}
	
	frame_counter++;
}

END_UPP_NAMESPACE

