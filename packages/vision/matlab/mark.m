function A = mark( IMG, COORD )
%MARK Uses the given coordinates to mark
%     green crosses on a new image.
%  Use this to help analyze an algorithm to see if it gave
%  the correct blob dimensions.
%  IMG = RGB image
%  COORD = [num_pixels top_width bottom_width top_height top_width]
%  Number of pixels is ignored in this function, but is expected to
%  be there for the convenience of the blobdetector function.

width = COORD(2:3);
height = COORD(4:5);

midW = sum(width) / 2;
midH = sum(height) / 2;

A = IMG;
A = mark_helper(A, [int32(height(1)) midW]); % Top
A = mark_helper(A, [int32(height(2)) midW]); % Bottom
A = mark_helper(A, [midH int32(width(1))]); % Left
A = mark_helper(A, [midH int32(width(2))]); % Right

end

function RET = mark_helper( IMG, LOC )
%MARK_HELPER Marks the green cross on the image at LOC
%    LOC = [row col]

RET = IMG;
SIZE = size(IMG);

height = SIZE(1);
width = SIZE(2);

% Start and end points for the cross with error correction
startr = max(0, LOC(1) - 5);
endr = min(height, LOC(1) + 5);

startc = max(0, LOC(2) - 5);
endc = min(width, LOC(2) + 5);

for row = startr:endr
    for col = startc:endc
        RET(row, col, :) = [0 255 0];
    end
end

end
