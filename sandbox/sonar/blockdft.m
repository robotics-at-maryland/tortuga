function blockdft(fname)
fid=fopen(fname,'r');
if fid == -1
  disp('Failed to open file');
  return;
end
dat = fread(fid,[4,Inf],'int16');
fclose(fid);

sampRate=500000;
blockSize=256;
blockCount=floor(length(dat)/blockSize);
triggered=0;
for blockNum=1:blockCount
  blockStartIndex = (blockNum-1)*blockSize+1;
  blockStopIndex = min(length(dat), blockNum*blockSize);
  block = dat(:,blockStartIndex:blockStopIndex);
  Range = 1:length(block);
  dft = abs(fft(block,[],2)).^2;
  dft = dft./repmat(sum(dft,2),1,size(dft,2));
  dft = dft(:,10:20);
  [dummy,dftMax] = max(dft,[],2);
  sameMax = all(dftMax == dftMax(1));
  dftMax = dftMax(1);
  if max(max(dft))>.2 && sameMax && !triggered
    triggered = 1;
    disp(sprintf("Signal at %f kHz",sampRate/blockSize*(dftMax+10)/1000));
    figure(1);
    for i=1:size(dat,1)
      plot(dft(i,:));
      hold on;
    end
    hold off;
    figure(2);
    contextBefore = 10*blockSize;
    contextAfter = 10*blockSize;
    contextBeforeStart = blockStartIndex-contextBefore;
    contextAfterStop = blockStopIndex+contextAfter+1;
    if contextBeforeStart <= 0
      contextAfterStop += -contextBeforeStart+1;
      contextBeforeStart = 1;
    elseif contextAfterStop > length(dat)
      contextBeforeStart -= contextAfterStop-length(dat);
      contextAfterStop = length(dat);
    end
    skip = 32;
    contextBeforeRange=contextBeforeStart:skip:(blockStartIndex-1);
    contextAfterRange=(blockStopIndex+1):skip:contextAfterStop;
    blockRange=blockStartIndex:skip:blockStopIndex;
    for i=1:size(dat,1)
      plot(blockRange,dat(i,blockRange),'b');
      hold on;
      if length(contextBeforeRange)>0
	plot(contextBeforeRange,dat(i,contextBeforeRange),'c');
      end
      if length(contextAfterRange)>0
	plot(contextAfterRange,dat(i,contextAfterRange),'c');
      end
    end
    hold off;
    axis('auto');
    drawnow;
    pause;
  elseif max(max(dft))<.001
    triggered = 0;
  end
end
end
