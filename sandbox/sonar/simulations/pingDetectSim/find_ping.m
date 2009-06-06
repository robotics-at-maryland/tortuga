clear all;
global data;

ping_level=10;

%first, get in the data 
plotStuff(1, [1 1 1 1],1,1);
figure(2);
%axis([2000 10000 0 50000]);

for i=1:4
    [absmax(i),index(i)]=max(data(i,:));
end
index
plot(index,absmax,'*r');

 interval=500;
 %Find the maxima
 colors=['c','m','b','k'];
 figure(3);
 hold off;
 figure(4);
 hold off;
 figure(5);
 hold off;
 clear x;
 clear channel;
 clear i;
 x=interval/2:interval:size(data,2)-interval/2;
 pings=zeros(5);
 for channel=1:4
 num_found=0;
    for i=1:floor(size(data,2)/interval)
        maxval(channel,i)=max(data(channel,(i-1)*interval+1:i*interval));
        if(i==1)
            minmax(channel,i)=maxval(channel,i);
        else
            minmax(channel,i)=min(maxval(channel,i),minmax(channel,(i-1)));
        end
        ratio(channel,i)=maxval(channel,i)./minmax(channel,i);
        if(ratio(channel,i)>ping_level)
            num_found=num_found+1;
            disp('Ping found at x=:')
            disp(i*interval-interval/2)
            pings(num_found)=i;
            minmax(channel,i)=maxval(channel,i); %reset the counter, just to mess with it.  Normally, you would want to wait a little before resetting
        end

    end
    figure(3);
    plot(x,maxval(channel,:),colors(channel));
    hold on;
    figure(4);
    plot(x,minmax(channel,:),colors(channel));
    hold on;
    figure(5);
    plot(x,ratio(channel,:),colors(channel));
    hold on;
    clear j;
    for j=1:num_found
        plot(x(pings(j)),ratio(channel,pings(j)),'*r');
    end
end
