function [lines] = join_lines(lines, params)
% JOIN_LINES joines together line segments that are parallel and
% whoes ends our close together    
%
% lines - a list of lines returned from the hough detector
% params - a matlab struct with:
%              THETA_THRESH - threshold for allowed angle difference
%              LINE_FILL_GAP - threshold for gap between lines
%              SIDE_DIST_THRESH - threshold for side to side gap
%
% Copyright (C) 2009 Joseph Lisee <jlisee@cmu.edu>

    
changed = 1;
used = zeros(1,length(lines));
usedPos = 1;
angleThres = params.THETA_THRESH;
forwardDistThres = params.LINE_FILL_GAP;
sideDistThres = params.SIDE_DIST_THRESH;

makeUnit = @(A) A/norm(A);

% While something changed
while changed
    changed = 0;
    
    % For each line segment
    for i = setdiff(1:length(lines), used)
        % Grab pt1 & pt2
        pt1 = lines(i).point1;
        pt2 = lines(i).point2;
        theta = lines(i).theta;
        
        % Compute the line as a unit vector and its normal
        lineVec = makeUnit(pt2 - pt1);
        normLineVec = [-lineVec(2) lineVec(1)];

        
        % For each other line segment of similar angle
        for j = setdiff(1:length(lines), used)
            if abs(lines(j).theta - theta) < angleThres
                isUsed = 0;

                dist12 = pt1 - lines(j).point2;
                forwardDist12 = dot(lineVec,dist12);
                sideDist12 = abs(dot(normLineVec,dist12));
                
                dist21 = pt2 - lines(j).point1;
                forwardDist21 = -dot(lineVec,dist21);
                sideDist21 = abs(dot(normLineVec,dist21));

                
                % if pt1 close to other pt2
                if (forwardDist12 < forwardDistThres && ...
                    forwardDist12 > 0 && sideDist12 < sideDistThres)
                    isUsed = 1;
% $$$                     norm(dist12)
% $$$                     forwardDist12
% $$$                     sideDist12
                    % Found line is "behind" this one, so make the
                    % head of the line the new head of this line
                    lines(i).point1 = lines(j).point1;
                elseif (forwardDist21 < forwardDistThres && ...
                        forwardDist21 > 0 && sideDist21 < sideDistThres)
                    isUsed = 1;
% $$$                     norm(dist21)
% $$$                     forwardDist21
% $$$                     sideDist21

                    % Found line is "ahead" this one, so make the
                    % tail of the line the new tail of this line
                    lines(i).point2 = lines(j).point2;
                end

                % Remove current segment (mark used)
                if isUsed
                    used(usedPos) = j;
                    usedPos = usedPos + 1;
                    
                    % mark changed
                    changed = 1;
                end
           end
        end
    end
end

% Filter our lines
lines = lines(setdiff(1:length(lines), used));
    
    
end