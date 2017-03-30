#include "SurfaceRender.h"

NAMESPACE_UPP

SurfaceRenderer::SurfaceRenderer() {
	running = false;
	initialized = false;
	zoom = false;
	
	SetBpp(24);
	SetVideoFlags(GetVideoFlags() | SDL_HWPALETTE);
}

SurfaceRenderer::~SurfaceRenderer() {
	if (running) Stop();
}

void SurfaceRenderer::Stop() {
	running = false;
	lock.Enter();
	SDL_Quit();
	lock.Leave();
	thrd.Wait();
}

void SurfaceRenderer::SetupPalette(SDL_Surface *screen) {
	SDL_Color pal[256];

	double d = 20;
	double dx = 63.0 / 256.0;

	for(int i = 0; i < 254; i++) {
		pal[i].r = (Uint8) d;
		pal[i].g = (Uint8) d;
		pal[i].b = 0;

		d += dx;
	}
	pal[254].r = 0;
	pal[254].g = 150;
	pal[254].b = 255;

	pal[255].r = 255;
	pal[255].g = 255;
	pal[255].b = 0;

	SDL_SetColors(screen, pal, 0, 256);
	SDL_Flip(screen);
	SDL_SetColors(screen, pal, 0, 256);
}

void SurfaceRenderer::Layout() {
	lock.Enter();
	
	SDLCtrl::Layout();
	
	if (!IsReady()) {
		lock.Leave();
		return;
	}
	
	if (!initialized) {
		lock.Leave();
		return;
	}
	
	int width = GetWidth();
	int height = GetHeight();
	
	
	surf.Resize(width, height);
	
	SetupPalette(surf.GetSurface());
	SetupPalette(GetSurface());
	
	lock.Leave();
}

void SurfaceRenderer::Run() {
	
	// Wait until SDL_Surface has been created
	running = true;
	while (!GetSurface() && running) Sleep(100);
	if (!running) return;
	
	SDLCtrl::Layout();
	
	Rect r = GetRect();
	int width = r.GetWidth();
	int height = r.GetHeight();

	if (!surf.CreateRGB(width, height, GetBpp()))
		return;

	SetupPalette(surf.GetSurface());
	SetupPalette(GetSurface());
	
	initialized = true;
	
	SDL_Event event;
	
	TimeStop ts;
	
	while (running) {
		if(SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == 1)
					LeftDown(Point(r.left, r.top), 0);
				else if (event.button.button == 2)
					MiddleDown(Point(r.left, r.top), 0);
				else if (event.button.button == 3)
					RightDown(Point(r.left, r.top), 0);
				break;
			case SDL_MOUSEMOTION:
				r = GetScreenView();
				MouseMove(Point(r.left+event.motion.x, r.top+event.motion.y), 0);
				break;
			case SDL_QUIT:
				running = false;
				break;
			default:
				break;
			}
		}
		else {
			r = GetRect();
			width = GetWidth();
			height = GetHeight();
		
			lock.Enter();
			surf.Lock();
			
			eng.Process(ts.Elapsed());
			ts.Reset();
			
			if (cv_stream) {
				const cv::Mat& buf = cv_stream->GetBuffer();
				if (buf.rows > 0 && buf.cols > 0) {
					Color c;
					if (zoom) {
						double dst_factor = (double)height / width;
						double src_factor = (double)buf.rows / buf.cols;
						int w, h, xoff = 0, yoff = 0;
						// If destination is narrower, then use width limit
						if (dst_factor > src_factor) {
							w = width;
							h = src_factor * w;
							yoff = (height - h) / 2;
						}
						// Else source is wider, and using height limit
						else {
							h = height;
							w = h / src_factor;
							xoff = (width - w) / 2;
						}
						for (int y = 0; y < h; y++) {
							int src_y = y * buf.rows / h;
							for (int x = 0; x < w; x++) {
								int src_x = x * buf.cols / w;
								cv::Vec3b pixel = buf.at<cv::Vec3b>(src_y, src_x);
								surf.DrawPixel(xoff + x, yoff + y, Color(pixel[2], pixel[1], pixel[0]));
							}
						}
					} else {
						for(int i=0; i < buf.rows; i++) {
							for(int j=0; j < buf.cols; j++) {
								cv::Vec3b pixel = buf.at<cv::Vec3b>(i,j);
								surf.DrawPixel(j, i, Color(pixel[2], pixel[1], pixel[0]));
							}
						}
					}
				}
			}
			
			surf.Unlock();
			Blit(surf);
			
			lock.Leave();
		}
	}
}

END_UPP_NAMESPACE
