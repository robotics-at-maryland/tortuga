function hedges = hedgedetector( RGB )
%HEDGEDETECTOR Finds the Hedge in an RGB image
%   Takes an RGB image and finds the hedge. Starts by converting the RGB
%   image to the CIELUV colorspace. Then it filters that image and uses
%   Canny edge detection to filter the image further. Then it applies a
%   basic hough transform and analyzes the resulting lines to find which
%   ones may apply to a hedge.

% Configuration parameters
cfg.filtLMax = 100;
cfg.filtLMin = 0;
cfg.filtUMax = -30;
cfg.filtUMin = -100;
cfg.filtVMax = 100;
cfg.filtVMin = -10;
cfg.minIntensity = .15;

cfg.angleThreshold = 15;
cfg.maxLines = 10;
cfg.edgedebug = 0;
cfg.debug = 0;

% Convert from RGB to CIELUV
LUV = colorspace('rgb->cieluv', RGB);

% Filter the image
I = colorfilter(LUV, cfg.filtLMin, cfg.filtLMax,...
                     cfg.filtUMin, cfg.filtUMax,...
                     cfg.filtVMin, cfg.filtVMax);

% Edge detector (does canny and hough detection)
lines = edgedetector(I, cfg, cfg.edgedebug);

% Merge similar lines?

% Analyze the lines to find the hedge
hedges = [];
for i=1:size(lines(:))-2
    % Check that the first number is a horizontal lines
    if (lines(i).theta <= (-90 + cfg.angleThreshold)) || ...
            (lines(i).theta >= (90 - cfg.angleThreshold))
        % Record the size
        length = distance(lines(i).point1, lines(i).point2);
        
        % Find two potential vertical pipes
        for j=i+1:size(lines(:))-1
            if ((lines(j).theta <= cfg.angleThreshold) && ...
                (lines(j).theta >= -cfg.angleThreshold))
                
                for k=j+1:size(lines(:))
                    if ((lines(k).theta <= cfg.angleThreshold) && ...
                        (lines(k).theta >= -cfg.angleThreshold))
                        
                        % Find the min-max of the hedge pipes lines
                        hedge.minX = 0;
                        hedge.minY = 0;
                        hedge.maxX = 0;
                        hedge.maxY = 0;
                        
                        hedge.minX = min([lines(i).point1(1)
                                          lines(i).point2(1)
                                          lines(j).point1(1)
                                          lines(j).point2(1)
                                          lines(k).point1(1)
                                          lines(k).point2(1)]);
                        hedge.maxX = max([lines(i).point1(1)
                                          lines(i).point2(1)
                                          lines(j).point1(1)
                                          lines(j).point2(1)
                                          lines(k).point1(1)
                                          lines(k).point2(1)]);
                        hedge.minY = min([lines(i).point1(2)
                                          lines(i).point2(2)
                                          lines(j).point1(2)
                                          lines(j).point2(2)
                                          lines(k).point1(2)
                                          lines(k).point2(2)]);
                        hedge.maxY = max([lines(i).point1(2)
                                          lines(i).point2(2)
                                          lines(j).point1(2)
                                          lines(j).point2(2)
                                          lines(k).point1(2)
                                          lines(k).point2(2)]);
                        
                        % Append it to the end of the list
                        hedges = [hedges hedge];
                    end
                end
            end
        end
    end
end

hedges = sortboundingboxes(hedges, 'descend');

end

