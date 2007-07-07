#include "core/include/AveragingQueue.h"
#include <iostream>
using namespace std;
using namespace ram::core;
int main()
{
    int total = 0;
    AveragingQueue* q=new AveragingQueue(10);
    for (int i=1;i<25;i++)
    {
        total += i;
        cout<<"adding:"<<i<<endl;
        q->addValue(i);
        cout<<"Average (Total: " << "):"<<q->getAverage()<<endl;
    }
    return 0;
}
