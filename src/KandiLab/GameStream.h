#ifndef _SurfaceDesign_GameStream_h_
#define _SurfaceDesign_GameStream_h_

//#include <plugin/box2d/Box2D.h>
#include "AirHockey.h"
#include "MediaStream.h"
#include "VisionCtrl.h"

NAMESPACE_UPP

class GameStream : public CvVideoStream {
	CvColorBlobDetector* player;
	GameCtrl::GameTable* game;
	Size sz;
public:
	GameStream();
	
	void SetGame(GameCtrl::GameTable* game) {this->game = game;}
	void SetSize(Size sz) {this->sz = sz;}
	void SetPlayer(CvColorBlobDetector& p) {player = &p;}
	
	virtual void Process(int ms);
	
};

END_UPP_NAMESPACE

#endif
