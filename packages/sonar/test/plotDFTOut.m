function plotDFTOut(fname, kBands)
% Display a spectrogram of the given file named fname.
% The optional kBands parameter selects the (C-style indexed) Fourier
% bins to plot.
  fid = fopen(fname);
  dat = fread(fid, Inf, 'int32');
  fclose(fid);
  N = 512;
  M = length(dat)/N;
  dat = reshape(dat, N, M);
  
  if nargin == 0
    kBands = 0:(N-1);
  end

  fs = 500000;
  Ts = 1/fs;
  T = 1:M;
  F = kBands/N*fs;
  imagesc(T, F/1000, dat(1+kBands,:));
  xlabel('Sample index','FontSize',14);
  ylabel('Frequency (kHz)','FontSize',14);
  title('Spectrogram', ...
        'FontSize',14);
  colorbar;
end
