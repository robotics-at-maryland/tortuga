% IMG = image object (make one using imread(filename))
% R = [lower, upper]
% G = [lower, upper]
% B = [lower, upper]

function A = colorfilter(IMG, R, G, B)
% Load the variables
lowerRed = R(1);
upperRed = R(2);
lowerGreen = G(1);
upperGreen = G(2);
lowerBlue = B(1);
upperBlue = B(2);
% Load the size of the image, depth is not used

redFilter = (IMG(:,:,1) <= upperRed) & (IMG(:,:,1) >= lowerRed);
greenFilter = (IMG(:,:,2) <= upperGreen) & (IMG(:,:,2) >= lowerGreen);
blueFilter = (IMG(:,:,3) <= upperBlue) & (IMG(:,:,3) >= lowerBlue);

A = redFilter & greenFilter & blueFilter;

end