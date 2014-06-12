function dx = makex(t,x)

dx=zeros(2,1);

dx(1) = x(2);
dx(2) = -9.8;