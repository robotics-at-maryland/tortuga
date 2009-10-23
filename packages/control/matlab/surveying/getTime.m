function [ time ] = getTime()
%GETTIME Summary of this function goes here
%   Detailed explanation goes here
c = clock;
time.hours = c(4);
time.minutes = c(5);
time.seconds = int32(c(6));
end

