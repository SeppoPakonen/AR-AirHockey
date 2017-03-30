#include "Engine.h"

NAMESPACE_UPP

Module::Module() {
	
}

Jack Module::GetIn(int i) {
	return Jack(0, this, i);
}

Jack Module::GetOut(int i) {
	return Jack(1, this, i);
}

Module& Module::SetIn(int i, Jack out) {
	in[i] = out;
	return *this;
}

Module& Module::SetOut(int i, Jack in) {
	out[i] = in;
	return *this;
}



END_UPP_NAMESPACE
