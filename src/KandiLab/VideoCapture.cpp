#include "VisionCtrl.h"

NAMESPACE_UPP

VideoCapture::VideoCapture() {
	id = -1;
	target_sz = Size(0,0);
}

VideoCapture& VideoCapture::SetExportDirectory(String path) {
	frame_counter = 0;
	export_path = path;
	return *this;
}

void VideoCapture::SetSize(Size sz) {
	if (id == -1) return;
	if (cap.isOpened())
		cap.release();
	cap.open(id);
	cap.set(CV_CAP_PROP_FRAME_WIDTH,	sz.cx);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT,	sz.cy);
	target_sz = sz;
}

int VideoCapture::GetCount() {
	cv::VideoCapture temp_camera;
	int max_tested = 10;
	for (int i = 0; i < max_tested; i++){
		cv::VideoCapture temp_camera(i);
		bool res = (!temp_camera.isOpened());
		temp_camera.release();
		if (res)
			return i;
	}
	return max_tested;
}

void VideoCapture::Process(int ms) {
	cap.read(buffer);
	if (buffer.rows == 0)
		return;
	else if ((target_sz.cx != 0 && buffer.cols != target_sz.cx) ||
		(target_sz.cy != 0 && buffer.rows != target_sz.cy)) {
		cv::resize(buffer, buffer, cv::Size(target_sz.cx, target_sz.cy), 0, 0, cv::INTER_NEAREST);
	}
	
	
	if (!export_path.IsEmpty()) {
		String file = export_path + DIR_SEPS + Format("%05d.png", frame_counter);
		try {
			imwrite(file.Begin(), buffer);
		}
		catch (...) {
			
		}
	}
	
	frame_counter++;
}


END_UPP_NAMESPACE
