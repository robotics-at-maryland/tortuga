function [ C FP N ] = analyze_images( dir_name, func_name )
%ANALYZE_IMAGES Will iterate over a directory and analyze it using
%                  a named algorithm
%   Iterates over every image in the directory with the indicated file_type
%   and performs and processes the image using the specified algorithm.
%   Then it checks the results with the named results in the corresponding
%   txt file.
%   NOTE: This assumes the folders stock, processed, and desired exist
%         in the folder specified.

% Directory contents
filelist = dir([dir_name '/stock']);

% Initialize return values
comp = [];
falsep = [];
neg = [];
total = 0;

func = str2func(func_name);

% Iterate over the filelist
for x=1:length(filelist)
    % Not a directory
    file = filelist(x);
    filepath = fullfile([dir_name '/stock'], file.name);
    if file.isdir == 0
        % Only check file_type
        [path, name, ext, junk] = fileparts(filepath);
        if strcmp(ext(2:length(ext)), 'png') == 1
            % Analyze this file
            disp(['Analyzing file ' name ext])
            total = total + 1;
            
            % Display original image
            A = imread(filepath);
            results = func(A);
            
            % Draw debugging box
            processed = A;
            if size(results) > 0
                processed = drawboundingbox(processed, results(1), ...
                                            0, name, [255 0 0]);
            end
            imwrite(processed, [dir_name '/processed/' file.name]);
            
            % Compare processed results to desired
        end
    end
end

end

