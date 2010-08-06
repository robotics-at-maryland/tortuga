function [ BLOBS ] = blobdetector( IMG )
%BLOBDETECTOR A MATLAB version of the blob detector.
%           This only collects one blob (if one exists) and
%           it will screw up badly if multiple blobs exist.
%   I am working on a version that will collect multiple blobs.
%   Works with the perfect blob image.

% Make a copy of the array that is 640 x 480
A = imresize(IMG, [480 640]);
%data = zeros(480, 640, 'uint8');

%%% CONFIGURATION %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
RED = [180 255];
GREEN = [0 20];
BLUE = [0 20];

A = colorfilter(A, RED, GREEN, BLUE);

% Black out top row and col
for i = 1:640
    A(1, i, :) = [0 0 0];
    %data(1, i) = 0;
end
for j = 1:480
    A(j, 1, :) = [0 0 0];
    %data(j, 1) = 0;
end

% Assumes there is only one blob
% [num_pixels top_height bottom_height top_width bottom_width]
blob = zeros(2, 2);
pixels = 0;
% Initialize to invalid values
height = [480 0];
width = [640 0];

for y = 2:480
    for x = 2:640
        % If the pixel isn't black
        if sum(A(y, x)) > 0
            pixels = pixels + 1;
            % Check the position
            if x < width(1)
                width(1) = x;
            end
            if x > width(2)
                width(2) = x;
            end
            
            if y < height(1)
                height(1) = y;
            end
            if y > height(2)
                height(2) = y;
            end
        else
            % Placeholder
        end
    end
end

% Create return type
BLOBS = create_blob(pixels, width, height);

end


function [ BLOB ] = create_blob( pixels, width, height )

BLOB = [pixels width height];

end
