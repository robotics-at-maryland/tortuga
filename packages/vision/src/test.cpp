#include <string>
#include <dlfcn.h>
#include <stdio.h>
#include "../include/VisionData.h"

typedef VisionData* (*visFunction)();
typedef int* (*intFunc)();

int main()
{
  //  std::string filename(getenv("RAM_SVN_DIR"));
  //filename += "/build/lib/libram_vision.dylib";
  //std::count 


  void* myFunc;
  void* handle=dlopen("libram_vision.so", RTLD_NOW);
  if (handle==NULL)
    {
      printf("%s\n",dlerror());
    }
  else
    myFunc=dlsym(handle,"visionStart");

  if (myFunc==NULL)
    {
      printf("now I'm going to cry\n");
    }
  else
    {
      printf("%p\n",myFunc);
      intFunc func=(intFunc) myFunc;
      int* x=func();
    }

  return 1;
}

