#ifndef _KandiLab_Game_h_
#define _KandiLab_Game_h_

#include "KandiLab.h"

NAMESPACE_UPP

class StartupDialog : public TopWindow {
	Button play, analyse, showai;
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

class AIOnlyGame : public TopWindow {
	GameCtrl::GameTable table;
	
public:
	typedef AIOnlyGame CLASSNAME;
	AIOnlyGame();
	
	
};

END_UPP_NAMESPACE

#endif
