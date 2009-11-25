function [ BLOBS ] = histogram( IMG )
%HISTOGRAM A MATLAB version of the histogram function.
%           This comes from the BlobDetector file.
%   Description to go here once I figure out how this works
%   Only accepts color images for now

% Make a copy of the array that is 640 x 480
A = imresize(IMG, [480 640]);
data = zeros(480, 640);

% Black out top row and col
for i = 1:640
    A(1, i, :) = [0 0 0];
    data(1, i) = 0;
end
for j = 1:480
    A(j, 1, :) = [0 0 0];
    data(j, 1) = 0;
end

for y = 2:480
    for x = 2:640
        % If the pixel isn't black
        if sum(A(y, x)) > 0
            % Found a valid pixel, check the value above and to the left
            above = data(y-1, x);
            left = data(y, x-1);
            if (above == 0 && left == 0)
                % Start of a new blob
            else
                above2 = above;
                left2 = left;
                %
            end
        end
    end
end

BLOBS = A;

end

