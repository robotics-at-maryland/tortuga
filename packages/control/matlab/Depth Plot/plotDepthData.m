function [result] = plotDepthData(file,depth_start,color)

a = load (file);

% Find the offset index for the starting depth
offset = findDepthFileOffset(a(:,8),depth_start);

% Measured Depth field is column 8
depth_m = a(offset+4:end,8);
% use a 5 point average filter on the depth data
depth_a = 1/5*(a(offset:end-4,8)+a(offset+1:end-3,8)+a(offset+2:end-2,8)+a(offset+3:end-1,8)+a(offset+4:end,8));
depth_d = a(end,16);

% time
t = a(offset+4:end,end)/1000;
t = t - t(1)*ones(1:length(t),1);

% steady state start point
i = round(length(t)*0.93);

result = zeros(1,5);
[result(1) result(2) result(3) result(4) result(5)] = FindStepInfo(t,depth_m,i,depth_start,depth_d,0.02);

% plots measured and desired depth
plot(t,depth_a,color);
set(gca,'YDir','reverse')
%plot(t(i),depth_a(i),'*')

end

function [steady_state,rise_time,settling_time,overshoot,sserror] = FindStepInfo(t,depth_a,i,depth_start,depth_d,settlingThreshold)

steady_state = mean(depth_a(i:end));
sserror = abs(depth_d-steady_state);
overshoot = (max(depth_a) - depth_d)/depth_d;

for j = length(depth_a):-1:1
    if (abs(depth_a(j) - steady_state) > abs(settlingThreshold*steady_state))
        settling_time = t(j)-t(1);
        break;
    end
end

for k = 2:length(depth_a)
   if (depth_a(k) > depth_start)
        break;
   end
end

if (depth_a(k-1) == depth_a(k))
    minRiseTime = t(k-1);
else
    minRiseTime = interp1(depth_a(k-1:k),t(k-1:k),depth_start);
end

for l = k:length(depth_a)
    if (depth_a(l) > depth_d)
        break;
    end
end

maxRiseTime = interp1(depth_a(l-1:l),t(l-1:l),depth_d);
rise_time = maxRiseTime - minRiseTime;

%stepinfo(depth_m,t,5,'SettlingTimeThreshold',0.02)
end

function offset = findDepthFileOffset(data,depth_start)
    offset = 1;
    [value,maxIndex] = max(data);
    for i = maxIndex:-1:1
        if (data(i)-depth_start < 0)
            offset = i;
            break;
        end
    end
end