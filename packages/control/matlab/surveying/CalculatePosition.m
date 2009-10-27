function [ result a ] = CalculatePosition( M )
%CALCULATEPOSITION Calculates position based on all measurements
%
%   M - containers.Map() object
%       maps char arrays to a Measurement objects
%
%
%   result.x    - x coordinate of result
%   result.sigx - error in x
%   result.y    - y coordinte of result
%   result.sigy - error in y
%
%
%
%
%   Figure out how to find all combinations of measurements valid for
%   plane intersection method.  Possibly add booleans to Measurement object
%   to make this easier.  Send all valid pairs to plane intersection
%   method.  Add results to a list of results.  Figure out how to find
%   measurements that are valid for the angle method and send them to the
%   angle method.  Add these results to the list of results.  Find a way to
%   combine all results to produce a more precice estimate.
%
assert(isa(M,'containers.Map'));

%   Method Validation
%   Retrieve Keys in form of cell array
k = keys(M);
for i = 1:int32(M.Count) % for each key
    v = M(char(k(i))); % get the measurement at that key (k(i)) must be casted to char
    if(isa(v,'Measurement') && v.isActive == 1 && isreal(v.x) && isreal(v.y) && isreal(v.phi) && isreal(v.sigx) && isreal(v.sigy) && isreal(v.sigphi))
        v.planeIntersectionValid = 1;
        if(isreal(v.D) && isreal(v.h) && isreal(v.theta) && isreal(v.sigD) && isreal(v.sigh) && isreal(v.sigtheta))
            v.angleMethodValid = 1;
        else
            v.angleMethodValid = 0;
        end %anglevalidation
    else
        v.planeIntersectionValid = 0;
        v.angleMethodValid = 0;
    end %planevalidation
end %for

% create temporary array to hold all results
a.xObjArr = [];
a.sigXObjArr = [];
a.yObjArr = [];
a.sigYObjArr = [];

% Calculate Results
for i = 1:int32(M.Count)
    mi = M(char(k(i))); % ith measurement
    if(mi.angleMethodValid == 1)
        % Calc Result
        r = AngleMethod(mi);
        % Add results to the result arrays
        a.xObjArr(1+end) = r.xobj;
        a.sigXObjArr(1+end) = r.sigxobj;
        a.yObjArr(1+end) = r.yobj;
        a.sigYObjArr(1+end) = r.sigyobj;
    end %if
    for j = i+1:int32(M.Count) % start with measurement after i to avoid duplication
        mj = M(char(k(j))); % jth measurement
        if(mi.planeIntersectionValid == 1 && mj.planeIntersectionValid == 1)
            % Calc Result
            r = PlaneIntersection(mi,mj);
            % Add results to result arrays
            a.xObjArr(1+end) = r.xobj;
            a.sigXObjArr(1+end) = r.sigxobj;
            a.yObjArr(1+end) = r.yobj;
            a.sigYObjArr(1+end) = r.sigyobj;
        end %if
    end %for
end %for

%initialize sums
xAveNum = 0;
xAveDenom = 0;
yAveNum = 0;
yAveDenom = 0;

% Calculate Weighted Average of xobj and yobj
[m n] = size(a.xObjArr);
for i = 1:n
    xAveNum = xAveNum + (a.xObjArr(i)/(a.sigXObjArr(i))^2);
    xAveDenom = xAveDenom + (1/(a.sigXObjArr(i))^2);
    yAveNum = yAveNum + (a.yObjArr(i)/(a.sigYObjArr(i))^2);
    yAveDenom = yAveDenom + (1/(a.sigYObjArr(i))^2);
end %for



result.xobj = xAveNum/xAveDenom;
result.sigxobj = sqrt(1/xAveDenom);
result.yobj = yAveNum/yAveDenom;
result.sigyobj = sqrt(1/yAveDenom);

end %function

