function [XX,ZZ] = kalman()
  dt = 1/10;
  t = 0:dt:200;
  
  % Have the sub move in a circle.
  speed = 0.5; # meters per second
  r = 15; # radius in meters
  circum = 2*pi*r;
  w = 2*pi*speed / circum;
  x = r*cos(w*t);
  y = r*sin(w*t);
  xdot = -r*w*sin(w*t);
  ydot = r*w*cos(w*t);
  # Yaw the sub back and forth.
  dyaw = pi/6;
  v = 2*pi*0.25; #every four seconds
  eta = dyaw*sin(v*t);
  etadot = dyaw*v*sin(v*t);
  
  # Pingers
  px = [-10;10;0;0];#10/sqrt(2);-10/sqrt(2)];
  py = [0;0;10;-10];#10/sqrt(2);-10/sqrt(2)];
  th = (px*x+py*y)./sqrt((px.*px+py.*py)*(x.*x+y.*y)) - repmat(eta,size(px,1),1);
  
  # Noise
  sigma_compass = 3 * pi / 180;
  sigma_pinger = 6 * pi / 180;
  sigma_gyro = 3 * pi / 180;
  
  # Angle measurements
  compass = randn_renormalized(eta, sigma_compass, size(eta));
  gyro    = randn_renormalized(etadot, sigma_gyro, size(etadot));
  pinger  = randn_renormalized(th, sigma_pinger, size(th));
  
  A = [1, 0, 0, 0; 0, 1, 0, 0; 0, 0, 1, dt; 0, 0, 0, 1];
  W = eye(3);
  dThDx = @(x,y,Rx,Ry) (y.*(-Ry*x+Rx*y)./(sqrt(px.*px+py.*py)*(x.*x+y.*y).^(3/2)));
  dThDy = @(x,y,Rx,Ry) (x.*(Ry*x-Rx*y)./(sqrt(px.*px+py.*py)*(x.*x+y.*y).^(3/2)));
  H1 = @(x,y) ([horzcat([dThDx(x,y,px,py),dThDy(x,y,px,py)],-ones(size(px,1),1),zeros(size(px,1),1));0,0,1,0;0,0,0,1]);
  H = @(X) H1(X(1,:),X(2,:));
  h1 = @(x,y,eta,etadot) [((px*x+py*y)./sqrt((px.*px+py.*py)*(x.*x+y.*y)));eta;etadot];
  h = @(X) h1(X(1,:),X(2,:),X(3,:),X(4,:));
  
  Q = diag([0.5, 0.5, 0, pi/180]);
  R = diag([repmat(sigma_pinger,1,size(px,1)), sigma_compass, sigma_gyro]);
  P = diag([2,2,pi/45,pi/90]) + ones(4)*0.001;
  X = [r; 0; 0; 0];

  ZZ = vertcat(pinger, compass, gyro);
  XX = zeros(4,size(ZZ,2));
  for i = 1:length(t)
    X = A*X;
    P = A*P*A' + Q;
    HH = H(X);
    K = P*HH'*inv(HH*P*HH'+R);
    X = X + K*(ZZ(:,i)-h(X));
    P = (eye(4) - K*HH)*P;
    XX(:,i) = X;
  end
  plot(x,y,'-b',XX(1,:),XX(2,:),'-g',px,py,'rx'),;
  legend("Actual path","Estimated path","Pingers");
end
