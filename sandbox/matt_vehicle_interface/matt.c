#include <stdlib.h>

typedef struct
{
  int a;
  int b;
  int c;
  int d;
  int e;
  int f;
  int g;
} MyInts;

static MyInts unsafe;
static MyInts** safe;
static MyInts guaranteed;
static MyInts dummyCheck;

static int startGet=0;
static int endGet=0;

MyInts* getDummy()
{
  dummyCheck.a=1;
  dummyCheck.b=1;
  dummyCheck.c=2;
  dummyCheck.d=3;
  dummyCheck.e=5;
  dummyCheck.f=8;
  dummyCheck.g=13;
  
  return &dummyCheck;
}

MyInts* getUnsafe()
{
  return &unsafe;
}
 
MyInts** getSafe()
{
  return safe;
}

MyInts* captureSnapshot(int);

MyInts* getData()
{
  return captureSnapshot(10);
}

MyInts* captureSnapshot(int tries)
{
  int x=0;
  for(x=0;x<tries;x++)
    {
      MyInts* buffer=*safe;
      guaranteed.a=(buffer->a);
      guaranteed.b=(buffer->b);
      guaranteed.c=(buffer->c);
      guaranteed.d=(buffer->d);
      guaranteed.e=(buffer->e);
      guaranteed.f=(buffer->f);
      guaranteed.g=(buffer->g);
      
      if (guaranteed.a==guaranteed.g)
	return &guaranteed;
    }
  return *safe;
}	

int main()
{
  MyInts  duplicateMe;
  MyInts *buffer1,*buffer2;

  buffer1=malloc(sizeof(MyInts));
  buffer2=calloc(1,sizeof(MyInts));
  
  int swapper=2;
  unsafe.a=0;
  unsafe.b=0;
  unsafe.c=0;
  unsafe.d=0;
  unsafe.e=0;
  unsafe.f=0;
  unsafe.g=0;

  buffer1->a=0;
  buffer1->b=0;
  buffer1->c=0;
  buffer1->d=0;
  buffer1->e=0;
  buffer1->f=0;
  buffer1->g=0;

  while(1)
    {
      unsafe.a++;
      unsafe.b++;
      unsafe.c++;
      unsafe.d++;
      unsafe.e++;
      unsafe.f++;
      unsafe.g++;
      if (swapper==1)
	{
	  buffer1->a=buffer2->a+1;
	  buffer1->b=buffer2->b+1;
	  buffer1->c=buffer2->c+1;
	  buffer1->d=buffer2->d+1;
	  buffer1->e=buffer2->e+1;
	  buffer1->f=buffer2->f+1;
	  buffer1->g=buffer2->g+1;
	  swapper=2;
	  safe=&buffer1;
	}
      else if (swapper==2)
	{
	  buffer1->a=buffer2->a+1;
	  buffer1->b=buffer2->b+1;
	  buffer1->c=buffer2->c+1;
	  buffer1->d=buffer2->d+1;
	  buffer1->e=buffer2->e+1;
	  buffer1->f=buffer2->f+1;
	  buffer1->g=buffer2->g+1;
	  swapper=1;
	  safe=&buffer2;
	}
    }
}
