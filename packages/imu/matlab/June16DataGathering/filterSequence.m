function filteredSequence = filterSequence(sequence,filterSize)
%**************************************************************
% filteredSequence = filterSequence(sequence,filterSize)
%
% smoothes the input sequence with an averaging filter
%
% input: 
%           sequence   - the data sequence to be filtered
%           filterSize - number of points to use in the averaging filter
% output: 
%           filteredSequence - the filtered version of the data sequence
%
% Written by Joseph Gland on June 12, 2007
%***************************************************************

numPoints=length(sequence);
filteredSequence=zeros(1,numPoints);
for i=1:1:numPoints
    if i<filterSize
        numerator=0;
        for j=1:1:i
            numerator=numerator+sequence(j);
        end
        filteredSequence(i)=numerator/i;
    else
        numerator=0;
        for j=i-(filterSize-1):1:i
            numerator=numerator+sequence(j);
        end
        filteredSequence(i)=numerator/filterSize;
    end
end