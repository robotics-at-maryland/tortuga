#ifndef RAM_DETECTOR_TEST_H_07_11_2007
#define RAM_DETECTOR_TEST_H_07_11_2007

#include "core/include/Updatable.h"
#include "core/include/EventPublisher.h"

#include "vision/include/OpenCVCamera.h"
#include "vision/include/OrangePipeDetector.h"
#include "vision/include/GateDetector.h"
#include "vision/include/RedLightDetector.h"
#include "vision/include/BinDetector.h"
#include "vision/include/FeatureDetector.h"
#include "vision/include/Recorder.h"
#include "vision/include/Events.h"

// Must Be Included Last
#include "vision/include/Export.h"

namespace ram{
namespace vision{

	class RAM_EXPORT DetectorTest : public core::Updatable, public core::EventPublisher
	{
		public:
		
			DetectorTest(int, bool);
			DetectorTest(std::string);
			~DetectorTest();
			bool orangeOn;
			bool lightOn;
			bool gateOn;
			bool binOn;
			bool featureOn;
			ram::vision::OpenCVCamera* camera;
			void orangeDetectOn();
			void lightDetectOn();
			void gateDetectOn();
			void binDetectOn();
			void featureDetectOn();
			void orangeDetectOff();
			void lightDetectOff();
			void gateDetectOff();
			void binDetectOff();
			void featureDetectOff();
			void allDetectorsOff();
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
			FeatureDetector* fDetect;
			Recorder* recorder;
			bool fromMovie;
			IplImage* undistorted;
			IplImage* differenceImage;
			IplImage* dest;
			IplImage* dataMove;
			
	};
} // namespace vision
} // namespace ram

#endif // RAM_DETECTOR_TEST_H_07_11_2007
