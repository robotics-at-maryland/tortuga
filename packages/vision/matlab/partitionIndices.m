function [indVec] = partitionIndices(nPartitions,nIndices)
% PARTIIONINDICES the function splits the indices, 1:nIndices, into 
% nPartitions if there's any remainder at end, it's put into last partition
%
% inputs:
%   nPartitions - 1x1 = number of partitions to split the indices
%   nIndices - 1x1 = number of indices to split
%
% outputs:
%   indVec = nPartitions x 2 = start,end indices of each partition
%
% Copyright (C) 2009 Joseph Lisee <jlisee@cmu.edu>
% Copyright (C) 2009 Jackie Libby <jlibby@cmu.edu>


cutSize = floor(nIndices/nPartitions);
indVec = zeros(nPartitions,2);
for i = 1:nPartitions
    indVec(i,1) = (i-1)*cutSize + 1;
    indVec(i,2) = i*cutSize;
end
%add the remainder to the last partition
indVec(nPartitions,2) = nIndices;