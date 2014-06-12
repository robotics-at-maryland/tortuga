snr = logspace(-1,4,10);
freq_sample = logspace(5,7,10);
phase_rez = zeros(length(snr),length(freq_sample));
[X,Y] = meshgrid(snr, freq_sample);

for i=1:length(snr)
  for j=1:length(freq_sample)
    phase_rez(i,j) = phase_resolution(snr(i),freq_sample(j));
  end
end

levels = 10.^(-10:0);


figure(1);
axes('XScale','log');
axes('YScale','log');
%axes('XTick',snr);
%axes('YTick',freq_sample);
%axes('XTickMode','manual');
%axes('YTickMode','manual');
[C,h] = contour(X,Y,phase_rez,levels);
set(h,'ShowText','on');
xlabel('Signal-to-noise ratio (unitless)');
ylabel('Sampling frequency (Hertz)');
