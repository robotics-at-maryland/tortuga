function [ FILTER ] = sobel_edge_filter(IMG)
% This accepts any colored image
% converts it into grayscale, and
% spits out an iamge using the sobel
% filter that accentuates both vertical
% and horizontal edges

GRAY = rgb2gray(IMG);

F1 = imfilter(GRAY, fspecial('sobel'));
F2 = imfilter(GRAY, fspecial('sobel')');

[height, width] = size(GRAY);

% Create a zeroed out image
FILTER = zeros(height, width, 'uint8');
for i=1:height
    for y=1:width
        FILTER(i,y) = max(F1(i,y),F2(i,y));
    end
end

end