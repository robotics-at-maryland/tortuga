function plotDFTOut
  fid = fopen('dft.out');
  dat = fread(fid, Inf, 'int32');
  fclose(fid);
  M = length(dat)/512;
  dat = reshape(dat, 512, M);

  imagesc(dat);
  xlabel('Sample index','FontSize',14);
  ylabel('Frequency bin','FontSize',14);
  title('Anomaly between fixed point sliding DFT and FFTW, N=512', ...
	'FontSize',14);
  colorbar;
end
