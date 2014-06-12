clear;
clc;
close all
% T3 = [10 20 40 60 80 100 120 120 90 80];

F1 = 50.0 * ones(1,10);  % left right thruster
F2 = 0.0 * ones(1,10);  % top bottom thruster
F3 = 0.0 * ones(1,10);  % front back thruster
T1 = 0.0 * ones(1,10);  % roll 
T2 = 0.0 * ones(1,10);  % pitch
T3 = 0.0 * ones(1,10);  % yaw


old_forces = [F1;F2;F3;T1;T2;T3];

%forward back up down right left
%thruster_vals = [0 0 0 0 0 0];
thruster_vals = getThrusterValues(old_forces(1,1), old_forces(2,1), old_forces(3,1), old_forces(4,1), old_forces(5,1), old_forces(6,1));
%old_thruster_vals = [0;0;0;0;0;0]
    old_thruster_vals(:,1) = thruster_vals';
for x = 2: length(old_forces)
    thruster_vals = getThrusterValues(old_forces(1,x), old_forces(2,x), old_forces(3,x), old_forces(4,x), old_forces(5,x), old_forces(6,x));
    old_thruster_vals(:,end+1) = thruster_vals';
end
old_thruster_vals;
%thruster_vals = getThrusterValues(F1(1), F2(1), F3(1), T1(1), T2(1), T3(1));
new_thruster_vals3 = old_thruster_vals;
thruster_max = 50.0;

new_thruster_vals = modifyThrusterValues3(new_thruster_vals3, thruster_max);
new_thruster_vals2 = modifyThrusterValues(new_thruster_vals3, thruster_max);
%new_thruster_vals2 = new_thruster_vals;
%for x = 1:length(new_thruster_vals)
%    if(max(new_thruster_vals(:,x)) > thruster_max)
%        ratio = thruster_max / max(new_thruster_vals(:,x));
%        new_thruster_vals(:,x) = new_thruster_vals(:,x) * ratio;
%    end
%end
%thruster_vals(:,1)
new_thruster_vals;
force_vals = getForceValues(new_thruster_vals(1,1), new_thruster_vals(2,1), new_thruster_vals(3,1), new_thruster_vals(4,1), new_thruster_vals(5,1), new_thruster_vals(6,1));
    new_force_vals(:,1) = force_vals';
for x = 2:length(new_thruster_vals)
    force_vals = getForceValues(new_thruster_vals(1,x), new_thruster_vals(2,x), new_thruster_vals(3,x), new_thruster_vals(4,x), new_thruster_vals(5,x), new_thruster_vals(6,x));
    new_force_vals(:,end+1) = force_vals';
end

force_vals2 = getForceValues(new_thruster_vals2(1,1), new_thruster_vals2(2,1), new_thruster_vals2(3,1), new_thruster_vals2(4,1), new_thruster_vals2(5,1), new_thruster_vals2(6,1));
    new_force_vals2(:,1) = force_vals2';
for x = 2:length(new_thruster_vals2)
    force_vals2 = getForceValues(new_thruster_vals2(1,x), new_thruster_vals2(2,x), new_thruster_vals2(3,x), new_thruster_vals2(4,x), new_thruster_vals2(5,x), new_thruster_vals2(6,x));
    new_force_vals2(:,end+1) = force_vals2';
end
new_force_vals2;

%old_forces;
%old_thruster_vals;
%new_thruster_vals;
%new_force_vals;

time = 1:10;
axis([time(1) time(end) -5 (thruster_max+20)]);
%title('Thruster Values');
%set(text(.4,0.95, 'Thruster Values'), 'fontweight','bold');
%[front back top bottom right left]
hold on;
subplot(3,2,1);
plot(time, old_thruster_vals(1,:));
hold on;
plot(time, new_thruster_vals(1,:), 'g');
plot(time, new_thruster_vals2(1,:), 'r');
title('Front');
xlabel('Time');
ylabel('Input');
subplot(3,2,2);
plot(time, old_thruster_vals(2,:));
hold on;
plot(time, new_thruster_vals(2,:), 'g');
plot(time, new_thruster_vals2(2,:), 'r');
title('Back');
xlabel('Time');
ylabel('Input');
subplot(3,2,3);
plot(time, old_thruster_vals(3,:));
hold on;
plot(time, new_thruster_vals(3,:),  'g');
plot(time, new_thruster_vals2(3,:), 'r');
title('Top');
xlabel('Time');
ylabel('Input');
subplot(3,2,4);
plot(time, old_thruster_vals(4,:));
hold on;
plot(time, new_thruster_vals(4,:), 'g');
plot(time, new_thruster_vals2(4,:), 'r');
title('Bottom');
xlabel('Time');
ylabel('Input');
subplot(3,2,5);
plot(time, old_thruster_vals(5,:));
hold on;
plot(time, new_thruster_vals(5,:), 'g');
plot(time, new_thruster_vals2(5,:), 'r');
title('Right');
xlabel('Time');
ylabel('Input');
subplot(3,2,6);
plot(time, old_thruster_vals(6,:));
hold on;
plot(time, new_thruster_vals(6,:), 'g');
plot(time, new_thruster_vals2(6,:), 'r');
title('Left');
xlabel('Time');
ylabel('Input');
legend('Requested Input', 'Algorithm 1, Equal Limiting', 'Algorithm 2, Weighted Limiting');
%set(text(.4,0.95, 'Thruster Values'), 'fontweight','bold');
hold off;
figure;
% forward left up roll pitch yaw
hold on;
subplot(3,2,1);
plot(time, old_forces(1,:));
hold on;
plot(time, new_force_vals(1,:), 'g');
plot(time, new_force_vals2(1,:), 'r');
title('Forward');
xlabel('Time');
ylabel('Force');
subplot(3,2,3);
plot(time, old_forces(2,:));
hold on;
plot(time, new_force_vals(2,:), 'g');
plot(time, new_force_vals2(2,:), 'r');
title('Left');
xlabel('Time');
ylabel('Force');
subplot(3,2,5);
plot(time, old_forces(3,:));
hold on;
plot(time, new_force_vals(3,:), 'g');
plot(time, new_force_vals2(3,:), 'r');
title('Up');
xlabel('Time');
ylabel('Force');
subplot(3,2,2);
plot(time, old_forces(4,:));
hold on;
plot(time, new_force_vals(4,:), 'g');
plot(time, new_force_vals2(4,:), 'r');
title('Roll');
xlabel('Time');
ylabel('Torque');
subplot(3,2,4);
plot(time, old_forces(5,:));
hold on;
plot(time, new_force_vals(5,:), 'g');
plot(time, new_force_vals2(5,:), 'r');
title('Pitch');
xlabel('Time');
ylabel('Torque');
subplot(3,2,6);
plot(time, old_forces(6,:));
hold on;
plot(time, new_force_vals(6,:), 'g');
plot(time, new_force_vals2(6,:), 'r');
title('Yaw');
xlabel('Time');
ylabel('Torque');
legend('Requested Input', 'Algorithm 1, Equal Limiting', 'Algorithm 2, Weighted Limiting');
%set(text(.1,0.95, 'Force Values'), 'fontweight','bold');
hold off;











