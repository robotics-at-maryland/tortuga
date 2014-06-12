/* Test cases for the 2x2 matrix class
     Written by Alex Janas on 2/18/08
*/
#include <UnitTest++/UnitTest++.h>

#include "math/test/include/MathChecks.h"
//#include "math/include/Quaternion.h"
#include "math/include/Matrix2.h"
#include "math/include/Math.h"

using namespace ram::math;

TEST(operator_plus)
{
Matrix2 a (3.0,1.0,4.0,1.0);
Matrix2 b (1.0,2.0,6.0,3.0);
Matrix2 c;

c = a+b;
double result[2][2] = {{4.0,3.0},{10.0,4.0}};


CHECK_ARRAY2D_CLOSE(result, c, 2,2, .0001); 
}



TEST(operator_minus)
{
Matrix2 a (3.0,1.0,4.0,1.0);
Matrix2 b (1.0,2.0,6.0,5.0);
Matrix2 c;

c = a-b;
double result[2][2] = {{2.0,-1.0},{-2.0,-4.0}};


CHECK_ARRAY2D_CLOSE(result, c, 2,2, .0001); 
}


TEST(negate)
{
Matrix2 a (3.0,1.0,4.0,-2.0);
Matrix2 c;
c = -a;
double result[2][2] = {{-3.0,-1.0},{-4.0,2.0}};


CHECK_ARRAY2D_CLOSE(result, c, 2,2, .0001); 
}


TEST(operator2x2by2x2)
{
Matrix2 a (3.0,1.0,4.0,1.0);
Matrix2 b (1.0,2.0,6.0,3.0);
Matrix2 c;

c = a*b;
double result[2][2] = {{9.0,9.0},{10.0,11.0}};


CHECK_ARRAY2D_CLOSE(result, c, 2,2, .0001); 
}



TEST(operator2x2byscaler)
{
Matrix2 a (3.0,1.0,4.0,1.0);
double b = 4.0;
Matrix2 c;

c = a*b;
double result[2][2] = {{12.0,4.0},{16.0,4.0}};


CHECK_ARRAY2D_CLOSE(result, c, 2,2, .0001); 
}




TEST(operatorscalerby2x2)
{
Matrix2 a (3.0,1.0,4.0,1.0);
double b = 4.0;
Matrix2 c;

c = b*a;
double result[2][2] = {{12.0,4.0},{16.0,4.0}};


CHECK_ARRAY2D_CLOSE(result, c, 2,2, .0001); 
}



TEST(operator2x2by1x2)
{
Matrix2 a (3.0,1.0,4.0,-2.0);
Vector2 c;
Vector2 b (2.0,7.0);
c = a*b;
double result[2] = {13.0,-6.0};


CHECK_ARRAY_CLOSE(result, c, 2, .0001); 
}




TEST(operator2x1by2x2)
{
Matrix2 a (3.0,1.0,4.0,-2.0);
Vector2 b (2.0,7.0);
Vector2 c;
c = b*a;
double result[2] = {34,-12};


CHECK_ARRAY_CLOSE(result, c, 2, .0001); 
}




TEST(operator_equal)
{
Matrix2 a (3.0,1.0,4.0,-2.0);
Matrix2 b (3.0,1.0,4.0,-2.0);
double c = 3;

if(a==b)
{
c = 1;
}


CHECK_EQUAL(c, 1);
}


TEST(operator_notequal)
{
Matrix2 a (3.0,1.0,4.0,-2.0);
Matrix2 b (3.0,-1.0,3.0,-2.0);
double c = 3;

if (a!=b)
{
	c=1;
}



CHECK_EQUAL(c,1); 
}



TEST(ZERO)
{
Matrix2 a = Matrix2::ZERO;
double b[2][2] = {{0,0},{0,0}};

CHECK_ARRAY2D_CLOSE(b,a,2,2,.00001);
}


TEST(IDENTITY)
{
Matrix2 a = Matrix2::IDENTITY;
double b[2][2] = {{1,0},{0,1}};
CHECK_ARRAY2D_CLOSE(b,a,2,2,.00001);
}


//////When building check if this is the best option
TEST(GetColumn)
{
Matrix2 a (2.0,5.0,1.0,3.0);
Vector2 b = a.GetColumn(0);
//double d[2] = {b.x,b.y};
// double d[2] = {{b[0],b[1]};
double c[2] = {2.0,1.0};

CHECK_ARRAY_CLOSE(c,b,2,.00001);
}



TEST(SetColumn)
{
Matrix2 a (7.0,5.0,2.0,3.0);
Vector2 b (2.0,1.0);
a.SetColumn(0,b);
Matrix2 c (2.0,5.0,1.0,3.0);

CHECK_ARRAY2D_CLOSE(a,c,2,2,.00001);
}



