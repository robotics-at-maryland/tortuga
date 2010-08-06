function plotStuff(scale,channelSelect,A,B)

global data;

close all;

file=fopen('dataweird.bin','rb'); allChannels=fread(file,Inf,'int16');fclose(file);

channel1=allChannels(1:4:size(allChannels,1));
channel2=allChannels(2:4:size(allChannels,1));
channel3=allChannels(3:4:size(allChannels,1));
channel4=allChannels(4:4:size(allChannels,1));

colors=['c','m','b','k'];

figure(1);
hold off
if(channelSelect(1)==1)
	plot(channel1*scale,colors(1))
	hold on
end
if(channelSelect(2)==1)
	plot(channel2*scale,colors(2))
	hold on
end
if(channelSelect(3)==1)
	plot(channel3*scale,colors(3))
	hold on
end
if(channelSelect(4)==1)
	plot(channel4*scale,colors(4))
	hold on
end

clear channel1
clear channel2
clear channel3
clear channel4
clear allChannels

		%name=['r0.' num2str(i) '.spec'];
		%disp(name)
                name='spectro_weird.bin';
		spec = fopen(name,'rb');
		specdata=fread(spec,Inf,'int32');
		fclose(spec);

figure(2);
hold off;
data=zeros(4,length(specdata)/4);

for k=1:length(specdata)/4
    for i = 0:3
        data(i+1,k)=specdata((k-1)*4+i+1);
    end
end

size(data)

for i = 0:3
    plot(data(i+1,:),colors(i+1));
    hold on;
end
		%calculation=myIIR(myIIR(data(27,:),B,A),B,A);
		%plot(calculation,colors(i+1))
		%plot(diff(calculation),colors(i+1))
		%hold on
		%clear spec
		%clear data
