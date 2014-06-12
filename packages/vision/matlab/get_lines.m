function [lines, BW, blurIm, H, T, R, P]  = get_lines(im, varargin)
% GET_LINES returns the line segments found by the hough transform
%
% im - the greyscale image array to work on
%
% Lots of optional arguments to tweak the following functions: 
%   fspecial - Sigma (gaussian blur level)
%   edge - EdgeType, EdgeP1, EdgeP2
%   hough - RhoRes, ThetaRes
%   houghpeaks - Peaks
%   houghlines - FillGap, MinLength
%
% Copyright (C) 2009 Joseph Lisee <jlisee@cmu.edu>

        
% Define input arguments    
p = inputParser;
p.addOptional('Sigma', 5);
p.addOptional('RhoRes', 0.5);
p.addOptional('ThetaRes', 0.5);
p.addOptional('Peaks', 100);
p.addOptional('FillGap', 100);
p.addOptional('MinLength', 7);
p.addOptional('EdgeType', 'prewitt');
p.addOptional('EdgeP1', 0.5);
p.addOptional('EdgeP2', 1);

% Parse
p.parse(varargin{:});
args = p.Results;

% Blur image
sigma = args.Sigma;
if sigma > 0
    gfilt = fspecial('gaussian', [sigma sigma] * 6, sigma);
    blurIm = imfilter(im, gfilt, 'replicate');
else
    blurIm = im;
end    


% Get the edge version of the image for hough
BW = edge(blurIm, args.EdgeType, args.EdgeP1, args.EdgeP2); % extract edges

% Get the hough tranform so we can find its peaks
[H,T,R] = hough(BW,'RhoResolution', args.RhoRes, 'ThetaResolution', ...
                args.ThetaRes);

% Get the "peaks" of the transform which are the lines in the image
P  = houghpeaks(H, args.Peaks);

% Grab all the lines
lines = houghlines(BW,T,R,P, 'FillGap', args.FillGap, 'MinLength', ...
                   args.MinLength);
    
end