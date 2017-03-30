#ifndef _SurfaceDesign_SurfaceRender_h_
#define _SurfaceDesign_SurfaceRender_h_


#include "Common.h"
#include "MediaStream.h"

using namespace Upp;
#include <SDL/SDLCtrl.h>

#include "GameStream.h"

NAMESPACE_UPP

class SurfaceRenderer : public SDLCtrl {
	
protected:
	SDLSurface surf;
	Ptr<CvVideoStream> cv_stream;
	bool running;
	bool initialized;
	Thread thrd;
	bool zoom;
	SpinLock lock;
	
	void Run();
public:
	typedef SurfaceRenderer CLASSNAME;
	SurfaceRenderer();
	~SurfaceRenderer();
	
	virtual void Lock() {lock.Enter();}
	virtual void Unlock() {lock.Leave();}
	
	void SetStream(CvVideoStream& vs) {cv_stream = &vs;}
	
	virtual void Start() {thrd.Run(THISBACK(Run));}
	virtual void Stop();
	virtual Ctrl* GetCtrl() {return this;}
	virtual void Layout();
	void SetupPalette(SDL_Surface * screen);
	
	virtual void LeftDown(Point p, dword keyflags) {WhenLeftDown();}

	Callback WhenLeftDown;
	
	Engine eng;
	
};

END_UPP_NAMESPACE

#endif
