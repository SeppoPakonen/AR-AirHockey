#ifndef _ModularCore_Jack_h_
#define _ModularCore_Jack_h_

#include <Core/Core.h>

NAMESPACE_UPP

class Module;

class Jack : Moveable<Jack> {
	int type, index;
	Ptr<Module> mod;
	
public:
	Jack() {}
	Jack(const Jack& j) : mod(j.mod) {}
	Jack(int type, Module* m, int index) : type(type), index(index), mod(m) {};
	
	bool IsEmpty() const {return !mod;}
	
	template <class T>
	T* Get() const {if (!mod) return 0; return dynamic_cast<T*>(&*mod);}
};

END_UPP_NAMESPACE

#endif
