#include "Game.h"

using namespace Upp;

GUI_APP_MAIN {
	StartupDialog dlg;
	dlg.Run();
	
	if (!dlg.action) return;
	
	if (dlg.action == 1) {
		Game().Run();
	}
	else {
		KandiLab().Run();
	}
	
	Thread::ShutdownThreads();
}
