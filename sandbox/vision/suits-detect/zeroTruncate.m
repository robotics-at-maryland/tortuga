W = function zeroTruncate(V)
V = flattenToRow(V);
lowIndex = min(find(V != 0));
highIndex = max(find(V != 0));
W = flattenToColumn(V(lowIndex:highIndex));
