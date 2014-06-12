function REGIONS = regiondetector( IMG, R, G, B )
%REGIONDETECTOR Find regions of interest
%   RELIES ON: colorfilter.m
%
%   This function will use a color filter to find any regions of
%   interest.

% Use a color filter on the image
A = colorfilter(IMG, R, G, B);

% Erode the image to remove noise
A = imerode(A, strel('square', 2));

% Intensify image based on surroundings
A = imfilter(A, [1 1 1;1 0 1;1 1 1]);

% Find regions
REGIONS = [];

[height, width] = size(A);

% Black out top row and left row
A(1,:) = zeros(1,width);
A(:,1) = zeros(height,1);

% TODO: Make this work
for i=2:height
    for y=2:width
        % Check the top and left pixel
        if (A(i-1, y) == 0) && (A(i, y-1) == 0)
            % New region
            %regSize = [y i; y i];
            %REGIONS = [REGIONS 0];
        else
            % There is a region that exists, find it
            %for j=1:size(REGIONS)
            %    if in_region([y i], REGIONS(j))
            %        dims = REGIONS(j);
                    % Extends height or width as necessary
            %        if dims(2,1) < y
            %            dims(2,1) = y;
            %        end
            %        if dims(2,2) < i
            %            dims(2,2) = i;
            %        end
            %        REGIONS(j) = dims;
            %    end
            %end
        end
    end
end

end

function BOOL = in_region(val, region)
    % Double check that val and region are the correct sizes
    % TODO: I have no idea how to do this
    
    % Check if x region is in the width
    xreg_bool = (region(1,2)-1) <= val(1) & (region(2,2)+1) >= val(1);
    yreg_bool = (region(1,1)-1) <= val(2) & (region(2,1)+1) >= val(2);
    BOOL = xreg_bool & yreg_bool;
end