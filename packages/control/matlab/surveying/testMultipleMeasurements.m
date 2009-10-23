clear
clc
o = Object('testObject');

xObj = 3;
yObj = 6;

x = [3 12 -10 -5 -10];
sigx = [.2 .4 1 .5 .9];
y = [1 6 6 5 6];
sigy = [.2 .3 .6 .4 .7];

phi = asin((xObj-x)./sqrt((xObj-x).^2+(yObj-y).^2));
sigphi = [toRad(4),toRad(3),toRad(4),toRad(3),toRad(4)];

for i = 1:3

    m = Measurement();
    
    time = getTime();
    m.name = strcat(num2str(i),':',num2str(time.hours),':',num2str(time.minutes),':',num2str(time.seconds));
    
    m.x = random('Normal',x(i),sigx(i));
    m.sigx = sigx(i);
    m.y = random('Normal',y(i),sigy(i));
    m.sigy = sigy(i);
    m.phi = random('Normal',phi(i),sigphi(i));
    m.sigphi = sigphi(i);
    m.D = 4;
    
    o.addMeasurement(m);
    
end

[r a] = CalculatePosition(o.measurements)

NumStdvX = (r.x-xObj)/r.sigx
NumStdvY = (r.y-yObj)/r.sigy