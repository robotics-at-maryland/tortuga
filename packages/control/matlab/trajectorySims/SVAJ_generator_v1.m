function [s,v] = SVAJ_generator(deltat, act_depth,des_depth,vel_max)

%deltat = sensor update rate (this will be a constant)
%act_depth = the depth we are currently at 
%des_depth = the depth we want to achieve 

change_in_depth = des_depth - act_depth;


%total_time = initially this will be defaulted to 20 seconds
    %but later on we will get this to calculate itself based off of velocity?

%We want to output an array of solutions for the equations for s,v which has calculated gains



%Based off of the change_in_depth we will want to change the # of data
%points we take to get to the final position.




%using the max reasonable vel = 1 meter/second
%solve using v(total_time/2) = vel_max


total_time = 35/16*change_in_depth/vel_max


%vel_max = (change_in_depth)*  (35*4*(1/total_time)*(current_time/total_time)^3  -84*5*(1/total_time)*(current_time/total_time)^4  +70*6*(1/total_time)*(current_time/total_time)^5 -20*7*(1/total_time)*(current_time/total_time)^6);
%----------------------------------------------------%
%[total_time2 current_time2] = solve('vel_max = (change_in_depth)*  (35*4*(1/total_time)*(current_time/total_time)^3  -84*5*(1/total_time)*(current_time/total_time)^4  +70*6*(1/total_time)*(current_time/total_time)^5 -20*7*(1/total_time)*(current_time/total_time)^6)','current_time = total_time/2','total_time','current_time')
%----------------------------------------------------%


 [Cp Cq Cr Cs] = svaj_coef_generator(4,5,6,7);


step = 0;
for current_time=0:deltat:total_time
step=step+1;
s(step) = (change_in_depth)*  (Cp*               (current_time/total_time)^4  +Cq*               (current_time/total_time)^5  +Cr*               (current_time/total_time)^6 +Cs*               (current_time/total_time)^7) + act_depth;
v(step) = (change_in_depth)*  (Cp*4*(1/total_time)*(current_time/total_time)^3  +Cq*5*(1/total_time)*(current_time/total_time)^4  +Cr*6*(1/total_time)*(current_time/total_time)^5 +Cs*7*(1/total_time)*(current_time/total_time)^6);
end

%have it analyze the maximum velocity and keep it within constraints
%otherwise redevelop trajectory




