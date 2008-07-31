function [A, B, C, D] = LoadBinary(fname)
% Load binary sonar data and load the four channels into arrays
fid = fopen(fname, 'rb');
data = fread(fid, Inf, 'int16');
fclose(fid);
A = data(1:4:end);
B = data(2:4:end);
C = data(3:4:end);
D = data(4:4:end);
end