module ram {
    
    module sonar {
        
        module scope {
            
            enum TriggerMode {
                TriggerModeStop,
                TriggerModeRun,
                TriggerModeAuto
            };
            
            enum TriggerSlope {
                TriggerSlopeRising,
                TriggerSlopeFalling
            };
            
            sequence<short> ShortSeq;
            
            struct OscilloscopeCapture {
                
                ShortSeq rawData;
                long timestamp;
                TriggerMode newTriggerMode;
                
            };
            
            
            sequence<int> IntSeq;
            
            struct SpectrumCapture {
                
                IntSeq currentLevels;
                IntSeq peakLevels;
                long timestamp;
                
            };
            
            interface Viewer {
                
                idempotent void NotifyCapture();
                
            };
            
            enum ScopeMode {
                ScopeModeOscilloscope,
                ScopeModeSpectrumAnalyzer
            };
            
            interface Oscilloscope {
                
                
                // Configuration routines
                
                idempotent void SetViewer(Viewer* v);
                idempotent void SetScopeMode(ScopeMode mode);
                
                
                // Oscilloscope routines
                
                idempotent void SetTriggerMode(TriggerMode mode);
                idempotent void SetTriggerChannel(short channel);
                idempotent void SetTriggerLevel(short level);
                idempotent void SetTriggerHoldoff(int holdoff);
                idempotent void SetTriggerSlope(TriggerSlope slope);
                
                //idempotent void SetHorizontalPosition(int timeShift);
                idempotent void SetHorizontalZoom(short powerOfTwo);
                
                OscilloscopeCapture GetLastCapture();
                
                
                
                // Spectrum analyzer routines
                
                SpectrumCapture GetLastSpectrum();
                
            };
            
        };
        
    };
    
};
