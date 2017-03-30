#include "Game.h"


NAMESPACE_UPP

StartupDialog::StartupDialog() {
	action = 0;
	
	Title("KandiLab");
	Icon(KandiLabImg::icon32());
	
	SetRect(0,0, 650, 330);
	Add(banner.VSizePos(0, 30).HSizePos());
	Add(play.BottomPos(2, 26).HCenterPos(100, -51));
	Add(analyse.BottomPos(2, 26).HCenterPos(100, +51));
	
	play.SetLabel("Play");
	play <<= THISBACK1(SetAction, 1);
	
	analyse.SetLabel("Analyse");
	analyse <<= THISBACK1(SetAction, 2);
	
	banner.SetImage(KandiLabImg::airhockey_banner());
}












Game::Game() {
	Title("AR AirHockey");
	Icon(KandiLabImg::icon32());
	
	Add(surf.SizePos());
	surf.WhenLeftDown = THISBACK(OpenAirHockey);
	
	SetRect(0,0,320,240);
	
	PostCallback(THISBACK(OpenAirHockey));
}

void Game::OpenAirHockey() {
	static int state = 0;
	
	ArAirHockey& ar						= Single<ArAirHockey>();
	
	VideoCapture& vc					= ar.vc;
	CvVideoDraw& vd						= ar.vd;
	CvVideoMixer& mix					= ar.mix;
	CvImageMatchStream& im				= ar.im;
	CvColorBlobDetector& cd				= ar.cd;
	GameStream& gs						= ar.gs;
	GameCtrl::GameTable& table	= ar.table;
	
	Engine& eng							= surf.eng;
	
	surf.Lock();
	
	int cam_w = 320;
	int cam_h = 240;
	int w = 512;
	int h = w * 3 / 5;
	
	if (state == 0) {
		surf.Start();
		
		if (!vc.GetCount()) {
			PromptOK("No cameras found!");
			Exit(1);
		}
		vc.Open(0);
		vc.SetSize(Size(cam_w, cam_h));
		
		vd.SetIn(0, vc.GetOut(0));
		
		mix.Add(vd, RectC(0, 0, 320, 240));
		mix.Add(gs, Point(0,0), Point(1,0), Point(1,1), Point(0,1));
		mix.SetSize(Size(320, 240));
		
		im.SetIn(0, vc.GetOut(0));
		im.SetMixer(mix, 0);
		
		cd.SetIn(0, im.GetOut(0));
		
		gs.SetSize(Size(w, h));
		gs.SetGame(&table);
		gs.SetPlayer(cd);
		
		table.Init();
		
		surf.SetStream(mix);
		
		eng.Add(vc);
		eng.Add(vd);
		eng.Add(im);
		eng.Add(cd);
		eng.Add(gs);
		eng.Add(mix);
	}
	
	
	vd.Clear();
	
	// Game area at screen
	int max_w = cam_w*0.8;
	h = max_w * h / w;
	w = max_w;
	int x = cam_w/2 - w/2;
	int y = cam_h/2 - h/2;
	Point tl(x, y), br(x+w, y+h);
	
	if (state == 2 || state == 0) {
		Vector<Point> pt;
		pt << tl << Point(br.x, tl.y) << br << Point(tl.x, br.y) << tl;
		vd.Add().SetPolyline(pt, 3, Yellow());
		
		String s = "Select game area and press screen";
		vd.Add().SetText(s, 12, Point(20, x+1), Black());
		vd.Add().SetText(s, 12, Point(21, x), White());
		vd.Add().SetImage(CtrlImg::save(), Point(0, 0));
		
		state = 1;
	}
	else if (state == 1) {
		
		// Get player area
		Image object = vc.GetImage(Rect(tl.x, tl.y, br.x, br.y));
		im.SetObject(object);
		
		state = 2;
	}
	
	surf.Unlock();
}


END_UPP_NAMESPACE
