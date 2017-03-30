#include "AirHockey.h"
#include "VisionCtrl.h"
#include "SurfaceRender.h"
#include "KandiLab.h"
#include "Analyzer.h"

#define IMAGECLASS KandiLabImg
#define IMAGEFILE <KandiLab/KandiLab.iml>
#include <Draw/iml_source.h>

NAMESPACE_UPP

KandiLab::KandiLab() {
	Title("KandiLab");
	Icon(KandiLabImg::icon32());
	
	Add(frame1.SizePos());
	Add(frame2.SizePos());
	Add(frame3.SizePos());
	
	CtrlLayout(frame1);
	CtrlLayout(frame2);
	CtrlLayout(frame3);
	
	frame1.left_bg.Color(Color(238, 250, 255));
	frame2.left_bg.Color(Color(238, 250, 255));
	frame3.left_bg.Color(Color(238, 250, 255));
	
	frame1.title <<= "test";
	frame1.quit <<= THISBACK(Quit);
	frame1.next <<= THISBACK1(Next, 0);
	frame1.rend.WhenLeftDown = THISBACK1(OpenAirHockey, -1);
	frame1.record <<= THISBACK1(OpenAirHockey, 1);
	
	frame2.quit <<= THISBACK(Quit);
	frame2.next <<= THISBACK1(Next, 1);
	frame2.total.Set(0, 1);
	frame2.current.Set(0, 1);
	frame2.monitor.AddColumn("");
	frame2.monitor.AddColumn("");
	
	frame3.quit <<= THISBACK(Quit);
	frame3.results.AddColumn("");
	frame3.results.AddColumn("");
	
	Size sz = Zsz(800, 600);
	SetRect(0,0, sz.cx, sz.cy);
	SetView(0);
}

void KandiLab::SetView(int i) {
	frame1.Hide();
	frame2.Hide();
	frame3.Hide();
	switch (i) {
		case 0: frame1.Show(); break;
		case 1: frame2.Show(); break;
		case 2: frame3.Show(); break;
	}
}

void KandiLab::Next(int from_frame) {
	
	if (from_frame == 0) {
		
		export_dir = ConfigFile((String)frame1.title.GetData());
		if (!DirectoryExists(export_dir)) {
			if (PromptYesNo("Do you want to create directory " + export_dir)) {
				RealizeDirectory(export_dir);
			}
			if (!DirectoryExists(export_dir)) {
				PromptOK("Can't open directory " + export_dir);
				return;
			}
		}
		
		frame1.rend.Stop();
		
		ArAirHockey& ar					= Single<ArAirHockey>();
		ar.vc.Close();
		
		SetView(1);
		
		frame2.next.Disable();
		Thread::Start(THISBACK(Analyze));
	}
	else if (from_frame == 1) {
		SetView(2);
	}
	else if (from_frame == 2) {
		Quit();
	}
}

inline int Area(const Size& sz)
{
	return sz.cx * sz.cy;
}

inline cv::Mat ImageCv(const Image& im) {
	Size obj_sz = im.GetSize();
	cv::Mat img_object(obj_sz.cy, obj_sz.cx, IPL_DEPTH_16U);
	const RGBA* src = im.Begin();
	cv::Vec3b color;
	for(int y = 0; y < obj_sz.cy; y++) {
		for(int x = 0; x < obj_sz.cx; x++) {
			color[0] = src->b;
			color[1] = src->g;
			color[2] = src->r;
			img_object.at<cv::Vec3b>(cv::Point(x, y)) = color;
			src++;
		}
	}
	return img_object;
}

void KandiLab::Analyze() {
	TimeStop ts;
	TimeStop ts_fast;
	
	
	// Get count of phases in this function
	int detector_count = Analyzer::GetDetectorCount();
	int descriptor_count = Analyzer::GetDescriptorCount();
	int slow_total = detector_count + descriptor_count;
	int slow = 0;
	
	// Load object image
	PNGRaster png;
	Image object = png.LoadFile(AppendFileName(export_dir, "object.png"));
	cv::Mat cv_object = ImageCv(object);
	
	if (Area(object.GetSize()) == 0) {
		PromptOK("Unable to analyze: object image loading failed");
		return;
	}
	Monitor("Object image size", AsString(object.GetSize()));
	
	
	// Load video images
	Monitor("Loading images...", "");
	Vector<String> image_files;
	FindFile ff(AppendFileName(export_dir, "*.png"));
	while (ff) {
		if(ff.IsFile()) {
			if (ff.GetName() != "object.png")
				image_files.Add(ff.GetPath());
		}
		ff.Next();
	}
	if (image_files.IsEmpty()) {
		Monitor("Error", "No images found");
		return;
	}
	Monitor("Found " + IntStr(image_files.GetCount()) + " images", "");
	Sort(image_files, StdLess<String>());
	
	
	// Convert images to OpenCV format
	Vector<Image> images;
	Array<cv::Mat> cv_images;
	for(int i = 0; i < image_files.GetCount(); i++) {
		images.Add(png.LoadFile(image_files[i]));
		cv_images.Add(ImageCv(images[i]));
		Progress(0, slow_total, i, image_files.GetCount());
	}
	Monitor("Elapsed time", ts.ToString());
	Monitor("Image size", AsString(images[0].GetSize()));
	Monitor("Image count", AsString(image_files.GetCount()));
	FileOut log_results(ConfigFile("results.txt"));
	
	
	// Test all detectors
	{
		Vector<Vector<double> > img_times;
		for(int i = 0; i < detector_count; i++)
			img_times.Add().SetCount(images.GetCount(), 0);
		Analyzer an;
		VectorMap<String, Vector<double> > values;
		Vector<String> titles;
		an.SetTest(0);
		for(int i = 0; i < detector_count; i++) {
			Vector<double>& detector_times = img_times[i];
			an.Start(i, image_files.GetCount());
			Monitor("Running benchmark: " + an.GetTitle(), "");
			for(int j = 0; j < images.GetCount(); j++) {
				const cv::Mat& img = cv_images[j];
				ts_fast.Reset();
				an.ProcessDetectorTest(img, j);
				detector_times[j] = ts_fast.Elapsed();
				Progress(slow, slow_total, j, images.GetCount());
			}
			slow++;
			an.Stop();
			titles.Add(an.GetTitle());
			values.GetAdd("Average time").Add(an.GetAverageTime());
			values.GetAdd("Speedup").Add(0);
			values.GetAdd("Average keypoints").Add(an.GetAverageKeypoints());
			Monitor("Elapsed time", an.GetTS().ToString());
		}
		
		// Calculate Speed-up value
		Vector<double>& times = values.GetAdd("Average time");
		Vector<double>& speedups = values.GetAdd("Speedup");
		int max_time_pos = -1;
		int max_time = 0;
		for(int i = 0; i < times.GetCount(); i++) {
			if (times[i] > max_time) {
				max_time = times[i];
				max_time_pos = i;
			}
		}
		for(int i = 0; i < times.GetCount(); i++)
			speedups[i] = (double)max_time / times[i];
		
		// Print results
		GuiLock __;
		String line;
		for(int i = 0; i < values.GetCount(); i++) {
			String key = values.GetKey(i);
			frame3.results.Add(key + " values", "");
			log_results << key << " values\n";
			
			Vector<double>& vec = values[i];
			for(int j = 0; j < titles.GetCount(); j++) {
				frame3.results.Add(key + " " + titles[j], DblStr(vec[j]));
				log_results << "\t" << titles[j] << "\t" << DblStr(vec[j]) << "\n";
			}
		}
		log_results.Flush();
		
		// Print CSV
		FileOut csv(ConfigFile("detector.csv"));
		for(int i = 0; i < image_files.GetCount(); i++) {
			for(int j = 0; j < detector_count; j++) {
				if (j) csv << ",";
				csv << img_times[j][i];
			}
			csv << "\n";
		}
	}
	
	
	// Test all descriptors
	{
		Vector<Vector<double> > img_times;
		for(int i = 0; i < descriptor_count; i++)
			img_times.Add().SetCount(images.GetCount(), 0);
		Analyzer an;
		VectorMap<String, Vector<double> > values;
		Vector<String> titles;
		an.SetTest(1);
		//an.SetObject(cv_object);
		Monitor("Pre-calculating keypoints", "");
		an.CalculateKeypoints(cv_images);
		for(int i = 0; i < descriptor_count; i++) {
			Vector<double>& descriptor_times = img_times[i];
			an.Start(i, image_files.GetCount());
			Monitor("Running benchmark: " + an.GetTitle(), "");
			for(int j = 0; j < images.GetCount(); j++) {
				const cv::Mat& img = cv_images[j];
				ts_fast.Reset();
				an.ProcessDescriptorTest(img, j);
				descriptor_times[j] = ts_fast.Elapsed();
				Progress(slow, slow_total, j, images.GetCount());
			}
			slow++;
			an.Stop();
			titles.Add(an.GetTitle());
			values.GetAdd("Average time").Add(an.GetAverageTime());
			values.GetAdd("Speedup").Add(0);
			Monitor("Elapsed time", an.GetTS().ToString());
		}
		
		// Calculate Speed-up value
		Vector<double>& times = values.GetAdd("Average time");
		Vector<double>& speedups = values.GetAdd("Speedup");
		int max_time_pos = -1;
		int max_time = 0;
		for(int i = 0; i < times.GetCount(); i++) {
			if (times[i] > max_time) {
				max_time = times[i];
				max_time_pos = i;
			}
		}
		for(int i = 0; i < times.GetCount(); i++)
			speedups[i] = (double)max_time / times[i];
		
		// Print results
		GuiLock __;
		String line;
		for(int i = 0; i < values.GetCount(); i++) {
			String key = values.GetKey(i);
			frame3.results.Add(key + " values", "");
			log_results << key << " values\n";
			
			Vector<double>& vec = values[i];
			for(int j = 0; j < titles.GetCount(); j++) {
				frame3.results.Add(key + " " + titles[j], DblStr(vec[j]));
				log_results << "\t" << titles[j] << "\t" << DblStr(vec[j]) << "\n";
			}
		}
		log_results.Flush();
		
		// Print CSV
		FileOut csv(ConfigFile("descriptor.csv"));
		for(int i = 0; i < image_files.GetCount(); i++) {
			for(int j = 0; j < detector_count; j++) {
				if (j) csv << ",";
				csv << img_times[j][i];
			}
			csv << "\n";
		}
	}
	
	
	
	
	
	
	/*String table;
	for (int bm = 0; bm < benchmark_count; bm++) {
		Analyzer an;
		an.Start(bm, image_files.GetCount());
		Monitor("Running benchmark: " + an.GetTitle(), "");
		
		an.SetObject(cv_object);
		
		for(int i = 0; i < images.GetCount(); i++) {
			const cv::Mat& img = cv_images[i];
			an.Process(img, i);
			
			Progress(bm+1, slow_total, i, images.GetCount());
		}
		
		an.Stop();
		Monitor("Elapsed time", an.GetTS().ToString());
		
		GuiLock __;
		String line;
		for(int i = 0; i < an.GetResultCount(); i++) {
			String key, value;
			an.GetResult(i, key, value);
			frame3.results.Add(key, value);
			
			log_results << key << "\t" << value << "\n";
			table << value << "\t";
		}
		table << "\n";
		
		log_results.Flush();
	}
	log_results << table << "\n";*/
	
	Progress(1,1,1,1);
	
	GuiLock __;
	frame2.next.Enable();
	
}

void KandiLab::Monitor(String key, String value) {
	GuiLock __;
	frame2.monitor.Add(key, value);
	frame2.monitor.ScrollEnd();
}

void KandiLab::Progress(int slow_actual, int slow_total, int fast_actual, int fast_total) {
	GuiLock __;
	frame2.total.Set(slow_actual, slow_total);
	frame2.current.Set(fast_actual, fast_total);
}

void KandiLab::OpenAirHockey(int record) {
	static int state = 0;
	static int recording = 0;
	
	if (record) {
		GuiLock __;
		frame1.record.Disable();
		frame1.title.Disable();
		//frame1.play.Disable();
		frame1.record.SetLabel("Recording. Press Next or Quit to stop.");
		recording = record;
		
		export_dir = ConfigFile((String)frame1.title.GetData());
		RealizeDirectory(export_dir);
	}
	else if (!record) {
		GuiLock __;
		frame1.record.Disable();
		frame1.title.Disable();
		frame1.next.Disable();
		recording = record;
	}
	else if (record == -1) {
		
	}
	
	SurfaceRenderer& surf				= frame1.rend;
	
	ArAirHockey& ar						= Single<ArAirHockey>();
	
	VideoCapture& vc					= ar.vc;
	CvVideoDraw& vd						= ar.vd;
	CvVideoMixer& mix					= ar.mix;
	CvImageMatchStream& im				= ar.im;
	CvColorBlobDetector& cd				= ar.cd;
	GameStream& gs						= ar.gs;
	GameCtrl::GameTable& table	= ar.table;
	
	Engine& eng						= surf.eng;
	
	frame1.rend.Lock();
	
	int cam_w = 320;
	int cam_h = 240;
	int w = 512;
	int h = w * 3 / 5;
	
	if (state == 0) {
		surf.Start();
		
		if (!vc.GetCount()) {
			PromptOK("No cameras found! Exiting...");
			Exit(1);
		}
		vc.Open(0);
		vc.SetSize(Size(cam_w, cam_h));
		
		vd.SetIn(0, vc.GetOut(0));
		
		mix.Add(vd, RectC(0, 0, 320, 240));
		mix.Add(gs, Point(0,0), Point(1,0), Point(1,1), Point(0,1));
		mix.Add(im, RectC(320, 0, 320, 240));
		mix.Add(cd, RectC(0, 240, 320, 240));
		mix.Add(gs, RectC(320, 240, 320, 240));
		mix.SetSize(Size(640, 480));
		
		// Export directory
		if (!CommandLine().IsEmpty() && DirectoryExists(CommandLine()[0])) {
			mix.SetExportDirectory(CommandLine()[0]);
		}
		
		im.SetIn(0, vc.GetOut(0));
		im.SetMixer(mix, 0);
		
		cd.SetIn(0, im.GetOut(0));
		
		gs.SetSize(Size(w, h));
		gs.SetGame(&table);
		gs.SetPlayer(cd);
		
		table.Init();
		
		surf.SetStream(mix);
		
		eng.Add(vc);
		eng.Add(vd);
		eng.Add(im);
		eng.Add(cd);
		eng.Add(gs);
		eng.Add(mix);
	}
	
	
	vd.Clear();
	
	// Game area at screen
	int max_w = cam_w*0.8;
	h = max_w * h / w;
	w = max_w;
	int x = cam_w/2 - w/2;
	int y = cam_h/2 - h/2;
	Point tl(x, y), br(x+w, y+h);
	
	if (state == 2 || state == 0) {
		Vector<Point> pt;
		pt << tl << Point(br.x, tl.y) << br << Point(tl.x, br.y) << tl;
		vd.Add().SetPolyline(pt, 3, Yellow());
		
		String s = "Select game area and press screen";
		vd.Add().SetText(s, 12, Point(20, x+1), Black());
		vd.Add().SetText(s, 12, Point(21, x), White());
		vd.Add().SetImage(CtrlImg::save(), Point(0, 0));
		
		state = 1;
	}
	else if (state == 1) {
		
		// Get player area
		Image object = vc.GetImage(Rect(tl.x, tl.y, br.x, br.y));
		im.SetObject(object);
		
		// Export source image for analyzing
		if (recording) {
			PNGEncoder png;
			png.SaveFile(AppendFileName(export_dir, "object.png"), object);
			
			vc.SetExportDirectory(export_dir);
		}
		
		state = 2;
	}
	
	frame1.rend.Unlock();
	
}

END_UPP_NAMESPACE
