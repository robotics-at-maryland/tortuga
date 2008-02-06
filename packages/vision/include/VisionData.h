typedef struct
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
  int lightVisible;
  int  pipeVisible;
  int binVisible;
  int frameNumCheck;
}VisionData;
