function [ success, total ] = analyze_directory( dir_name, file_type )
%ANALYZE_DIRECTORY Will iterate over a directory and analyze it using
%                  a named algorithm
%   Iterates over every image in the directory with the indicated file_type
%   and performs and processes the image using the specified algorithm.
%   Then it checks the results with the named results in the corresponding
%   txt file.

% File type cannot be a txt
if strcmp(file_type, 'txt') == 1
    error('File type cannot be a txt');
end

% Directory contents
filelist = dir(dir_name);

% Initialize return values
success = 0;
total = 0;

% Create the grayscale window
figure(1), imshow(zeros(480, 640, 'uint8'));
figure(2), imshow(logical(zeros(480, 640)));

% Iterate over the filelist
for x=1:length(filelist)
    % Not a directory
    file = filelist(x);
    filepath = fullfile(dir_name, file.name);
    if file.isdir == 0
        % Only check file_type
        [path, name, ext, junk] = fileparts(filepath);
        if strcmp(ext(2:length(ext)), file_type) == 1
            % Analyze this file
            disp(['Analyzing file ' name ext])
            total = total + 1;
            
	    % Display original image
            A = imread(filepath);
            figure(1), imagesc(A);

	    % Run any filters and display the new image
            R = sobel_edge_filter(A);
            R = R>25;
	    %R = rgb2hsv(A);
            %R = colorfilter(R, 0, 0.45, 0, 1, 0, 1);
	    
            figure(2), imagesc(R)
            pause(3);
        end
    end
end

close(1)
close(2)

end

