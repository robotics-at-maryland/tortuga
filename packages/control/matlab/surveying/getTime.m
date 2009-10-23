function [ time ] = getTime()
%GETTIME Summary of this function goes here
%   Detailed explanation goes here
c = clock;
t.hours = c(4);
t.minutes = c(5);
t.seconds = c(6);

time = strcat(num2str(t.hours),':',num2str(t.minutes),':',num2str(t.seconds));
end

