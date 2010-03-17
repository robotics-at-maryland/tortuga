function lines = edgedetector( I, debug )
%EDGEDETECTOR Filter to find edges
%
%   Uses a standard hough transformation to find lines within an
%   intensity image. If debug is true, then it shows helpful graphs
%   for the calculations.
%
%   Returns up to 5 lines showing the edges in the intensity image.

if nargin > 2
    error('edgedetector takes a maximum of 2 arguments');
end

if nargin == 1
    debug = 0;
end

% Canny edge detector
BW = edge(I, 'canny', .15);
if debug
    figure('Name', 'Canny Edge Detection'), imshow(BW);
end

% Standard hough transform
[H,T,R] = hough(BW, 'ThetaResolution', 1, 'RhoResolution', 1);
P = houghpeaks(H, 5, 'threshold', ceil(0.3*max(H(:))));
if debug
    figure('Name', 'Hough Transform'), imshow(H,[],'XData',T,'YData',R,...
                                             'InitialMagnification','fit');
    xlabel('\theta'), ylabel('\rho');
    axis on, axis normal, hold on
    x = T(P(:,2)); y = R(P(:,1));
    plot(x,y,'s','color','red');
end

% Detect lines
lines = houghlines(BW, T, R, P);

if debug
    % Display the original image
    figure('Name', 'Hough Lines'), imshow(I);
    axis off, axis normal, hold on
    
    % Draw lines on the image
    for i=1:size(lines(:))
        xy = [lines(i).point1; lines(i).point2];
        plot(xy(:,1), xy(:,2), 'LineWidth', 2, 'Color', 'green');
    end
end

end