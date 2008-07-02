function LocalizeManual(Range,SamplingRate)

M = [0,.984,0;.492,.492,.696;-.492,.492,.696];
M = inv(M); % Note: be sure to put this in Sonar Cookbook!

freq = 20000;

if( nargin < 2 )
    SamplingRate = 500000;
end

samplesPerWavelength = SamplingRate / freq;
samplesPerHalfWavelength = samplesPerWavelength / 2;



disp('Loading samples...');

%A = textread('/tmp/dataA.txt');
%B = textread('/tmp/dataB.txt');
%C = textread('/tmp/dataC.txt');
%D = textread('/tmp/dataD.txt');
file = fopen('r0.bin','rb');
data = fread(file, Inf, 'int16');
fclose(file);
A = data(1:4:end);
B = data(2:4:end);
C = data(3:4:end);
D = data(4:4:end);
t = (0:1/SamplingRate:(length(A)-1)/SamplingRate);



if( nargin < 1 )
    Range = 1:length(t);
end


% zoom in on ping
mousePts = [Range(1),0;Range(length(t)),0];
while length(mousePts) > 0
    rangeMin = floor(min(mousePts(1,1),mousePts(2,1)));
    rangeMax = floor(max(mousePts(1,1),mousePts(2,1)));
    Range = rangeMin:rangeMax;
    plot( ...
        Range,A(Range),'c-', ...
        Range,B(Range),'m-', ...
        Range,C(Range),'b-', ...
        Range,D(Range),'k-');
    drawnow;
    disp('Pick two points to zoom in, or press enter to proceed with selection');
    mousePts = ginput(2);
end
plot(Range,A(Range),'c-',Range,B(Range),'m-',Range,C(Range),'b-',Range,D(Range),'k-');
drawnow;

disp('Pick four points at the peaks.');
mousePts = ginput(4);

peaks = mousePts(1:4,1);
peaks = peaks - peaks(1);
peaks = peaks(2:4);
peaks = mod(peaks, samplesPerWavelength)-samplesPerHalfWavelength;

direction = M * peaks;
direction = direction/norm(direction);

disp('Peaks at:');
disp(peaks);

disp('Ping at:');
disp(direction);

disp('Earth azimuth:');
disp(180/pi*atan2(direction(2),direction(1)));
end
