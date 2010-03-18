function MOD = drawboundingbox( IMG, BOX, DISPLAY, NAME, COLOR )
%DRAWBOUNDINGBOX Draws a box around an RGB image and shows it

if nargin < 5
    COLOR = [0 255 0];
end

if nargin < 4
    NAME = 'Debugging Information';
end
if nargin < 3
    DISPLAY = 0;
end

MOD = IMG;

for i=BOX.minX:BOX.maxX
    MOD(BOX.minY, i, :) = COLOR;
    MOD(BOX.maxY, i, :) = COLOR;
end
for i=BOX.minY:BOX.maxY
    MOD(i, BOX.minX, :) = COLOR;
    MOD(i, BOX.maxX, :) = COLOR;
end

if DISPLAY
    figure('Name', NAME), imshow(MOD);
end

end
