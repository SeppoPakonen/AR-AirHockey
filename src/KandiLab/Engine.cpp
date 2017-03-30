#include "Engine.h"

NAMESPACE_UPP

Engine::Engine() {
	
}
	
Engine& Engine::Add(Module& module) {
	process_order.Add(&module);
	
	return *this;
}

Engine& Engine::Process(int ms) {
	for(int i = 0; i < process_order.GetCount(); i++) {
		Ptr<Module>& mod = process_order[i];
		if (!mod) continue;
		mod->Process(ms);
	}
	
	return *this;
}


END_UPP_NAMESPACE
