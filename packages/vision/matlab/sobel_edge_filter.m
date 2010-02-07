function [ FILTER ] = sobel_edge_filter(IMG)
% This accepts any colored image
% converts it into grayscale, and
% spits out an iamge using the sobel
% filter that accentuates both vertical
% and horizontal edges

GRAY = rgb2gray(IMG);

% Perform a gaussian filter on the image first

GRAY = imfilter(GRAY, fspecial('gaussian', 5));

% Image must be a double before using the sobel filter, or we lose
% half of the edges

D = double(GRAY);

F1 = imfilter(D, fspecial('sobel'));
F2 = imfilter(D, fspecial('sobel')');

% Absolute value so negative values are emphasized
modF1 = abs(F1);
modF2 = abs(F2);

% Scale image so the max is 256
max_value = max(max(max(F1)), max(max(F2)));
multiple = max_value / 256;
scaledF1 = modF1 ./ multiple;
scaledF2 = modF2 ./ multiple;

FILTER = max(uint8(scaledF1), uint8(scaledF2));


end