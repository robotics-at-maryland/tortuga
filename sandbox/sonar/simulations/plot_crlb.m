function [X,Y,Z,V] = plot_crlb(filename)
posticks = -100:20:100;
depthticks = -10:4:10;
[X,Y,Z]=meshgrid(posticks,posticks,depthticks);
Z = zeros(length(posticks),length(posticks),length(depthticks));
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

levels = 0.01:0.02:10;
for k = 1:length(depthticks)
    subplot(prows,pcols,k);
    [C,h] = contour(X(:,:,1),Y(:,:,1),V(:,:,k),levels);
    axis square;
    title(sprintf('Depth: %d meters',depthticks(k)));
end
set(h,'ShowText','on','TextStep',2);
