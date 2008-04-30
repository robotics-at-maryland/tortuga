/* Author: Scott Watson
 * Date: April 19, 2008
 * last modified(If I remembered to update this): April 19, 2008
 * 
 * IIR filter code: 
 * biquadFilter: first library function performce general biquad IIR filter
 *		with explicit coefficients
 * biquadFilter2: more efficient than 'biquadFilter', coefficients are passed 
 *		by arrays which seems to speed things up
 * biquadFilter3: (PLANNED!) most efficient implementation of a biquad filter.
 *		uses MAC instructions and hand assembly tuning 
 * secondOrderFIR: sometimes there may only be enough spare instruction cycles
 * 		to run a second order FIR filter.  it takes about half the time of an IIR.
 * secondOrderFIR2: (PLANNED) most efficient implementation of 2nd order FIR Filter
 *		makes use of MAC instruction and hand assembly tuning
 */

//takes between 903 and 1414 instruction cycles to call
float biquadFilter(float input, float A0, float A1, float A2, float B1, float B2, float s[]) {
	//From my matlab implementation...
	//v=x(n) - sum(B(2:NAB).*s(1:NAB-1));
	//y(n)=A(1)*v + sum(A(2:NAB).*s(1:NAB-1));
	//s(2:NAB)=s(1:NAB-1);
	//s(1)=v;
	float output;
	s[0] = input - (B1*s[1] + B2*s[2]);
	output = s[0]*A0 + s[1]*A1 + s[2]*A2;
	s[2]=s[1];
	s[1]=s[0]; //s[0] is 'v'
	return output;
}

//takes between 910 and 1080 instruction cycles to call
float biquadFilter2(float input, float A[], float B[], float s[]) {
	//From my matlab implementation...
	//v=x(n) - sum(B(2:NAB).*s(1:NAB-1));
	//y(n)=A(1)*v + sum(A(2:NAB).*s(1:NAB-1));
	//s(2:NAB)=s(1:NAB-1);
	//s(1)=v;
	float output;
	s[0] = input - (B[1]*s[1] + B[2]*s[2]);
	output = s[0]*A[0] + s[1]*A[1] + s[2]*A[2];
	s[2]=s[1];
	s[1]=s[0]; //s[0] is 'v'
	return output;
}

//takes between 882 and 1200 instruction cycles to call
float fixedBiquadFilter1(float input, float s[]) {
	//From my matlab implementation...
	//v=x(n) - sum(B(2:NAB).*s(1:NAB-1));
	//y(n)=A(1)*v + sum(A(2:NAB).*s(1:NAB-1));
	//s(2:NAB)=s(1:NAB-1);
	//s(1)=v;
	float output;
	s[0] = input - (0.2*s[1] + 0.3*s[2]);
	output = s[0]*1.23 + s[1]*0.45 + s[2]*0.34;
	s[2]=s[1];
	s[1]=s[0]; //s[0] is 'v'
	return output;
}

//takes between 505 and 656 instruction cycles to call
//takes 478 instruction cycles if code is copy pasted to desired location
float secondOrderFIR(float input, float A[], float s[]) {
	float output = input*A[0] + s[1]*A[1] + s[2]*A[2];
	s[2]=s[1];
	s[1]=input;
	return output;
}

//these "fast average" filters can be optimized to require one less memory element
//not yet tested.  (should do some FIR lowpass/highpass in this way as well)
//(should also do some lowpass/highpass IIR types in this way

//consistently 28 instructions to execute
int fastAverageFIR1stOrder(int input, int s[]) {
	int sum = input + s[1];
	int output = sum>>1;
	s[1]=input;
	return output;
}
//consistently 51 instructions to execute
int fastAverageFIR2ndOrder(int input, int s[]) {
	int sum = input + s[1] + s[2];
	// 1/4+1/16+1/64+1/256+1/1024+1/4096 = 0.3332519... or close enough to 1/3 (add more terms as neccessary)
	int output = (sum>>2) + (sum>>4) + (sum>>6) + (sum>>8) + (sum>>10);
	s[2]=s[1];
	s[1]=input;
	return output;
}
//consistently 52 instructions to execute
int fastAverageFIR3rdOrder(int input, int s[]) {
	int sum = input + s[1] + s[2] + s[3];
	int output = sum>>2;
	s[3]=s[2];
	s[2]=s[1];
	s[1]=input;
	return output;
}
        
/* The following is matlab code that impliments a direct form
 * IIR filter that I wrote for my DSP class(ENEE425)
 * 
function y=myIIRtype1(x,A,B)
% Author: Scott Watson
% This is my own implementation of an IIR type one direct form filter.  
% It takes as input a vector 'x' representing sampled data, and it uses 
% the A and B vectors to determine the transfer function(H(z)) used 
% to produce the output 'y'.
%	      1      A(z)
% X(z)* ----- * ----- = Y(z)
%        B(z)     1
%
%	      1                    A(z)
% X(z)* ----- = v(z) ;  v(z)* ----- = Y(z)
%        B(z)                   1
% STEPS:
% 1) v[n] = x[n] - b1*s1[n] - b2*s[n] - ... - bN*sN[n]
% 2) y[n] = a0*v[n] + a1*s1[n] + ... + aN*sN[n]
% 3) sN[n+1] = sN-1[n]
%	 sN-1[n+1] = sN-2[n]
%	 ...
%	 s1[n+1] = v[n]
%
% ___example usage___
% A1=[1 1.732154 1];B1=[1 -0.620227 0.195243];
% x=cos(w.*points.*T); %input signal
% y=myIIRtype1(x,A1,B1);

%zero pad the end of transfer function vectors to be of equal length
if size(A,2)>size(B,2)
	BB(1:size(A,2))=0;
	BB(1:size(B,2))=B;
	clear B
	B=BB;
elseif size(A,2)<size(B,2)
	AA(1:size(B,2))=0;
	AA(1:size(A,2))=A;
	clear A
	A=AA;	
end
% normalize so that lowest order coefficient of B vector is 1
A=A./B(1);
B=B./B(1);
% get length of coeficients and input
NAB=size(A,2);
N=size(x,2);
% initialize output to all zeros
y(1:N)=0;
v=0;
s(1:NAB-1)=0;
for n=1:N
	v=x(n) - sum(B(2:NAB).*s(1:NAB-1));
	y(n)=A(1)*v + sum(A(2:NAB).*s(1:NAB-1));
	s(2:NAB)=s(1:NAB-1);
	s(1)=v;
end*/
