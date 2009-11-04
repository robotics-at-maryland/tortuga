% IMG = image object (make one using imread(filename))
% R = [lower, upper]
% G = [lower, upper]
% B = [lower, upper]

function A = colorfilter(IMG, R, G, B)
% Load the variables
A = [];
lowerRed = R(1);
upperRed = R(2);
lowerGreen = G(1);
upperGreen = G(2);
lowerBlue = B(1);
upperBlue = B(2);
% Load the size of the image, depth is not used
[height, width, depth] = size(IMG);
% Horribly inefficient embedded for loop
for I = 1:height
    for Y = 1:width
        red = IMG(I, Y, 1);
        green = IMG(I, Y, 2);
        blue = IMG(I, Y, 3);
        % Check if the current RGB values of this pixel are in the restrictions
        if lowerRed <= red && red <= upperRed...
                && lowerGreen <= green...
                && green <= upperGreen...
                && lowerBlue <= blue...
                && blue <= upperBlue
            % If they are, sets the pixel to white, otherwise, black
            A(I, Y, 1:3) = [255 255 255];
        else
            A(I, Y, 1:3) = [0 0 0];
        end
    end
end
end
