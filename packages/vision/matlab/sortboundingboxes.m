function list = sortboundingboxes( list, mode )
%SORTBOUNDINGBOXES Sorts bounding boxes based on the mode
%    Sorts bounding boxes in ascending or descending order based on
%    the mode. Done using bubble sort. Why? Because I'm not writing
%    quick sort in matlab to sort less than 10 elements. I'll use a
%    faster sort when this gets ported to C++, which has comparators.

if nargin < 2
    mode = 'ascend';
end

for i=1:size(list(:))
    for j=i:size(list(:))
        swap = 0;
        if strcmp(mode, 'ascend') && (area(list(j)) < area(list(i)))
            swap = 1;
        elseif strcmp(mode, 'descend') && (area(list(i)) < ...
                                           area(list(j)))
            swap = 1;
        end
        
        if swap
            temp = list(j);
            list(j) = list(i);
            list(i) = temp;
        end
    end
end

end

function A = area( box )
% Helper function to get the area of a box
A = (box.maxX - box.minX) * (box.maxY - box.minY);
end
