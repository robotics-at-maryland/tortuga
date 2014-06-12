
% X = array of time domain data values
% w = exp(2*pi*i*k/N)
% N = window length

function [K] = slidingDFT( X, w, N )

    if length(X) <= N
        disp('Not enough data points.')
        K = 0;
        return
    end

    % First take the DFT of the first N points to initialize
    Kp = dft(X(1:N),w,N);
    K = [zeros(1,N-1) norm(Kp)];
    Xp = X(1:N);
    % Then loop for remaining points, adding a new DFT value each time
    for a = N+1:length(X)
       Xp = [Xp X(a)];
       Kp = (Kp - Xp(1)+X(a))*w;
       Xp = Xp(2:N+1);
       K = [K norm(Kp)];
       
    end
    
end
