

N = 256;
sample_f = 30e4;

slope_t_delta = 64
trigger_noise = 6.5

k_values = 26

f = fopen('adc.dat', 'rb');
dat = fread(f, [4, 30000], 'int8');
fclose(f);
clf;

t = 0:1/sample_f:(length(dat)-1)/sample_f;

result = zeros(4,length(t));


    for j = 1:4
        % test with k values
        w = exp(2*pi*i*k_values/N);
        result(j,:) = slidingDFT(dat(j,:),w,N);
    end

    
    
    % Calibrate trigger values
    trigger_values = zeros(4,1);
    for j=1:4
        % Get peak slope in noise region
         for m = slope_t_delta+1:3000
             delta(m-slope_t_delta) = abs(result(j,m)-result(j,m-slope_t_delta));
%             trigger_values(j) = max([delta, trigger_values(j)]);
         end
         trigger_values(j) = mean(delta);

    end
    trigger_values;
    trigger_values = trigger_values * trigger_noise
    
    
    trigger = zeros(4,length(t));
    trigger_times = zeros(4,1);
    % Apply triggers
    for j=1:4
        % Test for slope
        for m = slope_t_delta+1:30000
            delta = result(j,m)-result(j,m-slope_t_delta);
            if delta > trigger_values(j)
                trigger(j,m) = 1;
                if trigger_times(j) == 0
                    trigger_times(j) = t(m);
                end
            end
        end
    end
    
    disp(vpa(trigger_times,5))
    
     
    
    
    

close all
plot(t,result)
xlabel('Time')
ylabel('Filter output')

legend('n=1','n=2','n=3','n=4')

figure

plot(t,trigger)
xlabel('Time')
ylabel('Trigger output')

legend('n=1','n=2','n=3','n=4')


