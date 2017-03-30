#include "SurfaceRender.h"
#include <Painter/Painter.h>

NAMESPACE_UPP

GameStream::GameStream() {
	game = NULL;
	sz = Size(320, 240);
}

void GameStream::Process(int ms) {
	if (!game) return;
	
	// NOTE: airhockey is vertical while this must export horizontal
	if (buffer.cols != sz.cy || buffer.rows != sz.cx) {
		buffer = cv::Mat(sz.cy, sz.cx, IPL_DEPTH_16U);
	}
	
	// Get player point
	if (player) {
		Pointf ply = player->GetLast();
		DUMP(ply);
		Swap(ply.x, ply.y);
		game->SetPlayer(ply);
	}
	
	// Redraw game
	ImageBuffer ib(Size(sz.cy, sz.cx));
	BufferPainter bp(ib, MODE_NOAA);
	game->SetRect(0, 0, sz.cy, sz.cx);
	game->Paint(bp);
	RGBA* src = ib.Begin();
	cv::Vec3b color;
	for(int x = 0; x < sz.cx; x++) {
		for(int y = sz.cy-1; y >= 0; y--) {
			color[2] = src->r;
			color[1] = src->g;
			color[0] = src->b;
			buffer.at<cv::Vec3b>(cv::Point(x,y)) = color;
			src++;
		}
	}
	
}

END_UPP_NAMESPACE
