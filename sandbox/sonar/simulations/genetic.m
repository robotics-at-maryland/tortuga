function hydro_actual = genetic
config;
Format = .05;
SUM_NEW = 0;
SUM_OLD = 0;
test = 0;
test1 = 1;
y0 = rand()*Format*.1; z0 = rand()*Format*.1;
y1 = y0; z1 = z0;
hydro_old =  [rand()*Format, 0, rand()*Format-z0;
              rand()*Format, 0.20955, rand()*Format-z0;
	          rand()*Format,rand()*Format-y0,0; 
	          rand()*Format,rand()*Format-y0,0.20955;]
hydro_actual = hydro_old;
hydro_actual(1,3) = hydro_old(1,3) + z0;
hydro_actual(2,3) = hydro_old(2,3) + z0;
hydro_actual(3,2) = hydro_old(3,2) + y0;
hydro_actual(4,2) = hydro_old(4,2) + y0;
hydro_actual
hydro_new = hydro_old;
z0 = 0; y0 =0;
iterations = 200;
save_matrix_test = 1;
save_num = 1;
%the save_matrix stores not only hydro_actual, but also the implicit (0,0,0)
%hydrophone's position and the corresponding fitness level
%row 1 contains the fitness parameter
%row 2 the position of the implicit hydrophone
%rows 3-6 contain hydro_actual
save_matrix = zeros(6,3,iterations); 
	      
for j = -2.5:.5:2.5
    for k = -100:40:100
        for d = -50:20:50
             SUM_OLD = SUM_OLD + range_error([j k d], hydro_old, hydro_pos_accuracy, tdoa_accuracy);
        end
    end
end
SUM_NEW = SUM_OLD
for i = 1:iterations
    if(SUM_NEW < SUM_OLD)
       hydro_old = hydro_new;
        SUM_OLD = SUM_NEW;
        save_num = save_num +1;
        %modifies the untrimmed save_matrix; allows you adjust how often
        %you modify the save_matrix
        if( mod(save_num,10) == 0)
        save_matrix(3:6,:, save_num) = save_matrix(3:6,:,save_num) + hydro_actual;
        save_matrix(2,2,save_num) = save_matrix(2,2,save_num) + y0;
        save_matrix(2,3,save_num) = save_matrix(2,3,save_num) + z0;
        save_matrix(1,1,save_num) = save_matrix(1,1,save_num) + SUM_OLD;
        end   
    end
    [hydro_new y0 z0] = evolve(hydro_old, y0, z0, Format);
    SUM_NEW = 0;
    for j = -2.5:.5:2.5
        for k = -100:40:100
            for d = -50:20:50
                SUM_NEW = SUM_NEW + range_error([j k d], hydro_new, hydro_pos_accuracy, tdoa_accuracy );
            end
        end
    end
end
    
y0
z0
SUM_NEW
test2 = iterations;
MATRIX_SUM = 0;
%removes any unnecessary matrices from the save_matrix array
while( save_matrix_test <= test2)
    MATRIX_SUM = sum(sum(save_matrix(:,:,save_matrix_test)));
    if( MATRIX_SUM == 0)
        save_matrix(:,:,save_matrix_test) = [];
        test2 = test2 - 1;
    else
        save_matrix_test = save_matrix_test + 1;
    end
end
save('Results', 'save_matrix');
