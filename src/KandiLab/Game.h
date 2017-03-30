#ifndef _KandiLab_Game_h_
#define _KandiLab_Game_h_

#include "KandiLab.h"

NAMESPACE_UPP

class StartupDialog : public TopWindow {
	Button play, analyse;
	ImageCtrl banner;
	
public:
	typedef StartupDialog CLASSNAME;
	StartupDialog();
	
	int action;
	
	void Close0() {Close();}
	void SetAction(int i) {action = i; PostCallback(THISBACK(Close));}
	
};

class Game : public TopWindow {
	SurfaceRenderer surf;
	
public:
	typedef Game CLASSNAME;
	Game();
	
	void OpenAirHockey();
	
};

END_UPP_NAMESPACE

#endif
