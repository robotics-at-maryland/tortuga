function y = ping(f, fs)
dt = 1/fs;
t = 0:dt:0.2;
y = sin(2*pi*f*t).*(t >= 0.002).*(t<=0.0033);
y = y';
echotimes = rand(5,1)*0.008;
for i=1:3
echotimes = bounce(echotimes);
end
echotimes = sort(echotimes);
echoamps = ((echotimes+0.002)/0.002) .^ -2;
echotimes = floor(echotimes*fs);
y = reverb(y, echotimes, echoamps);
y = y + rand(length(t),1)*.02;
y = y .* 2^15;
y = int16(y);

function echotimes=bounce(echotimes)
echotimes = pdist([0;echotimes], @(u,v) (repmat(2.*u,size(v,1),1)+v));
end
