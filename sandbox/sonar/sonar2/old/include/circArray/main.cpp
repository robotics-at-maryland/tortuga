//Code to test the circArray class

#include <iostream>
#include "circArray.h"

using namespace std;

int main()
{
    double myarray[]={2.3, 4.5, 6.7, 8.9, 1.2};
    circArray <int, 13> array1;
    circArray<double, 5> array2(myarray);

    cout<<"Array1:\n";
    for(int k=0; k<array1.length();k++)
        cout<<array1[k]<<" ";
    cout<<endl;

    cout<<"Array2:\n";
    for(int k=0; k<array2.length();k++)
        cout<<array2[k]<<" ";
    cout<<endl;

    //Add some data
    array1.update(10);
    array1.update(15);
    array1.update(20);

    array2.update(3.4);
    array2.update(5.6);
    array2.update(7.8);
    cout<<"Array1:\n";
    for(int k=0; k<array1.length();k++)
        cout<<array1[k]<<" ";
    cout<<endl;

    cout<<"Array2:\n";
    for(int k=0; k<array2.length();k++)
        cout<<array2[k]<<" ";
    cout<<endl;

    for(int k=1; k<22; k++)
        array1.update(k);

    for(int k=1; k<4; k++)
        array2.update(k);

    cout<<"Array1:\n";
    for(int k=0; k<array1.length();k++)
        cout<<array1[k]<<" ";
    cout<<endl;

    cout<<"Array2:\n";
    for(int k=0; k<array2.length();k++)
        cout<<array2[k]<<" ";
    cout<<endl;

    int* pointer=*array1;
    cout<<"Array1:\n";
    for(int k=0; k<array1.length();k++)
        cout<<pointer[k]<<" ";
    cout<<endl;

    array2.refill(myarray);
    cout<<"Array2:\n";
    for(int k=0; k<array2.length();k++)
        cout<<array2[k]<<" ";
    cout<<endl;

    return 0;
}
