clear

%filterSequenceTester
time=linspace(0,20,100);
%sequence=sin(time)+4*(rand-0.5);
sequence=zeros(1,length(time));
for i=1:length(time)
    sequence(i)=4*sin(time(i))+4*(rand-0.5);
end

filteredSequence=filterSequence(sequence,10);

plot(time,sequence,time,filteredSequence,':');
legend('raw','filtered')