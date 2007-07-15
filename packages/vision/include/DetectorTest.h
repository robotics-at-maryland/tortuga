#ifndef RAM_DETECTOR_TEST_H_07_11_2007
#define RAM_DETECTOR_TEST_H_07_11_2007

#include "core/include/Updatable.h"
#include "vision/include/OpenCVCamera.h"
#include "vision/include/OrangePipeDetector.h"
#include "vision/include/GateDetector.h"
#include "vision/include/RedLightDetector.h"
#include "vision/include/BinDetector.h"
#include "vision/include/Recorder.h"

namespace ram{
	namespace vision{
	
	class DetectorTest : public core::Updatable
	{
		public:
			DetectorTest(int, bool);
			DetectorTest(std::string);
			~DetectorTest();
			bool orangeOn;
			bool lightOn;
			bool gateOn;
			bool binOn;
			ram::vision::OpenCVCamera* camera;
			void orangeDetectOn();
			void lightDetectOn();
			void gateDetectOn();
			void binDetectOn();
			void orangeDetectOff();
			void lightDetectOff();
			void gateDetectOff();
			void binDetectOff();
			void update(double);
			OrangePipeDetector* getOrangeDetector();
			GateDetector* getGateDetector();
			BinDetector* getBinDetector();
			RedLightDetector* getRedDetector();
		private:
			OpenCVImage* frame;
			OrangePipeDetector* opDetect;
			GateDetector* gDetect;
			BinDetector* bDetect;
			RedLightDetector* rlDetect;
			Recorder* recorder;
			IplImage* undistorted;
			IplImage* differenceImage;
			IplImage* dest;
			
	};
}
}//ram::vision
#endif