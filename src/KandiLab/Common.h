#ifndef _VisionCtrl_Common_h_
#define _VisionCtrl_Common_h_

#undef CPU_SSE2

#include <opencv2/opencv.hpp>
#ifdef flagNONFREE
	#include <opencv2/nonfree/features2d.hpp>
#endif


#include <plugin/box2d/Box2D.h>
#include <CtrlLib/CtrlLib.h>

#endif
