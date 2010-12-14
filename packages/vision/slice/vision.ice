
module proxy {
    module vision {
        interface VisionSystem {
            void redLightDetectorOn();
            void redLightDetectorOff();
            void buoyDetectorOn();
            void buoyDetectorOff();
            void binDetectorOn();
            void binDetectorOff();
            void pipeLineDetectorOn();
            void pipeLineDetectorOff();
            void ductDetectorOn();
            void ductDetectorOff();
            void downwardSafeDetectorOn();
            void downwardSafeDetectorOff();
            void gateDetectorOn();
            void gateDetectorOff();
            void targetDetectorOn();
            void targetDetectorOff();
            void windowDetectorOn();
            void windowDetectorOff();
            void barbedWireDetectorOn();
            void barbedWireDetectorOff();
            void hedgeDetectorOn();
            void hedgeDetectorOff();
            void velocityDetectorOn();
            void velocityDetectorOff();
        };
    };
};
