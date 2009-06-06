function [X,Y,Z,V] = plot_crlb(filename)
config;
posticks = -30:3:30;
depthticks = -30:20:30;
[X,Y,Z]=meshgrid(posticks,posticks,depthticks);
V = zeros(length(posticks),length(posticks),length(depthticks));
config;
load(filename,'hydro_pos');
hydro_pos = hydro_pos * lambda;

for i = 1:length(posticks)
    for j = 1:length(posticks)
        for k = 1:length(depthticks)
           V(i,j,k) = range_error([posticks(i),posticks(j),depthticks(k)], hydro_pos, hydro_pos_accuracy, tdoa_accuracy);
        end
    end
end

m = length(depthticks);
prows = floor(sqrt(m));
pcols = ceil(m/prows);

clf;
title('Projected lower error bound for sonar-based position estimate (meters)');
xlabel('x (meters)');
ylabel('y (meters)');
zlabel('z (meters)');
contourslice(X,Y,Z,V,[0],[0],depthticks);
axis equal;
view(3);
axis vis3d;
colorbar;
print('-r600','-depsc',filename);
