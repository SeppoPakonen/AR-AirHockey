#ifndef _ModularCore_Engine_h_
#define _ModularCore_Engine_h_

#include "Module.h"

NAMESPACE_UPP

class Engine {
	Vector<Ptr<Module> > process_order;
	
public:
	Engine();
	
	Engine& Add(Module& module);
	Engine& Process(int ms);
	
};

END_UPP_NAMESPACE

#endif
