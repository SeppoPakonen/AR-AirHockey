#ifndef _ModularCore_Module_h_
#define _ModularCore_Module_h_

#include "Jack.h"

NAMESPACE_UPP

class Module : public Pte<Module> {
	
protected:
	Vector<Ptr<Module> > sources;
	Vector<Jack> in, out;
	
public:
	Module();
	
	virtual void Process(int ms) {}
	
	Jack GetIn(int i);
	Jack GetOut(int i);
	Module& SetIn(int i, Jack out);
	Module& SetOut(int i, Jack in);
	Module& SetInCount(int count) {in.SetCount(count); return *this;}
	Module& SetOutCount(int count) {out.SetCount(count); return *this;}
	
};

END_UPP_NAMESPACE

#endif
