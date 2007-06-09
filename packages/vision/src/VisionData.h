struct VisionData
{
  int frameNum;
  int width;
  int height;
  
  int redLightx;
  int redLighty;
  
  int distFromVertical;
  double angle;
  
  int binx;
  int biny;

  ///Tells you what is visible
  bool lightVisible;
  bool pipeVisible;
  bool binVisible;
  int frameNumCheck;
};
