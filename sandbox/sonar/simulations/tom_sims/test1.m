

sample_time = 1/500000;
t = [0:sample_time:.03];
f = 25500
omega = f*2*pi;
X = [zeros(1,4000) sin(t(4001:4800)*omega)];
X = [X zeros(1,length(t)-length(X))];
%X = [X X X];
%t = [0:sample_time:(length(X)-1)*sample_time];
N = 256
f_detect = 25800
k_detect = round(f_detect*N*sample_time)
f_detect_actual = k_detect/N/sample_time
w_detect = exp(2*pi*i*k_detect/N);
[K] = slidingDFT(X,w_detect,N);

close all
plot(t,K,'-')
