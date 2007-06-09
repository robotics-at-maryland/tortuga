#include <stdio.h>
#include <stdlib.h>

int x = 0;

int increment() {
  return x;
}
int run() {
  while(1) {
    x++;
  }
}
