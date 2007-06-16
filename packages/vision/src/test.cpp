#include <dlfcn.h>
#include <stdio.h>
#include "../include/VisionData.h"

typedef VisionData* (*visFunction)();

int main()
{
  void* myFunc;
  void* handle=dlopen("./vision.so",RTLD_NOW);
  if (handle==NULL)
    {
      printf("%s\n",dlerror());
    }
  else
    myFunc=dlsym(handle,"getDummy");

  if (myFunc==NULL)
    {
      printf("now I'm going to cry\n");
    }
  else
    {
      printf("%p\n",myFunc);
      visFunction func=(visFunction) myFunc;
      VisionData* x=func();

      printf("x=%d\n%d\n%d\n%d\n%d\n%d\n%f\n%d\n%d\n%d\n%d\n%d\n%d\n",x->frameNum,x->width,x->height,x->redLightx,x->redLighty,x->distFromVertical,x->angle,x->binx,x->biny,x->lightVisible,x->pipeVisible,x->binVisible,x->frameNumCheck);
    }

  return 1;
}
