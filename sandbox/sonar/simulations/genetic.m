function hydro_actual = genetic
lambda = 1400/30000;
Format = .01;
SUM_NEW = 0;
SUM_OLD = 0;
test = 0;
y0 = rand()*Format; z0 = rand()*Format;
y1 = y0; z1 = z0;
hydro_old =  [rand()*Format, 0, rand()*Format-z0;
	      rand()*Format, 3, rand()*Format-z0;
	      rand()*Format,rand()*Format-y0,0; 
	      rand()*Format,rand()*Format-y0,3;]
hydro_actual = hydro_old;
hydro_actual(1,3) = hydro_old(1,3) + z0;
hydro_actual(2,3) = hydro_old(2,3) + z0;
hydro_actual(3,2) = hydro_old(3,2) + y0;
hydro_actual(4,2) = hydro_old(4,2) + y0;
hydro_actual
z0 = 0; y0 =0;

test_matrix = [6,0,3; 6,3,3; 6,3,0; 6,3,3]; 
 
	      
for j = -2.5:.5:2.5
    for k = -100:40:100
        for d = -50:20:50
             SUM_OLD = SUM_OLD + range_error([j k d], hydro_old);
        end
    end
end
SUM_NEW = SUM_OLD
for i = 1:3
    if(SUM_NEW < SUM_OLD)
       hydro_old = hydro_new;
        SUM_OLD = SUM_NEW;
    end
    hydro_new = evolve(hydro_old);
    SUM_NEW = 0;
    for j = -2.5:.5:2.5
        for k = -100:40:100
            for d = -50:20:50
                SUM_NEW = SUM_NEW + range_error([j k d], hydro_new);
            end
        end
    end
end

    function hydro_new = evolve(hydro_old)
        isReady = false;
        while not(isReady)
            while(y1 > 3 && z1 > 3)
            y1 = y0 + (rand()-.1)*Format; z1 = z0 + (rand()-.1)*Format;
            end
            hydro_up =  [(rand()-.1)*Format, 0, (rand()-.1)*Format-z1;
	                 (rand()-.1)*Format, 0, (rand()-.1)*Format-z1;
	                 (rand()-.1)*Format,(rand()-.1)*Format-y1,0; 
			 (rand()-.1)*Format,(rand()-.1)*Format-y1,0;];
            hydro_new = hydro_old + hydro_up;
            hydro_actual = hydro_new;
            hydro_actual(1,3) = hydro_new(1,3) + (z1 -z0);
            hydro_actual(2,3) = hydro_new(2,3) + (z1 - z0);
            hydro_actual(3,2) = hydro_new(3,2) + (y1 -y0);
            hydro_actual(4,2) = hydro_new(4,2) + (y1- y0);
test = (sum(sum(hydro_actual > test_matrix)) + sum(sum(hydro_actual < zeros(4,3))));
	    %isReady = isWellPacked(hydro_new, lambda);
	    if( test > 0)
               isReady = false;
	    else
	       isReady = true;
            end
        end
	    y0 = y1; z0 = z1;
    end
y0
z0
SUM_NEW
end

