function result=DHT(x)
%implementation of the discrete Hartley transform...
%Hn=(1/sqrt(N))*(cos(2*pi*m*n/N)+sin(2*pi*m*n/N))
%http://en.wikipedia.org/wiki/Discrete_Hartley_transform
%the major benefit of this transform is it requires only
%calculations with real numbers, however to get full phase
%information the negative and positive frquency bins are
%required.
N=size(x,2);
common=(2*pi)/N;
m=0:N-1;
n=0:N-1;
H=(1/sqrt(N)).*(cos(common.*(m'*n))+sin(common.*(m'*n)));
result=H*x';