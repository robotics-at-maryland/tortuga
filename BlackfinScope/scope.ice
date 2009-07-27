#include "Ice/BuiltinSequences.ice"

module ram {
    
    module sonar {
        
        module scope {
            
            enum TriggerMode {
                TriggerModeRun,
                TriggerModeStop,
                TriggerModeAuto
            };
            
            struct OscilloscopeCapture {
                
                Ice::ShortSeq rawData;
                long timestamp;
                
            };
            
            interface Viewer {
                
                void NotifyCapture();
                
            };
            
            interface Oscilloscope {
                
                idempotent void SetViewer(Viewer* v);
                
                idempotent void SetTriggerMode(TriggerMode mode);
                idempotent void SetTriggerChannel(short channel);
                idempotent void SetTriggerLevel(short level);
                idempotent void SetTriggerHoldoff(int holdoff);
                
                idempotent void SetHorizontalPosition(int timeShift);
                idempotent void SetHorizontalZoom(short powerOfTwo);
                
                OscilloscopeCapture GetLastCapture();
                
            };
            
        };
        
    };
    
};
