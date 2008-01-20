function K = dft(X,w,N)
    K = 0;
    for a = 1:N
        K = K + X(a)*w^(1-a);
    end
    
end
