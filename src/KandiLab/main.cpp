#include "Game.h"

using namespace Upp;

GUI_APP_MAIN {
	StartupDialog dlg;
	dlg.Run();
	
	if (!dlg.action) return;
	
	if (dlg.action == 1) {
		Game().Run();
	}
	else if (dlg.action == 3) {
		AIOnlyGame().Run();
	}
	else {
		KandiLab().Run();
	}
	
	Thread::ShutdownThreads();
}
