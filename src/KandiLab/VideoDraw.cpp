#include "VisionCtrl.h"

NAMESPACE_UPP

CvVideoDraw::CvVideoDraw() {
	src = NULL;
	SetInCount(1);
	SetOutCount(1);
	
}

void CvVideoDraw::Process(int ms) {
	const Jack& vid_in = in[0];
	
	if (vid_in.IsEmpty()) {
		src = NULL;
		return;
	}
	
	if (src == NULL) {
		src = vid_in.Get<CvVideoStream>();
		if (!src) return;
	}
	
	// Copy source image
	buffer = src->GetBuffer().clone();
	
	// Skip if buffer is empty
	if (!buffer.rows || !buffer.cols) return;
	
	for(int i = 0; i < graphs.GetCount(); i++) {
		const Graphic& g = graphs[i];
		const Color& c = g.clr1;
		if (g.type == Graphic::LINE) {
			const Point& p0 = g.pts[0];
			const Point& p1 = g.pts[1];
			cv::Point a(p0.x, p0.y);
			cv::Point b(p1.x, p1.y);
			cv::line(buffer, a, b, CV_RGB(c.GetR(), c.GetG(), c.GetB()), g.arg1);
		}
		else if (g.type == Graphic::RECT) {
			const Point& p0 = g.pts[0];
			const Point& p1 = g.pts[1];
			cv::Point a(p0.x, p0.y);
			cv::Point b(p1.x, p1.y);
			cv::rectangle(buffer, a, b, CV_RGB(c.GetR(), c.GetG(), c.GetB()),  CV_FILLED);
		}
		else if (g.type == Graphic::POLYGON) {
			cv::Point small_buf[10];
			cv::Point* reserved = 0;
			cv::Point* buf = 0;
			if (g.pts.GetCount() <= 10) {
				buf = small_buf;
			} else {
				reserved = (cv::Point*)MemoryAlloc(sizeof(cv::Point) * g.pts.GetCount());
				buf = reserved;
			}
			cv::Point* iter = buf;
			for(int j = 0; j < g.pts.GetCount(); j++) {
				const Point& p = g.pts[j];
				iter->x = p.x;
				iter->y = p.y;
				iter++;
			}
			int npts = g.pts.GetCount();
			fillPoly(buffer, (const cv::Point**)&buf, &npts, 1, CV_RGB(c.GetR(), c.GetG(), c.GetB()));
			if (reserved)
				MemoryFree(reserved);
		}
		else if (g.type == Graphic::POLYLINE) {
			cv::Point a, b;
			CvScalar clr = CV_RGB(c.GetR(), c.GetG(), c.GetB());
			for(int j = -1; j < g.pts.GetCount(); j++) {
				const Point* p0 = &g.pts[j == -1 ? g.pts.GetCount()-1 : j];
				const Point* p1 = &g.pts[j == g.pts.GetCount()-1 ? 0 : j+1];
				a.x = p0->x;
				a.y = p0->y;
				b.x = p1->x;
				b.y = p1->y;
				cv::line(buffer, a, b, clr, g.arg1);
			}
		}
		else if (g.type == Graphic::TEXT) {
			const String& txt = *(String*)g.value;
			const Point& p = g.pts[0];
			int font_face = cv::FONT_HERSHEY_SIMPLEX;
			// TODO: find real scale and fix also y offset
			double font_scale = (double)g.arg1 / 32.0;
			int thickness = g.arg2;
			cv::Point tl(p.x, p.y / 2.5);
			cv::putText(buffer, txt.Begin(), tl, font_face, font_scale, CV_RGB(c.GetR(), c.GetG(), c.GetB()), thickness, 8);
		}
		else if (g.type == Graphic::IMAGE) {
			const Image& img = *(Image*)g.value;
			const Point& p = g.pts[0];
			const RGBA* it  = img.Begin();
			const RGBA* end = img.End();
			int x = p.x, y = p.y;
			int w = img.GetWidth();
			int bw = buffer.cols;
			int bh = buffer.rows;
			int col = 0;
			cv::Vec3b color;
			while (it != end) {
				if (it->a != 0 && x >= 0 && x < bw && y >= 0 && y < bh) {
					color[0] = it->b;
					color[1] = it->g;
					color[2] = it->r;
					buffer.at<cv::Vec3b>(cv::Point(x,y)) = color;
				}
				x++;
				col++;
				it++;
				if (col == w || x >= bw) {
					it += w - col;
					x = p.x;
					y++;
					col = 0;
					if (y >= bh)
						break;
				}
			}
		}
	}
}

END_UPP_NAMESPACE

