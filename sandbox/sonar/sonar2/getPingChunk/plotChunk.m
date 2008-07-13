function plotChunk(filename)

file=fopen(filename,'rb');

if(file==-1)
    disp('Error! Could not open file');
else
     %starts=fread(file,4,'int32');
     starts=[0,0,0,0];

     allChannels=fread(file,Inf,'int16');
     fclose(file);
     colors=['c','m','b','k'];
     
     %channel=allChannels(1:4:size(allChannels,1));
     %channel2=allChannels(2:4:size(allChannels,1));
     %channel3=allChannels(3:4:size(allChannels,1));
     %channel4=allChannels(4:4:size(allChannels,1));
     length=size(allChannels,1)/4-1;

     for i=1:4
        figure(i);
        hold off;
        clearplot;
        x=starts(i):starts(i)+length;
        plot(x,allChannels(1:4:size(allChannels,1)),colors(i));
    end
 end
