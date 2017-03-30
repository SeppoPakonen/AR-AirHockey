#ifndef _KandiLab_KandiLab_h
#define _KandiLab_KandiLab_h

#include "VisionCtrl.h"
#include "SurfaceRender.h"

#define LAYOUTFILE <KandiLab/KandiLab.lay>
#include <CtrlCore/lay.h>

#define IMAGECLASS KandiLabImg
#define IMAGEFILE <KandiLab/KandiLab.iml>
#include <Draw/iml_header.h>


NAMESPACE_UPP

struct ArAirHockey {
	VideoCapture vc;
	CvVideoDraw vd;
	CvVideoMixer mix;
	CvImageMatchStream im;
	CvColorBlobDetector cd;
	GameStream gs;
	GameCtrl::GameTable table;
};

class KandiLab : public TopWindow {
	
	With_1_GameRecord<ParentCtrl> frame1;
	With_2_Benchmark<ParentCtrl> frame2;
	With_3_Results<ParentCtrl> frame3;
	String export_dir;
	
public:
	typedef KandiLab CLASSNAME;
	KandiLab();
	
	void SetView(int i);
	void OpenAirHockey(int record);
	void Analyze();
	void Next(int from_frame);
	void Quit() {PostCallback(THISBACK(Close0));}
	void Close0() {Close();}
	void Progress(int slow_actual, int slow_total, int fast_actual, int fast_total);
	void Monitor(String key, String value);
	
};

END_UPP_NAMESPACE

#endif
