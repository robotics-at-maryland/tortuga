dPosX = data(:,1);
dPosY = data(:,2);

dVelX = data(:,3);
dVelY = data(:,4);

dAccX = data(:,5);
dAccY = data(:,6);

ePosX = data(:,7);
ePosY = data(:,8);

eVelX = data(:,9);
eVelY = data(:,10);

mass = data(:,11);

force_xn = data(:, 25);
force_yn = data(:, 26);

time = data(:,31);

figure()
subplot(4,1,1)
plot(time, dPosX, 'b.', time, ePosX, 'g.', time, dPosY, 'r.', time, ePosY, 'c.');
subplot(4,1,2)
plot(time, dVelX, 'b.', time, eVelX, 'g.', time, dVelY, 'r.', time, eVelY, 'c.');
subplot(4,1,3)
plot(time, dAccX, 'b.', time, dAccY, 'r.');
subplot(4,1,4)
plot(time, force_xn, 'b.', time, force_yn, 'r.');