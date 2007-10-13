function plot_crlb(filename)
posticks = -100:10:100;
depth = 0.1;
[X,Y]=meshgrid(posticks,posticks);
Z = zeros(length(posticks));
load(filename,'hydro_pos');

for i = 1:length(posticks)
    for j = 1:length(posticks)
        Z(i,j) = crlb([posticks(i),posticks(j),depth], hydro_pos);
    end
end

mesh(X,Y,Z);
xlabel('x (meters)');
ylabel('y (meters)');
zlabel('Minimum std. deviation for source position (meters)');
% plot(X(1,:),Z(10,:));