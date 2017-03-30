#ifndef _VisionCtrl_VideoDraw_h_
#define _VisionCtrl_VideoDraw_h_

#include "Common.h"

NAMESPACE_UPP

class CvVideoDraw : public CvVideoStream {
	CvVideoStream* src;
	
public:
	
	struct Graphic : Moveable<Graphic> {
		int type;
		int arg1, arg2;
		Color clr1;
		Vector<Point> pts;
		void* value;
		
		Graphic() : value(0) {}
		~Graphic() {if (value) Clear();}
		void Clear() {
			if (type == TEXT)  delete (String*)value;
			if (type == IMAGE) delete (Image*)value;
			value = NULL;
			type = -1;
		}
		enum {LINE, RECT, POLYGON, POLYLINE, TEXT, IMAGE};
		Graphic& SetLine(Point a, Point b, Color clr = White(), int w=1) {Clear(); type = LINE; pts.SetCount(2); pts[0] = a; pts[1] = b; clr1 = clr; arg1 = w; return *this;}
		Graphic& SetRect(Point tl, Point br, Color clr = White()) {Clear(); type = RECT; pts.SetCount(2); pts[0] = tl; pts[1] = br; clr1 = clr; return *this;}
		Graphic& SetPolygon(const Vector<Point>& vec, Color clr = White()) {Clear(); type = POLYGON; pts.SetCount(2); pts <<= vec; clr1 = clr; return *this;}
		Graphic& SetPolyline(const Vector<Point>& vec, int w=1, Color clr = Black()) {Clear(); type = POLYLINE; pts.SetCount(2); pts <<= vec; clr1 = clr; arg1 = w; return *this;}
		Graphic& SetText(String txt, int height, Point tl, Color clr=Black(), int thickness=1) {Clear(); type = TEXT;  arg1 = height; arg2 = thickness; pts.SetCount(1); pts[0] = tl; clr1 = clr; value = new String(txt); return *this;}
		Graphic& SetImage(const Image& img, Point tl) {Clear(); type = IMAGE; pts.SetCount(1); pts[0] = tl; value = new Image(img); return *this;}
		
	};
	
	CvVideoDraw();
	
	Graphic& Add() {return graphs.Add();}
	void Clear() {graphs.Clear();}
	
	int GetCount() const {return graphs.GetCount();}
	Graphic& Get(int i) {return graphs[i];}
	
	virtual void Process(int ms);
	
	
protected:
	Vector<Graphic> graphs;
	
};

END_UPP_NAMESPACE

#endif
