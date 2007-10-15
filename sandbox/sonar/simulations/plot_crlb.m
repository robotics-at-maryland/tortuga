function [X,Y,Z,V] = plot_crlb(filename)
posticks = -30:3:30;
depthticks = -30:20:30;
[X,Y,Z]=meshgrid(posticks,posticks,depthticks);
V = zeros(length(posticks),length(posticks),length(depthticks));
load(filename,'hydro_pos');

for i = 1:length(posticks)
    for j = 1:length(posticks)
        for k = 1:length(depthticks)
           V(i,j,k) = crlb([posticks(i),posticks(j),depthticks(k)], hydro_pos);
        end
    end
end

m = length(depthticks);
prows = floor(sqrt(m));
pcols = ceil(m/prows);

%levels = 0.01:0.02:10;
%for k = 1:length(depthticks)
%    subplot(prows,pcols,k);
%    [C,h] = contour(X(:,:,1),Y(:,:,1),V(:,:,k),levels);
%    axis square;
%    title(sprintf('Depth: %d meters',depthticks(k)));
%end
%set(h,'ShowText','on','TextStep',2);
clf;
title('Projected error bounds for sonar location measurements');
xlabel('x (meters)');
ylabel('y (meters)');
zlabel('z (meters)');
contourslice(X,Y,Z,V,[0],[0],depthticks);
axis square;
colorbar;
