num_tests=10;
max_length=20;
xnoise=3;
ynoise=2;
x_step=3;
length=ceil(rand*max_length/2) * 2 + 1;

for i=1:num_tests
    ycen(i)=ceil(rand*2*max_length)-max_length;
    start(i)=ycen(i)-length+ceil(rand*length);
    ampl(i)=-ceil(rand*10);
end

%generate the parabolas and plot them
figure(1);
clearplot;
hold on;
for i=1:num_tests
    for j=1:length
        x(i,j)=start(i)+(j-1)*x_step+round(xnoise/2+rand*xnoise);
        y(i,j)=ampl(i)*(x(i,j)-ycen(i))^2;
    end
    plot(x(i,:),y(i,:));
    [p(i,:),s]=polyfit(x(i,:),y(i,:),2);
end
