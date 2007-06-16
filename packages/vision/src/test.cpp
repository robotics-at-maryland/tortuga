#include <dlfcn.h>
#include <stdio.h>

typedef int (*intFunction)();

int main()
{
  void* myFunc;
  void* handle=dlopen("./vision.so",RTLD_NOW);
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
      intFunction func=(intFunction) myFunc;
      int x=func();

      printf("x=%d\n",x);
    }

  return 1;
}
