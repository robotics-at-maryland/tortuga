function [t,x] = generatePulse(sample_time,f)

    t = [0:sample_time:.03];
    omega = f*2*pi;
    x = [0];
    for k = 1:length(t)
        if t(k) > .01 && t(k) < .011
            x(k) = sin(omega*t(k));
        else
            x(k) = 0;
        end
    end

end

