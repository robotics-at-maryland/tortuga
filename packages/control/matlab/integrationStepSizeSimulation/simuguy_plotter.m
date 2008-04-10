clear all
close all
clc

[time_low    x_low]  = simuguy(60);
[time_high   x_high] = simuguy(1000);
[time_high2   x_high2] = simuguy(1001);


subplot(2,1,1)
plot(time_low,x_low(1,:),'b',time_high,x_high(1,:),'g',time_high2,x_high2(1,:),'m')
legend('X_l_o_w _f_r_e_q','X_h_i_g_h A_n_e_w','X_h_i_g_h A_o_l_d')
ylabel('depth')
xlabel('time (sec)')
title('position')

subplot(2,1,2)
plot(time_low,x_low(2,:),'b',time_high,x_high(2,:),'g',time_high2,x_high2(2,:),'m')
legend('X_l_o_w _f_r_e_q','X_h_i_g_h A_n_e_w','X_h_i_g_h A_o_l_d')
ylabel('vel')
xlabel('time (sec)')
title('velocity')