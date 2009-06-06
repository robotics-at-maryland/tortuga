%frequency input is in hertz
freq = 60;
m=20;
c=11.5;
A = [0 1;0 -c/m];
B = [0; 1/m];
C = [1 0];
D = 0;


continuous_system = ss(A,B,C,D);

dt = 1/freq;

discrete_system = c2d(continuous_system,dt);

[Ad Bd Cd Dc]=ssdata(discrete_system);


L = (place(Ad',Cd',[-10,-9]))'