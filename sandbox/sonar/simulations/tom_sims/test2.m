

N = 256
sample_f = 5e5


f_axis = 20000:100:30000;

k_values = round(min(f_axis)*N/sample_f) : round(max(f_axis)*N/sample_f)
k_axis = zeros(length(k_values),length(f_axis));


for m = 1:length(f_axis)

    % generate test pulses of many frequencies
    zzz = sprintf('Analyzing pulse frequency of %d...',f_axis(m));
    disp(zzz);

    [t,x] = generatePulse(1/sample_f, f_axis(m));


    for j = 1:length(k_values)
        % test with k values

        w = exp(2*pi*i*k_values(j)/N);
        bigX = slidingDFT(x,w,N);
        k_axis(j,m) = max(bigX);


    end


end

close all
plot(f_axis,k_axis)
xlabel('Ping frequency')
ylabel('Detection peak value')

q=sprintf('k = %2d',k_values(1));
for s=2:length(k_values) 
    q=[q;sprintf('k = %2d',k_values(s))]; 
end
legend(q)









%X = [X X X];
%t = [0:sample_time:(length(X)-1)*sample_time];
%N = 256
%%f_detect = 25800
% k_detect = 14 %round(f_detect*N*sample_time)
% f_detect_actual = k_detect/N/sample_time
% w_detect = exp(2*pi*i*k_detect/N);
% [K] = slidingDFT(X,w_detect,N);
%
% close all
% plot(t,K,'-')
