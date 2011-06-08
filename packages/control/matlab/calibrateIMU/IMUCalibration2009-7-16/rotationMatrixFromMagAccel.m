function R = rotationMatrixFromMagAccel( mag, accel )
%ROTATIONMATRIXFROMMAGACCEL Calculate a rotation matrix from magnetometer
% and accelerometer readings while holding the nominal orientation

% calculate column vectors of the averages for each axis for each sensor
a = mean(accel)';
m = mean(mag)';

n3=-a;
n3=n3/norm(n3);
n2=cross(m,a);
n2=n2/norm(n2);
n1=cross(n2,n3);

R=[n1'; n2'; n3']';

end
