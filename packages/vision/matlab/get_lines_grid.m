function [lines]  = get_lines_grid(im, subSize, params)
% GET_LINES_GRID Like get_lines but works on the image in subSize
% chunks, this appears to reduce the noise if you have an image
% with tons and tones of small edges.
%
% im - the greyscale image array to work on
% subSize - the number of divions for each dimension, ie. 4 = 16 tiles
% params - a matlab struct which takes all the options arguments for get_lines
%          a the keys
%
% NOTE: this might only be needed because I was not able to proper
% tune the detectors
%
% Copyright (C) 2009 Joseph Lisee <jlisee@cmu.edu>
    
% Generate the lines by taking smaller sub size hough transforms to
% reduce noise
imWidth = size(im,2);
imHeight = size(im,1);
xIndices = partitionIndices(floor(imWidth/subSize), imWidth);
yIndices = partitionIndices(floor(imHeight/subSize), imHeight);

% Repeat the indicies such that we have a list of all the possible combinations
B = ones(size(xIndices,1),1)*size(xIndices,1);
h=[];
h(cumsum(B))=1;
C=yIndices(cumsum(h)-h+1,:);
indices = [repmat(xIndices, size(xIndices,1),1), C];

% Allocation for results
results = cell(size(indices,1),3);

for i = 1:size(indices,1)
    % Grab our bounds
    minX = indices(i,1);
    maxX = indices(i,2);
    minY = indices(i,3);
    maxY = indices(i,4);

    % Sub IM
    subIm = im(minY:maxY,minX:maxX);
     
    if max(max(subIm)) ~= min(min(subIm))
        % Get lines from sub image
        subLines  = get_lines(subIm, ...
                              'Sigma', params.GA_SIGMA, 'RhoRes', ...
                              params.HOUGH_RHO_RES, ...
                              'ThetaRes', params.HOUGH_THETA_RES, ...
                              'FillGap', params.LINE_FILL_GAP, ... 
                              'Peaks', params.HOUGH_PEAKS, ...
                              'MinLength', params.LINE_MIN_LENGTH, ...
                              'EdgeType', params.EDGE_TYPE, ...
                              'EdgeP1', params.EDGE_P1, 'EdgeP2', ...
                              params.EDGE_P2);
    else
        subLines = struct([]);
    end
    

    
    % Store lines and keep track of how many we have
    results{i,1} = subLines;
    results{i,2} = minX;
    results{i,3} = minY;
end

% Combine all the lines into one
lines = struct([]);
for i = 1:size(results,1)
    % Define our offset
    xOffset = results{i,2};
    yOffset = results{i,3};
    offset = [xOffset yOffset];
    
    % Grab our lines and then offset them
    curLines = results{i,1};

    % Now shift the lines
    for j = 1:length(curLines)
        curLines(j).point1 = curLines(j).point1 + offset;
        curLines(j).point2 = curLines(j).point2 + offset;
    end
    
    lines = [curLines lines];
end


% Link up lines that are end to end
lines = join_lines(lines, params);