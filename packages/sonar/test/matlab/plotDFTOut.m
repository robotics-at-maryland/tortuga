function plotDFTOut(fname, kBands)
% Display a spectrogram of the given file named fname.
% The optional kBands parameter selects the (C-style indexed) Fourier
% bins to plot.
% If passed a single kBand, this will generate a plot instead of a
% heatmap.

  % Read in the data
  fid = fopen(fname);
  dat = fread(fid, Inf, 'int32');
  fclose(fid);
  N = 512;
  M = length(dat)/N;
  dat = reshape(dat, N, M);
  
  % Select all kBands if none are specified
  if nargin == 1
    kBands = 0:(N-1);
  end
  
  % Calculate axes
  fs = 500000;
  Ts = 1/fs;
  T = 1:M;
  F = kBands/N*fs;
  
  % Plot
  if length(kBands) == 1
    plot(T, dat(1+kBands(1),:));
    xlabel('Sample index', 'FontSize', 14);
    ylabel('Intensity', 'FontSize', 14);
    title(sprintf('DFT at %.01f kHz', F(1)));
  else
    imagesc(T, F/1000, dat(1+kBands,:));
    xlabel('Sample index','FontSize',14);
    ylabel('Frequency (kHz)','FontSize',14);
    title('Spectrogram', ...
          'FontSize',14);
    colorbar;
  end
end
