function hydro_new = genetic
lambda = 1400/30000;
SUM_NEW = 0;
SUM_OLD = 0;
hydro_old = rand(4,3)*lambda;
for j = 1:5
    for k = 1:40:200
        for d = -100:40:100
            SUM_OLD = SUM_OLD + crlb([j k d], hydro_old);
        end
    end
end
SUM_NEW = SUM_OLD;
for i = 1:100
    if(SUM_NEW < SUM_OLD)
        hydro_old = hydro_new
        SUM_OLD = SUM_NEW;
    end
    hydro_new = evolve(hydro_old);
    SUM_NEW = 0;
    for j = 1:5
        for k = 1:40:200
            for d = -100:40:100
                SUM_NEW = SUM_NEW + crlb([j k d], hydro_new);
            end
        end
    end
end

    function hydro_new = evolve(hydro_old)
        isReady = false;
        while not(isReady)
            hydro_new = hydro_old + ((rand(4,3) - 0.5)*2*lambda*.1);
            isReady = isWellPacked(hydro_new, lambda);
        end
    end
end
