#ifndef _MediaStream_VideoMixer_h_
#define _MediaStream_VideoMixer_h_

#include <Core/Core.h>
#include <Draw/Draw.h>
#include "Engine.h"

NAMESPACE_UPP

class VideoMixer : public Module {
	
protected:
	
public:
	VideoMixer();
	
};

class CvVideoMixer : public CvVideoStream {
	
protected:
	typedef Tuple4<Point, Point, Point, Point> Polygon;
	typedef Tuple2<CvVideoStream*, Polygon>	PolygonSource;
	typedef Tuple2<CvVideoStream*, Rect> RectSource;
	
	Vector<PolygonSource> poly_sources;
	Vector<RectSource> rect_sources;
	CvVideoStream* src;
	int frame_counter;
	String export_path;
	
public:
	CvVideoMixer();
	
	virtual void Process(int ms);
	
	CvVideoMixer& Add(CvVideoStream& s, Rect area);
	CvVideoMixer& Add(CvVideoStream& s, Point tl, Point tr, Point br, Point bl);
	CvVideoMixer& Set(int i, Point tl, Point tr, Point br, Point bl);
	CvVideoMixer& SetSize(Size sz);
	CvVideoMixer& SetExportDirectory(String path);
	
};


END_UPP_NAMESPACE

#endif
