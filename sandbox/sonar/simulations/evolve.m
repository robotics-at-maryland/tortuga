function [hydro_new, y1, z1] = evolve(hydro_old, y0, z0, Format)
isReady = false;
test1 = 1;
test_matrix = [1.016,0,0.20955; 1.016,0.20955,0.20955; 1.016,0.20955,0; 1.016,0.20955,0.20955]; 
while not(isReady)
    while(test1)
        y1 = y0 + (rand()-.2)*Format;
        z1 = z0 + (rand()-.2)*Format;
        if( ~(y1 > 3 | z1 > 3))
            test1 = false;
        end
    end
    hydro_up =  [(rand()-.2)*Format, 0,(rand()-.2)*Format-(z1-z0);
        (rand()-.2)*Format, 0,(rand()-.2)*Format-(z1-z0);
        (rand()-.2)*Format,(rand()-.2)*Format-(y1-y0),0;
        (rand()-.2)*Format,(rand()-.2)*Format-(y1-y0),0;];
    hydro_new = hydro_old + hydro_up;
    hydro_actual = hydro_new;
    hydro_actual(1,3) = hydro_new(1,3) + (z1-z0);
    hydro_actual(2,3) = hydro_new(2,3) + (z1-z0);
    hydro_actual(3,2) = hydro_new(3,2) + (y1-y0);
    hydro_actual(4,2) = hydro_new(4,2) + (y1-y0);
    test = (sum(sum(hydro_actual > test_matrix)) + sum(sum(hydro_actual < zeros(4,3))));

    %disabled isReady = isWellPacked(hydro_new, lambda);
    if( test > 0)
        isReady = false;
    else
        isReady = true;
    end
end

