= paSpectrogram =

Real-time, CD-audio quality, computer microphone driven
spectrogram (waterfall plot) with dynamic noise floor

Dependencies:
 * PortAudio (can be downloaded from Xcode)
 * OpenGL and GLUT (preinstalled on Macs)

To run, click "Build and Run" within Xcode.

Controls:
 Noise floor cutoff
  '-': decrease steepness of nosie floor cutoff
  '+': increase steepness of noise floor cutoff
 
 Noise floor update rate
  '[': decrease stride between noise floor updates
  ']': increase stride between noise floor updates
 Smaller stride means more fine-grained noise floor estimate, but
 slower frame rate.
 Larger stride means more coarse noise floor estimate, but
 faster frame rate.
 
 Screen update stride
  ';': decrease stride between screen updates
  ''': increase stride between screen updates
 Smaller stride means high time resolution on vertical scale, but
 slower frame rate.
 Larger stride means low time resolution on vertical scale, but
 faster frame rate.
