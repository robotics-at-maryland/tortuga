data=[-0.778 0.612 -0.139;
-0.535 0.821 -0.201;
-0.231 0.945 -0.233;
0.021 0.980 -0.198;
0.288 0.932 -0.220;
0.513 0.830 -0.219;
0.868 0.815 0;
0.862 0.461 -0.212;
0.958 0.194 -0.212;
0.860 -0.448 -0.244;
0.638 -0.742 -0.204;
0.301 -0.928 -0.221;
-0.035 -0.966 -0.258;
-0.370 -0.882 -0.293;
-0.548 -0.736 -0.397;
-0.821 -0.514 -0.248;
-0.891 -0.208 -0.404;
-0.981 0.050 -0.187];

points=[0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 11, 12 ,13, 14, 15, 16, 17, 18];

angles=atan2(data(:,1),data(:,2))*180/pi();

line=points*18-55;

for i=1:length(angles)
    if(angles(i)<-60)
        angles(i)=angles(i)+360;
    end
end

close all;
figure(1);
plot(points, angles,'r*');
hold on;
plot(points, line,'b.-');
xlabel('Point number');
ylabel('Angle');
title('Yaw angle');
legend('Hydrophone measurements', 'Expected', 'Location','SouthEast');

figure(2);
plot(points, -data(:,3),'r*');
xlabel('Point number');
ylabel('Z component');
hold on;
avline(1:length(points))=mean(data(:,3));
plot(points, -avline,'b.-');

title('Z component, expected to stay constant');
legend('Hydrophone measurements', 'Average', 'Location','SouthEast');
