

N = 256
sample_f = 30e4



k_values = round(25000*N/sample_f) : round(35000*N/sample_f)

f = fopen('adc.dat', 'rb');
dat = fread(f, [4, 30000], 'int8');
fclose(f);
clf;

t = 0:1/sample_f:(length(dat)-1)/sample_f;

result = zeros(length(k_values),length(t));


    for j = 1:length(k_values)
        % test with k values
        disp(k_values(j))
        w = exp(2*pi*i*k_values(j)/N);
        result(j,:) = slidingDFT(dat(1,:),w,N);
     


    end


close all
plot(t,result)
xlabel('Time')
ylabel('Filter output')

q=sprintf('k = %2d',k_values(1));
for s=2:length(k_values) 
    q=[q;sprintf('k = %2d',k_values(s))]; 
end
legend(q)

